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
#include "FileInfo.h"
#include "FileInfo.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


#define CONNECT_BUTTON(BUTTON, RCVR_SLOT) \
    connect( (BUTTON), SIGNAL( clicked() ), this, SLOT( RCVR_SLOT ) )


MimeCategoryConfigPage::MimeCategoryConfigPage( QWidget * parent ):
    QWidget( parent ),
    _ui( new Ui::MimeCategoryConfigPage ),
    _dirTree( 0 ),
    _updatesLocked( false )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );
    _listWidget = _ui->listWidget; // shortcut for a frequently needed widget
    _ui->treemapView->setFixedColor( Qt::white );
    populateTreemapView();

    connect( _listWidget, SIGNAL( currentItemChanged( QListWidgetItem *,
						      QListWidgetItem *	 ) ),
	     this,	  SLOT	( currentItemChanged( QListWidgetItem *,
						      QListWidgetItem *	 ) ) );

    connect( _ui->nameLineEdit,	 SIGNAL( textChanged ( QString ) ),
	     this,		 SLOT  ( nameChanged( QString ) ) );

    connect( _ui->colorLineEdit, SIGNAL( textChanged ( QString ) ),
	     this,		 SLOT  ( colorChanged( QString ) ) );

    CONNECT_BUTTON( _ui->addButton,    add()	   );
    CONNECT_BUTTON( _ui->removeButton, remove()    );
    CONNECT_BUTTON( _ui->colorButton,  pickColor() );
}


MimeCategoryConfigPage::~MimeCategoryConfigPage()
{
    logDebug() << "MimeCategoryConfigPage destructor" << endl;

    _ui->treemapView->setDirTree(0);

    if ( _dirTree )
	delete _dirTree;
}


void MimeCategoryConfigPage::setup()
{
    fillList();
    updateActions();
}


void MimeCategoryConfigPage::applyChanges()
{
    logDebug() << endl;

    save( category( _listWidget->currentItem() ) );
    _categorizer->writeSettings();
}


void MimeCategoryConfigPage::discardChanges()
{
    logDebug() << endl;

    _listWidget->clear();
    _categorizer->clear();
    _categorizer->readSettings();
}


void MimeCategoryConfigPage::fillList()
{
    CHECK_PTR( _categorizer );
    _listWidget->clear();

    foreach ( MimeCategory * category, _categorizer->categories() )
    {
	CategoryListItem * item = new CategoryListItem( category );
	CHECK_NEW( item );
	_listWidget->addItem( item );
    }

    QListWidgetItem * firstItem = _listWidget->item(0);

    if ( firstItem )
        _listWidget->setCurrentItem( firstItem );
}


MimeCategory * MimeCategoryConfigPage::category( QListWidgetItem * item )
{
    if ( ! item )
	return 0;

    CategoryListItem * configListItem = dynamic_cast<CategoryListItem *>( item );
    CHECK_DYNAMIC_CAST( configListItem, "CategoryListItem *" );

    return configListItem->category();
}


void MimeCategoryConfigPage::currentItemChanged(QListWidgetItem * currentItem,
						QListWidgetItem * previousItem )
{
    save( category( previousItem ) );
    load( category( currentItem	 ) );
    updateActions();
}


void MimeCategoryConfigPage::nameChanged( const QString & newName )
{
    QListWidgetItem * currentItem = _listWidget->currentItem();

    if ( currentItem )
    {
	MimeCategory * category = this->category( currentItem );
	category->setName( newName );
	currentItem->setText( newName );
    }
}


void MimeCategoryConfigPage::colorChanged( const QString & newColor )
{
    QListWidgetItem * currentItem = _listWidget->currentItem();

    if ( currentItem )
    {
	QColor color( newColor );

	if ( color.isValid() )
	{
	    MimeCategory * category = this->category( currentItem );
	    category->setColor( color );
	    _ui->treemapView->setFixedColor( color );
	    _ui->treemapView->rebuildTreemap();
	}
    }
}


