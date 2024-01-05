/*
 *   File name: LocateFilesWindow.cpp
 *   Summary:	QDirStat "locate files" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMenu>

#include "LocateFilesWindow.h"
#include "QDirStatApp.h"        // SelectionModel, CleanupCollection
#include "TreeWalker.h"
#include "FileInfoIterator.h"
#include "SelectionModel.h"
#include "ActionManager.h"
#include "CleanupCollection.h"
#include "SettingsHelpers.h"
#include "HeaderTweaker.h"
#include "QDirStatApp.h"        // dirTreeModel()
#include "DirTreeModel.h"       // itemTypeIcon()
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


LocateFilesWindow::LocateFilesWindow( TreeWalker * treeWalker,
                                      QWidget    * parent ):
    QDialog( parent ),
    _ui( new Ui::LocateFilesWindow ),
    _treeWalker( treeWalker ),
    _sortCol( LocateListPathCol ),
    _sortOrder( Qt::AscendingOrder )
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

    connect( _ui->treeWidget,    SIGNAL( customContextMenuRequested( const QPoint & ) ),
             this,               SLOT  ( itemContextMenu           ( const QPoint & ) ) );
}


LocateFilesWindow::~LocateFilesWindow()
{
    // logDebug() << "destroying" << endl;

    writeWindowSettings( this, "LocateFilesWindow" );
    delete _treeWalker;
    delete _ui;
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
    selectFirstItem();
}


void LocateFilesWindow::initWidgets()
{
    QFont font = _ui->heading->font();
    font.setBold( true );
    _ui->heading->setFont( font );

    _ui->treeWidget->setContextMenuPolicy( Qt::CustomContextMenu );
    _ui->treeWidget->setColumnCount( LocateListColumnCount );
    _ui->treeWidget->setHeaderLabels( QStringList()
				      << tr( "Size" )
				      << tr( "Last Modified" )
				      << tr( "Path" )  );
    _ui->treeWidget->header()->setStretchLastSection( false );
    HeaderTweaker::resizeToContents( _ui->treeWidget->header() );
    _ui->resultsLabel->setText( "" );
    addCleanupHotkeys();
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
    showResultsCount();

    _ui->treeWidget->setSortingEnabled( true );
    _ui->treeWidget->sortByColumn( _sortCol, _sortOrder );
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
            LocateListItem * locateListItem = new LocateListItem( item );
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


void LocateFilesWindow::showResultsCount()
{
    QString text;
    int     count = _ui->treeWidget->topLevelItemCount();

    if ( _treeWalker->overflow() )
    {
        text = tr( "Limited to %1 Results" ).arg( count );
    }
    else
    {
        text = tr( "%1 Results" ).arg( count );
    }

    _ui->resultsLabel->setText( text );

}


void LocateFilesWindow::selectFirstItem()
{
    QTreeWidgetItem * firstItem = _ui->treeWidget->topLevelItem( 0 );

    if ( firstItem )
        _ui->treeWidget->setCurrentItem( firstItem );
}


void LocateFilesWindow::locateInMainWindow( QTreeWidgetItem * item )
{
    if ( ! item )
	return;

    LocateListItem * searchResult = dynamic_cast<LocateListItem *>( item );
    CHECK_DYNAMIC_CAST( searchResult, "LocateListItem" );
    CHECK_PTR( _subtree.tree() );

    // logDebug() << "Locating " << searchResult->path() << " in tree" << endl;
    app()->selectionModel()->setCurrentItem( searchResult->path() );
}


void LocateFilesWindow::itemContextMenu( const QPoint & pos )
{
    QMenu menu;
    QStringList actions;
    actions << "actionMoveToTrash";

    ActionManager::instance()->addEnabledActions( &menu, actions );

    if ( app()->cleanupCollection() )
        app()->cleanupCollection()->addEnabledToMenu( &menu );

    menu.exec( _ui->treeWidget->mapToGlobal( pos ) );
}


void LocateFilesWindow::addCleanupHotkeys()
{
    ActionManager::instance()->addActions( this,
                                           QStringList()
                                           << "actionMoveToTrash"
                                           << "actionFindFiles"   );

    if ( app()->cleanupCollection() )
    {
        foreach ( Cleanup * cleanup, app()->cleanupCollection()->cleanupList() )
        {
            if ( cleanup->worksForFile() && ! cleanup->shortcut().isEmpty() )
                addAction( cleanup );
        }
    }
}


void LocateFilesWindow::setHeading( const QString & text )
{
    _ui->heading->setText( text );
}


void LocateFilesWindow::sortByColumn( int col, Qt::SortOrder order )
{
    _sortCol   = col;
    _sortOrder = order;

    _ui->treeWidget->sortByColumn( _sortCol, _sortOrder );
    selectFirstItem();
}




LocateListItem::LocateListItem( FileInfo * item ):
    QTreeWidgetItem( QTreeWidgetItem::UserType )
{
    CHECK_PTR( item );

    _path  = item->url();
    _size  = item->totalSize();
    _mtime = item->mtime();

    QIcon icon = app()->dirTreeModel()->itemTypeIcon( item );

    setText( LocateListSizeCol,	 formatSize( _size )  + " " );
    setText( LocateListMTimeCol, formatTime( _mtime ) + " " );
    setText( LocateListPathCol,	 _path                + " " );
    setIcon( LocateListPathCol,  icon );

    setTextAlignment( LocateListSizeCol,	 Qt::AlignRight );
    setTextAlignment( LocateListMTimeCol,	 Qt::AlignLeft  );
    setTextAlignment( LocateListPathCol,	 Qt::AlignLeft	);
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

