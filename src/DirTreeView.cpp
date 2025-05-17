/*
 *   File name: DirTreeView.cpp
 *   Summary:	Tree view widget for directory tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMenu>
#include <QKeyEvent>

#include "DirTreeView.h"
#include "DirTreeModel.h"
#include "SelectionModel.h"
#include "ActionManager.h"
#include "CleanupCollection.h"
#include "PercentBar.h"
#include "SizeColDelegate.h"
#include "HeaderTweaker.h"
#include "DirTree.h"
#include "FormatUtil.h"
#include "Exception.h"
#include "Logger.h"

using namespace QDirStat;


DirTreeView::DirTreeView( QWidget * parent ):
    QTreeView( parent ),
    _cleanupCollection(0)
{
    _percentBarDelegate = new DirTreePercentBarDelegate( this, PercentBarCol );
    CHECK_NEW( _percentBarDelegate );
    setItemDelegateForColumn( PercentBarCol, _percentBarDelegate );

    _sizeColDelegate = new SizeColDelegate( this );
    CHECK_NEW( _sizeColDelegate );
    setItemDelegateForColumn( SizeCol, _sizeColDelegate );

    setRootIsDecorated( true );
    setSortingEnabled( true );
    setSelectionMode( ExtendedSelection );
    setContextMenuPolicy( Qt::CustomContextMenu );
    setTextElideMode( Qt::ElideMiddle );
    setUniformRowHeights( true ); // Important for very large directories

    _headerTweaker = new HeaderTweaker( header(), this );
    CHECK_NEW( _headerTweaker );

    connect( this , SIGNAL( customContextMenuRequested( const QPoint & ) ),
	     this,  SLOT  ( contextMenu		      ( const QPoint & ) ) );
}


DirTreeView::~DirTreeView()
{
    delete _headerTweaker;
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

    DataColumn col  = DataColumns::fromViewCol( index.column() );
    FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
    CHECK_MAGIC( item );

    if ( col == SizeCol )
	sizeContextMenu( pos, item );
    else
	actionContextMenu( pos, item );
}


void DirTreeView::actionContextMenu( const QPoint & pos, FileInfo * item )
{
    QMenu menu;
    QStringList actions;

    // The first action should not be a destructive one like "move to trash":
    // It's just too easy to select and execute the first action accidentially,
    // especially on a laptop touchpad.

    actions << "actionGoUp"
            << "actionGoToToplevel"
            << "---"
            << "actionMoveToTrash"
        ;

    // Intentionally adding unconditionally, even if disabled
    ActionManager::instance()->addActions( &menu, actions );


    // User-defined cleanups

    if ( _cleanupCollection )
	_cleanupCollection->addEnabledToMenu( &menu );

    // Less commonly used menu options
    actions.clear();
    actions << "---"
	    << "actionRefreshSelected"
	    << "actionReadExcludedDirectory"
	    << "actionContinueReadingAtMountPoint"
        ;

    ActionManager::instance()->addEnabledActions( &menu, actions );

    // Submenu for the auxiliary views to keep the context menu short.
    //
    // Those actions are strictly speaking irrelevant in most cases, and so
    // they should be omitted from a context menu. But here this serves for
    // discoverability: Most users don't even know that it is an option to
    // start any of those views from a subdirectory in the tree. As a
    // compromise to keep the context menu short, those auxiliary views go to a
    // submenu of the context menu. Submenus in context menus are generally
    // also discouraged, but here discoverability of these features is more
    // important.

    if ( item->isDirInfo() )    // Not for files, symlinks etc.
    {
        QMenu * subMenu = menu.addMenu( tr( "View in" ) );

        actions.clear();
        actions << "actionFileSizeStats"
                << "actionFileTypeStats"
                << "actionFileAgeStats"
            ;

        ActionManager::instance()->addActions( subMenu, actions );
    }

    menu.exec( mapToGlobal( pos ) );
}


void DirTreeView::sizeContextMenu( const QPoint & pos, FileInfo * item )
{
    if ( item->totalSize() >= 1024 ||
         item->totalAllocatedSize() > item->totalSize() )
    {
        QString text = DirTreeModel::sizeText( item, formatByteSize );

        if ( text.isEmpty() )
        {
            text = item->sizePrefix() + formatByteSize( item->totalSize() );

            if ( item->allocatedSize() > item->totalSize() )
            {
                text += tr( " (allocated: %1)" )
                    .arg( formatByteSize( item->totalAllocatedSize() ) );
            }
        }

        QMenu menu;
        menu.addAction( text );
        menu.exec( mapToGlobal( pos ) );
    }
}


QModelIndexList DirTreeView::expandedIndexes() const
{
    QModelIndexList expandedList;

    if ( ! model() )
	return QModelIndexList();

    DirTreeModel * dirTreeModel = dynamic_cast<DirTreeModel *>( model() );

    if ( ! dirTreeModel )
    {
	logError() << "Wrong model type to get this information" << ENDL;
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

    scrollTo( currentIndex(), QAbstractItemView::PositionAtCenter );
}


void DirTreeView::setExpanded( FileInfo * item, bool expanded )
{
    DirTreeModel * dirTreeModel = dynamic_cast<DirTreeModel *>( model() );

    if ( ! dirTreeModel )
    {
	logError() << "Wrong model type" << ENDL;
        return;
    }

    QModelIndex index = dirTreeModel->modelIndex( item );

    if ( index.isValid() )
        QTreeView::setExpanded( index, expanded );
}


void DirTreeView::mousePressEvent( QMouseEvent * event )
{
    if ( event )
    {
        switch ( event->button() )
        {
            // Leave the the back / forward buttons on the mouse to act like the
            // history back / forward buttons in the tool bar.
            //
            // By default, the QTreeView parent class uses them to act as
            // cursor up / cursor down in the tree which defeats the idea of
            // using them as history consistently throughout the application,
            // making those mouse buttons pretty much unusable.
            //
            // So this makes sure those events are immediately propagated up to
            // the parent widget.

            case Qt::BackButton:
            case Qt::ForwardButton:
                event->ignore();
                break;

            default:
                QTreeView::mousePressEvent( event );
                break;
        }
    }
}


void DirTreeView::keyPressEvent( QKeyEvent * event )
{
    if ( event )
    {
        switch ( event->key() )
        {
            case Qt::Key_Asterisk:
                // By default, this opens all tree branches which completely
                // kills our performance, negating all our lazy sorting in
                // each branch in the DirTreeModel / DirInfo classes.
                //
                // So let's just ignore this key; we have better alternatives
                // with "Tree" -> "Expand to Level" -> "Level 0" .. "Level 5".
                return;

            default:
                QTreeView::keyPressEvent( event );
                break;
        }
    }
}
