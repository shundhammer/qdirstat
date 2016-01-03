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
#include "PercentBar.h"
#include "DirTree.h"
#include "Exception.h"
#include "Logger.h"


using namespace QDirStat;


DirTreeView::DirTreeView( QWidget * parent ):
    QTreeView( parent )
#if 0
    , _selectionModelProxy(0)
#endif
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
    header()->setSectionResizeMode( QHeaderView::ResizeToContents );

    connect( this , SIGNAL( customContextMenuRequested( const QPoint & ) ),
             this,  SLOT  ( contextMenu               ( const QPoint & ) ) );

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
        logDebug() << "No item at this position" << endl;
        return;
    }

    logDebug() << "Context menu for " << this << endl;

    QMenu menu;
    QStringList actions;
    actions << "actionGoUp"
            << "actionCopyUrlToClipboard"
            << "---"
            << "actionRefreshSelected"
            << "actionReadExcludedDirectory"
            << "actionContinueReadingAtMountPoint"
        ;

    ActionManager::instance()->addActions( &menu, actions );

    QAction * selectedAction = menu.exec( mapToGlobal( pos ) );

    if ( selectedAction )
    {
        logDebug() << "Executing action " << selectedAction->objectName() << endl;
        selectedAction->trigger();
    }
    else
    {
        logDebug() << "No action selected" << endl;
    }
}


#if 0
void DirTreeView::setSelectionModel( QItemSelectionModel * selectionModel )
{
    QTreeView::setSelectionModel( selectionModel );
    selectionModel->disconnect( this );

    if ( _selectionModelProxy )
	delete _selectionModelProxy;

    SelectionModel * master = dynamic_cast<SelectionModel *>( selectionModel );
    CHECK_DYNAMIC_CAST( master, "SelectionModel *" );

    _selectionModelProxy = new SelectionModelProxy( master, this );

    connect( _selectionModelProxy, SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
	     this,		   SLOT	 ( selectionChanged( QItemSelection, QItemSelection ) ) );

    connect( _selectionModelProxy, SIGNAL( currentChanged( QModelIndex, QModelIndex ) ),
	     this,		   SLOT	 ( currentChanged( QModelIndex, QModelIndex ) ) );

    connect( _selectionModelProxy, SIGNAL( currentColumnChanged( QModelIndex, QModelIndex ) ),
	     this,		   SLOT	 ( currentColumnChanged( QModelIndex, QModelIndex ) ) );

    connect( _selectionModelProxy, SIGNAL( currentRowChanged( QModelIndex, QModelIndex ) ),
	     this,		   SLOT	 ( currentRowChanged( QModelIndex, QModelIndex ) ) );
}
#endif
