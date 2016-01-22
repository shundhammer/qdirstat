/*
 *   File name: DirTreeView.cpp
 *   Summary:	Tree view widget for directory tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QHeaderView>
#include <QMenu>

#include "DirTreeView.h"
#include "DirTreeModel.h"
#include "SelectionModel.h"
#include "ActionManager.h"
#include "CleanupCollection.h"
#include "PercentBar.h"
#include "DirTree.h"
#include "Exception.h"
#include "Logger.h"


using namespace QDirStat;


DirTreeView::DirTreeView( QWidget * parent ):
    QTreeView( parent ),
    _cleanupCollection(0)
{
    _percentBarDelegate = new PercentBarDelegate( this );
    CHECK_NEW( _percentBarDelegate );

    setItemDelegate( _percentBarDelegate );

    setRootIsDecorated( true );
    setSortingEnabled( true );
    setSelectionMode( ExtendedSelection );
    setContextMenuPolicy( Qt::CustomContextMenu );

    header()->setSortIndicator( NameCol, Qt::AscendingOrder );
    header()->setStretchLastSection( false );

    // TO DO: This is too strict. But it's better than the brain-dead defaults.
#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
    header()->setResizeMode( QHeaderView::ResizeToContents );
#else
    header()->setSectionResizeMode( QHeaderView::ResizeToContents );
#endif

    connect( this , SIGNAL( customContextMenuRequested( const QPoint & ) ),
	     this,  SLOT  ( contextMenu		      ( const QPoint & ) ) );

}


DirTreeView::~DirTreeView()
{
    delete _percentBarDelegate;
}


void DirTreeView::currentChanged( const QModelIndex & current,
				  const QModelIndex & oldCurrent )
{
    // logDebug() << "Setting new current to " << current << endl;
    QTreeView::currentChanged( current, oldCurrent );
    scrollTo( current );
}


void DirTreeView::contextMenu( const QPoint & pos )
{
    QModelIndex index = indexAt( pos );

    if ( ! index.isValid() )
    {
	// logDebug() << "No item at this position" << endl;
	return;
    }

    QMenu menu;
    QStringList actions;
    actions << "actionGoUp"
	    << "actionCopyUrlToClipboard"
	    << "---"
	    << "actionRefreshSelected"
	    << "actionReadExcludedDirectory"
	    << "actionContinueReadingAtMountPoint"
	    << "---"
	    << "actionMoveToTrash"
	;

    ActionManager::instance()->addActions( &menu, actions );

    if ( _cleanupCollection && ! _cleanupCollection->isEmpty() )
    {
	menu.addSeparator();
	_cleanupCollection->addToMenu( &menu );
    }

    menu.exec( mapToGlobal( pos ) );
}


QModelIndexList DirTreeView::expandedIndexes() const
{
    QModelIndexList expandedList;

    if ( ! model() )
	return QModelIndexList();

    DirTreeModel * dirTreeModel = dynamic_cast<DirTreeModel *>( model() );

    if ( ! dirTreeModel )
    {
	logError() << "Wrong model type to get this information" << endl;
	return QModelIndexList();
    }

    foreach ( const QModelIndex & index, dirTreeModel->persistentIndexList() )
    {
	if ( isExpanded( index ) )
	    expandedList << index;
    }

    return expandedList;
}


void DirTreeView::closeAllExcept( const QModelIndex & branch )
{
    QModelIndexList branchesToClose = expandedIndexes();

    // Remove all ancestors of 'branch' from branchesToClose

    QModelIndex index = branch;

    while ( index.isValid() )
    {
	// logDebug() << "Not closing " << index << endl;
	branchesToClose.removeAll( index );
	index = index.parent();
    }

    // Close all items in branchesToClose

    foreach ( index, branchesToClose )
    {
	// logDebug() << "Closing " << index << endl;
	collapse( index );
    }
}