void MimeCategoryConfigPage::pickColor()
{
    QListWidgetItem * currentItem = _listWidget->currentItem();

    if ( currentItem )
    {
	MimeCategory * category = this->category( currentItem );
	QColor color = category->color();
	color = QColorDialog::getColor( color,
					window(), // parent
					tr( "Category Color" ) );

	if ( color.isValid() )
	{
	    MimeCategory * category = this->category( currentItem );
	    category->setColor( color );
	    _ui->colorLineEdit->setText( color.name() );
	    _ui->treemapView->setFixedColor( color );
	    _ui->treemapView->rebuildTreemap();
	}
    }
}


void MimeCategoryConfigPage::updateActions()
{
#if 0
    int currentRow = _listWidget->currentRow();
    int count	   = _listWidget->count();
#endif
}


void MimeCategoryConfigPage::save( MimeCategory * category )
{
    // logDebug() << category << endl;

    if ( ! category || _updatesLocked )
	return;

    category->clear();
    QString patterns = _ui->caseInsensitivePatternsTextEdit->toPlainText();
    category->addPatterns( patterns.split( "\n" ), Qt::CaseInsensitive );

    patterns = _ui->caseSensitivePatternsTextEdit->toPlainText();
    category->addPatterns( patterns.split( "\n" ), Qt::CaseSensitive );
}


void MimeCategoryConfigPage::load( MimeCategory * category )
{
    // logDebug() << category << endl;

    if ( ! category || _updatesLocked )
	return;

    _ui->nameLineEdit->setText( category->name() );
    QColor color = category->color();
    _ui->colorLineEdit->setText( color.isValid() ?
				 category->color().name() : "" );

    setPatternList( _ui->caseInsensitivePatternsTextEdit,
		    category->humanReadablePatternList( Qt::CaseInsensitive ) );

    setPatternList( _ui->caseSensitivePatternsTextEdit,
		    category->humanReadablePatternList( Qt::CaseSensitive ) );

    _ui->treemapView->setFixedColor( color.isValid() ? color : Qt::red );
    _ui->treemapView->rebuildTreemap();
}


void MimeCategoryConfigPage::setPatternList( QPlainTextEdit    * textEdit,
					     const QStringList & patternList )
{
    textEdit->setPlainText( patternList.join( "\n" ) );
}


void MimeCategoryConfigPage::add()
{
    MimeCategory * category = new MimeCategory( "", Qt::white );
    CHECK_NEW( category );

    CategoryListItem * item = new CategoryListItem( category );
    CHECK_NEW( item );

    _categorizer->add( category );
    _listWidget->addItem( item );
    _listWidget->setCurrentItem( item );
}


void MimeCategoryConfigPage::remove()
{
    QListWidgetItem * currentItem = _listWidget->currentItem();
    int currentRow		  = _listWidget->currentRow();

    if ( ! currentItem )
	return;

    MimeCategory * category = this->category( currentItem );

    //
    // Confirmation popup
    //

    QString msg = tr( "Really delete category \"%1\"?" ).arg( category->name() );
    int ret = QMessageBox::question( window(),
				     tr( "Please Confirm" ), // title
				     msg );
    if ( ret == QMessageBox::Yes )
    {
	//
	// Delete current category
	//

	_updatesLocked = true;
	_listWidget->takeItem( currentRow );
	delete currentItem;
	_categorizer->remove( category );
	_updatesLocked = false;

	load( this->category( _listWidget->currentItem() ) );
    }
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

    DirInfo * topDir = new DirInfo( _dirTree, root, "demo", mode, dirSize, mtime );
    CHECK_NEW( topDir );
    root->insertChild( topDir );

    DirInfo * dir1 = new DirInfo( _dirTree, topDir, "dir1", mode, dirSize, mtime );
    CHECK_NEW( dir1 );
    topDir->insertChild( dir1 );

    DirInfo * dir2 = new DirInfo( _dirTree, topDir, "dir2", mode, dirSize, mtime );
    CHECK_NEW( dir2 );
    topDir->insertChild( dir2 );

    DirInfo * dir21 = new DirInfo( _dirTree, dir2, "dir21", mode, dirSize, mtime );
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
					mode, fileSize, mtime );
	CHECK_NEW( file );
	parent->insertChild( file );
    }

    logDebug() << "Demo tree: " << fileCount << " files with "
	       << formatSize( topDir->totalSize() ) << " total"
	       << endl;

    _ui->treemapView->setDirTree( _dirTree );
}

