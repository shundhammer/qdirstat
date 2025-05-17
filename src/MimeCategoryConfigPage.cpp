/*
 *   File name: MimeCategoryConfigPage.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <time.h>	// time()
#include <stdlib.h>	// random(), srandom()
#include <QMessageBox>
#include <QColorDialog>

#include "MimeCategoryConfigPage.h"
#include "DirTree.h"
#include "DirInfo.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


// This is a mess that became necessary because Qt's moc cannot handle template
// classes. Yes, this is ugly.
#define CATEGORY_CAST(VOID_PTR) (static_cast<MimeCategory *>(VOID_PTR))



MimeCategoryConfigPage::MimeCategoryConfigPage( QWidget * parent ):
    ListEditor( parent ),
    _ui( new Ui::MimeCategoryConfigPage ),
    _categorizer( MimeCategorizer::instance() ),
    _dirTree( 0 )
{
    CHECK_NEW( _ui );
    CHECK_PTR( _categorizer );

    _ui->setupUi( this );
    setListWidget  ( _ui->listWidget   );
    setAddButton   ( _ui->addButton    );
    setRemoveButton( _ui->removeButton );

    _ui->treemapView->setFixedColor( Qt::white );
    populateTreemapView();

    connect( _ui->nameLineEdit,	 SIGNAL( textChanged ( QString ) ),
	     this,		 SLOT  ( nameChanged( QString ) ) );

    connect( _ui->colorLineEdit, SIGNAL( textChanged ( QString ) ),
	     this,		 SLOT  ( colorChanged( QString ) ) );

    connect( _ui->colorButton,	 SIGNAL( clicked()   ),
	     this,		 SLOT  ( pickColor() ) );
}


MimeCategoryConfigPage::~MimeCategoryConfigPage()
{
    // logDebug() << "MimeCategoryConfigPage destructor" << endl;

    _ui->treemapView->setDirTree(0);

    if ( _dirTree )
	delete _dirTree;

    delete _ui;
}


void MimeCategoryConfigPage::setup()
{
    fillListWidget();
    updateActions();
}


void MimeCategoryConfigPage::applyChanges()
{
    // logDebug() << endl;

    save( value( listWidget()->currentItem() ) );
    _categorizer->writeSettings();
}


void MimeCategoryConfigPage::discardChanges()
{
    // logDebug() << endl;

    listWidget()->clear();
    _categorizer->readSettings();
}


void MimeCategoryConfigPage::fillListWidget()
{
    CHECK_PTR( _categorizer );
    listWidget()->clear();

    foreach ( MimeCategory * category, _categorizer->categories() )
    {
	QListWidgetItem * item = new ListEditorItem( category->name(), category );
	CHECK_NEW( item );
	listWidget()->addItem( item );
    }

    QListWidgetItem * firstItem = listWidget()->item(0);

    if ( firstItem )
	listWidget()->setCurrentItem( firstItem );
}


void MimeCategoryConfigPage::nameChanged( const QString & newName )
{
    QListWidgetItem * currentItem = listWidget()->currentItem();

    if ( currentItem )
    {
	MimeCategory * category = CATEGORY_CAST( value( currentItem ) );
	category->setName( newName );
	currentItem->setText( newName );
    }
}


void MimeCategoryConfigPage::colorChanged( const QString & newColor )
{
    QListWidgetItem * currentItem = listWidget()->currentItem();

    // Always set the new colour, even if empty or invalid, for the mini-treemap to rebuild
    QColor color( newColor );
    _ui->treemapView->setFixedColor( color );
    _ui->treemapView->rebuildTreemap();

    if ( currentItem )
    {
	MimeCategory * category = CATEGORY_CAST( value( currentItem ) );
	category->setColor( color );
    }
}


void MimeCategoryConfigPage::pickColor()
{
    QListWidgetItem * currentItem = listWidget()->currentItem();

    if ( currentItem )
    {
	MimeCategory * category = CATEGORY_CAST( value( currentItem ) );
	QColor color = category->color();
	color = QColorDialog::getColor( color,
					window(), // parent
					tr( "Category Color" ) );

	if ( color.isValid() )
	{
	    category->setColor( color );
	    _ui->colorLineEdit->setText( color.name() );
	    _ui->treemapView->setFixedColor( color );
	    _ui->treemapView->rebuildTreemap();
	}
    }
}


void MimeCategoryConfigPage::save( void * value )
{
    MimeCategory * category = CATEGORY_CAST( value );
    // logDebug() << category << endl;

    if ( ! category || updatesLocked() )
	return;

    category->clear();
    QString patterns = _ui->caseInsensitivePatternsTextEdit->toPlainText();
    category->addPatterns( patterns.split( "\n" ), Qt::CaseInsensitive );

    patterns = _ui->caseSensitivePatternsTextEdit->toPlainText();
    category->addPatterns( patterns.split( "\n" ), Qt::CaseSensitive );
}


void MimeCategoryConfigPage::load( void * value )
{
    if ( updatesLocked() )
	return;

    MimeCategory * category = CATEGORY_CAST( value );

    // Populate the name and patterns from this category
    _ui->nameLineEdit->setText( category ? category->name() : "" );

    QStringList patternList = category ? category->humanReadablePatternList( Qt::CaseSensitive ) : QStringList();
    setPatternList( _ui->caseSensitivePatternsTextEdit, patternList );

    patternList = category ? category->humanReadablePatternList( Qt::CaseInsensitive ) : QStringList();
    setPatternList( _ui->caseInsensitivePatternsTextEdit, patternList );

    // Set this category colour in the form and mini-treemap
    QColor color = category ? category->color() : QColor();
    _ui->colorLineEdit->setText( color.isValid() ? category->color().name() : "" );
    _ui->treemapView->setFixedColor( color );
   // _ui->treemapView->rebuildTreemap(); // it rebuilds itself when the colour is set
}


void MimeCategoryConfigPage::setPatternList( QPlainTextEdit    * textEdit,
					     const QStringList & patternList )
{
    QString text = patternList.join( "\n" );

    if ( ! text.isEmpty() )
	text += "\n";	   // Let the user begin writing on a new line

    textEdit->setPlainText( text );
}


void * MimeCategoryConfigPage::createValue()
{
    MimeCategory * category = new MimeCategory( "", Qt::white );
    CHECK_NEW( category );
    _categorizer->add( category );

    return category;
}


void MimeCategoryConfigPage::removeValue( void * value )
{
    MimeCategory * category = CATEGORY_CAST( value );
    CHECK_PTR( category );

    _categorizer->remove( category );
}


QString MimeCategoryConfigPage::valueText( void * value )
{
    MimeCategory * category = CATEGORY_CAST( value );
    CHECK_PTR( category );

    return category->name();
}


QString MimeCategoryConfigPage::deleteConfirmationMessage( void * value )
{
    MimeCategory * category = CATEGORY_CAST( value );
    return tr( "Really delete category \"%1\"?" ).arg( category->name() );
}


void MimeCategoryConfigPage::populateTreemapView()
{
    _dirTree = new DirTree();
    CHECK_NEW( _dirTree );

    DirInfo * root    = _dirTree->root();
    mode_t    mode    = 0755;
    FileSize  dirSize = 4096;
    time_t    mtime   = 0;

    // Create a very basic directory structure:
    //
    //	 demo
    //	   dir1
    //	   dir2
    //	     dir21

    DirInfo * topDir = new DirInfo( _dirTree, root, "demo", mode, dirSize, false, 0, 0, mtime );
    CHECK_NEW( topDir );
    root->insertChild( topDir );

    DirInfo * dir1 = new DirInfo( _dirTree, topDir, "dir1", mode, dirSize, false, 0, 0, mtime );
    CHECK_NEW( dir1 );
    topDir->insertChild( dir1 );

    DirInfo * dir2 = new DirInfo( _dirTree, topDir, "dir2", mode, dirSize, false, 0, 0, mtime );
    CHECK_NEW( dir2 );
    topDir->insertChild( dir2 );

    DirInfo * dir21 = new DirInfo( _dirTree, dir2, "dir21", mode, dirSize, false, 0, 0, mtime );
    CHECK_NEW( dir21 );
    dir2->insertChild( dir21 );

    // Collect all directories in a list to pick from at random

    QList<DirInfo *> dirs;
    dirs << topDir << dir1 << dir2 << dir21;

    srandom( (unsigned) time(0) ); // Seed random number generator
    int	     fileCount = random() % 30 + 12;
    FileSize maxSize   = 100*1024*1024;	// 100 MB


    // Generate a random number of files with random sizes

    for ( int i=0; i < fileCount; i++ )
    {
	// Pick a random directory as parent

	int dirNo = random() % dirs.size();
	DirInfo * parent = dirs.at( dirNo );

	// Select a random file size
	FileSize fileSize = random() % maxSize;

	// Create a FileInfo item and add it to the parent
	FileInfo * file = new FileInfo( _dirTree, parent,
					QString( "File_%1" ).arg( i ),
					mode,
                                        false, 0, 0, // withUidGid, uid, gid
                                        fileSize, mtime );
	CHECK_NEW( file );
	parent->insertChild( file );
    }

    logDebug() << "Demo tree: " << fileCount << " files with "
	       << formatSize( topDir->totalSize() ) << " total"
	       << ENDL;

    _ui->treemapView->setDirTree( _dirTree );
}


void MimeCategoryConfigPage::updateActions()
{
    ListEditor::updateActions();

    setActions( listWidget()->currentItem() );
}


void MimeCategoryConfigPage::currentItemChanged( QListWidgetItem * current,
                                                 QListWidgetItem * previous)
{
    //logDebug() << current << ", " << previous << endl;

    ListEditor::currentItemChanged( current, previous );

    setActions( current );
}


void MimeCategoryConfigPage::setActions( const QListWidgetItem * currentItem )
{
    const bool isSymlink = currentItem && currentItem->text() == CATEGORY_SYMLINKS;
    const bool isExecutable = currentItem && currentItem->text() == CATEGORY_EXECUTABLES;

    // Name can't be changed for symlinks and executables
    _ui->nameLineEdit->setEnabled( currentItem && !isSymlink && !isExecutable );

    // Patterns can't be changed for symlinks
    _ui->patternsTopWidget->setEnabled( currentItem && !isSymlink );
    _ui->patternsBottomWidget->setEnabled( currentItem && !isSymlink );

    // Symlinks and executables can't be removed
    enableButton( _ui->removeButton, currentItem && !isSymlink && !isExecutable );

    // Colour can be edited for any item
    _ui->colorLineEdit->setEnabled( currentItem );
    enableButton( _ui->colorButton, currentItem );
}
