/*
 *   File name: LocateFilesWindow.cpp
 *   Summary:	QDirStat "locate files" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "LocateFilesWindow.h"
#include "DirTree.h"
#include "FileInfoIterator.h"
#include "DotEntry.h"
#include "SelectionModel.h"
#include "SettingsHelpers.h"
#include "HeaderTweaker.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


LocateFilesWindow::LocateFilesWindow( TreeWalker *     treeWalker,
                                      SelectionModel * selectionModel,
                                      QWidget *	       parent ):
    QDialog( parent ),
    _ui( new Ui::LocateFilesWindow ),
    _treeWalker( treeWalker ),
    _selectionModel( selectionModel )
{
    // logDebug() << "init" << endl;

    CHECK_PTR( _treeWalker );
    CHECK_NEW( _ui );
    _ui->setupUi( this );
    initWidgets();
    readWindowSettings( this, "LocateFilesWindow" );

    connect( _ui->refreshButton, SIGNAL( clicked() ),
	     this,		 SLOT  ( refresh() ) );

    connect( _ui->treeWidget,	 SIGNAL( currentItemChanged( QTreeWidgetItem *,
							     QTreeWidgetItem * ) ),
	     this,		 SLOT  ( locateInMainWindow( QTreeWidgetItem * ) ) );
}


LocateFilesWindow::~LocateFilesWindow()
{
    // logDebug() << "destroying" << endl;

    delete _treeWalker;
    writeWindowSettings( this, "LocateFilesWindow" );
}


void LocateFilesWindow::clear()
{
    _ui->treeWidget->clear();
}


void LocateFilesWindow::setTreeWalker( TreeWalker * newTreeWalker )
{
    CHECK_PTR( newTreeWalker );

    delete _treeWalker;
    _treeWalker = newTreeWalker;
}


void LocateFilesWindow::refresh()
{
    populate( _subtree() );
}


void LocateFilesWindow::initWidgets()
{
    QFont font = _ui->heading->font();
    font.setBold( true );
    _ui->heading->setFont( font );

    _ui->treeWidget->setColumnCount( LocateListColumnCount );
    _ui->treeWidget->setHeaderLabels( QStringList()
				      << tr( "Path" )
				      << tr( "Size" )
				      << tr( "Last Modified" ) );
    _ui->treeWidget->header()->setStretchLastSection( false );
    HeaderTweaker::resizeToContents( _ui->treeWidget->header() );
}


void LocateFilesWindow::reject()
{
    deleteLater();
}


void LocateFilesWindow::populate( FileInfo * newSubtree )
{
    // logDebug() << "populating with " << newSubtree << endl;

    clear();
    _subtree = newSubtree;
    _treeWalker->prepare( _subtree() );

    // For better Performance: Disable sorting while inserting many items
    _ui->treeWidget->setSortingEnabled( false );

    populateRecursive( newSubtree ? newSubtree : _subtree() );

    _ui->treeWidget->setSortingEnabled( true );
    _ui->treeWidget->sortByColumn( LocateListPathCol, Qt::AscendingOrder );

    // Make sure something is selected, even if this window is not the active
    // one (for example because the user just clicked on another suffix in the
    // file type stats window). When the window is activated, the tree widget
    // automatically uses the topmost item as the current item, and in the
    // default selection mode, this item is also selected. When the window is
    // not active, this does not happen yet - until the window is activated.
    //
    // In the context of QDirStat, this means that this is also signaled to the
    // SelectionModel, the corresponding branch in the main window's dir tree
    // is opened, and the matching files are selected in the dir tree and in
    // the treemap.
    //
    // It is very irritating if this only happens sometimes - when the "locate
    // files" window is created, but not when it is just populated with new
    // content from the outside (from the file type stats window).
    //
    // So let's make sure the topmost item is always selected.

    QTreeWidgetItem * firstItem = _ui->treeWidget->topLevelItem( 0 );

    if ( firstItem )
        _ui->treeWidget->setCurrentItem( firstItem );
}


void LocateFilesWindow::populateRecursive( FileInfo * dir )
{
    if ( ! dir )
	return;

    FileInfoIterator it( dir );

    while ( *it )
    {
	FileInfo * item = *it;

        if ( _treeWalker->check( item ) )
        {
            LocateListItem * locateListItem =
                new LocateListItem( item->url(), item->size(), item->mtime() );
            CHECK_NEW( locateListItem );

            _ui->treeWidget->addTopLevelItem( locateListItem );
        }

	if ( item->hasChildren() )
	{
	    populateRecursive( item );
	}

        ++it;
    }
}


void LocateFilesWindow::locateInMainWindow( QTreeWidgetItem * item )
{
    if ( ! item )
	return;

    LocateListItem * searchResult = dynamic_cast<LocateListItem *>( item );
    CHECK_DYNAMIC_CAST( searchResult, "LocateListItem" );
    CHECK_PTR( _subtree.tree() );

    // logDebug() << "Locating " << searchResult->path() << " in tree" << endl;
    _selectionModel->setCurrentItem( searchResult->path() );
}


void LocateFilesWindow::setHeading( const QString & text )
{
    _ui->heading->setText( text );
}


void LocateFilesWindow::sortByColumn( int col, Qt::SortOrder order )
{
    _ui->treeWidget->sortByColumn( col, order );
}




LocateListItem::LocateListItem( const QString & path,
                                FileSize	size,
                                time_t          mtime ):
    QTreeWidgetItem( QTreeWidgetItem::UserType ),
    _path( path ),
    _size( size ),
    _mtime( mtime )
{
    setText( LocateListPathCol,	 path                 + " " );
    setText( LocateListSizeCol,	 formatSize( _size )  + " " );
    setText( LocateListMTimeCol, formatTime( _mtime ) + " " );

    setTextAlignment( LocateListPathCol,	 Qt::AlignLeft	);
    setTextAlignment( LocateListSizeCol,	 Qt::AlignRight );
    setTextAlignment( LocateListMTimeCol,	 Qt::AlignLeft  );
}


bool LocateListItem::operator<( const QTreeWidgetItem & rawOther ) const
{
    // Since this is a reference, the dynamic_cast will throw a std::bad_cast
    // exception if it fails. Not catching this here since this is a genuine
    // error which should not be silently ignored.
    const LocateListItem & other = dynamic_cast<const LocateListItem &>( rawOther );

    int col = treeWidget() ? treeWidget()->sortColumn() : LocateListPathCol;

    switch ( col )
    {
	case LocateListPathCol:  return path()  < other.path();
	case LocateListSizeCol:  return size()  < other.size();
	case LocateListMTimeCol: return mtime() < other.mtime();
	default:	         return QTreeWidgetItem::operator<( rawOther );
    }
}

