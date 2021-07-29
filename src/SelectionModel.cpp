/*
 *   File name: SelectionModel.cpp
 *   Summary:	Handling of selected items
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "SelectionModel.h"
#include "DirTreeModel.h"
#include "DirTree.h"
#include "DirInfo.h"
#include "SignalBlocker.h"
#include "Exception.h"
#include "Logger.h"

#define VERBOSE_SELECTION	0

using namespace QDirStat;


SelectionModel::SelectionModel( DirTreeModel * dirTreeModel, QObject * parent ):
    QItemSelectionModel( dirTreeModel, parent ),
    _dirTreeModel( dirTreeModel ),
    _currentItem(0),
    _currentBranch(0),
    _selectedItemsDirty(false),
    _verbose(false)
{
    connect( this, SIGNAL( currentChanged	  ( QModelIndex, QModelIndex ) ),
	     this, SLOT	 ( propagateCurrentChanged( QModelIndex, QModelIndex ) ) );

    connect( this, SIGNAL( selectionChanged	    ( QItemSelection, QItemSelection  ) ),
	     this, SLOT	 ( propagateSelectionChanged( QItemSelection, QItemSelection  ) ) );

    connect( dirTreeModel->tree(), SIGNAL( deletingChild      ( FileInfo * ) ),
	     this,		   SLOT	 ( deletingChildNotify( FileInfo * ) ) );

    connect( dirTreeModel->tree(), SIGNAL( clearing() ),
	     this,		   SLOT	 ( clear()    ) );
}


SelectionModel::~SelectionModel()
{
    // NOP
}


void SelectionModel::clear()
{
    _selectedItems.clear();
    _selectedItemsDirty = true;
    _currentItem = 0;
    _currentBranch = 0;

    clearSelection();
}


FileInfoSet SelectionModel::selectedItems()
{
    if ( _selectedItemsDirty )
    {
	// Build set of selected items from the selected model indexes

	_selectedItems.clear();

	foreach ( const QModelIndex index, selectedIndexes() )
	{
	    if ( index.isValid() )
	    {
		FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
		CHECK_MAGIC( item );

		// logDebug() << "Adding " << item << " col " << index.column() << " to selected items" << endl;
		_selectedItems << item;
	    }
	}

	_selectedItemsDirty = false;
    }

    return _selectedItems;
}


void SelectionModel::propagateCurrentChanged( const QModelIndex & newCurrentIndex,
					      const QModelIndex & oldCurrentIndex )
{
    _currentItem = 0;

    if ( newCurrentIndex.isValid() )
    {
	_currentItem = static_cast<FileInfo *>( newCurrentIndex.internalPointer() );
	CHECK_MAGIC( _currentItem );
    }

    FileInfo * oldCurrentItem = 0;

    if ( oldCurrentIndex.isValid() )
    {
	oldCurrentItem = static_cast<FileInfo *>( oldCurrentIndex.internalPointer() );
	CHECK_MAGIC( oldCurrentItem );
    }

    emit currentItemChanged( _currentItem, oldCurrentItem );
}


void SelectionModel::propagateSelectionChanged( const QItemSelection & selected,
						const QItemSelection & deselected )
{
    Q_UNUSED( selected );
    Q_UNUSED( deselected );

    _selectedItemsDirty = true;
    emit selectionChanged();
    emit selectionChanged( selectedItems() );
}


void SelectionModel::selectItem( FileInfo * item )
{
    extendSelection( item,
		     true ); // clear
}


void SelectionModel::extendSelection( FileInfo * item, bool clear )
{
    if ( item )
    {
	QModelIndex index = _dirTreeModel->modelIndex( item, 0 );

	if ( index.isValid() )
	{
	    logDebug() << "Selecting " << item << endl;
	    SelectionFlags flags = Select | Rows;

	    if ( clear )
		flags |= Clear;

	    select( index, flags ); // emits selectionChanged()
	}
    }
    else
    {
	if ( clear )
	    clearSelection(); // emits selectionChanged()
    }
}


void SelectionModel::setSelectedItems( const FileInfoSet & selectedItems )
{
    if ( _verbose )
	logDebug() << "Selecting " << selectedItems.size() << " items" << endl;

    QItemSelection sel;

    foreach ( FileInfo * item, selectedItems )
    {
	QModelIndex index = _dirTreeModel->modelIndex( item, 0 );

	if ( index.isValid() )
	     sel.merge( QItemSelection( index, index ), Select );
    }

    select( sel, Clear | Select | Rows );
}


void SelectionModel::setCurrentItem( FileInfo * item, bool select )
{
    if ( _verbose )
	logDebug() << item << " select: " << select << endl;

    if ( select )
	clear();

    _currentItem = item;

    if ( item )
    {
	QModelIndex index = _dirTreeModel->modelIndex( item, 0 );

	if ( index.isValid() )
	{
	    if ( _verbose )
		logDebug() << "Setting current to " << index << endl;

	    setCurrentIndex( index, select ? ( Current | Select | Rows ) : Current );
	}
	else
	{
	    logError() << "NOT FOUND in dir tree: " << item << endl;
	}
    }
    else
    {
#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
	setCurrentIndex( QModelIndex(), Current );
#else
	clearCurrentIndex();
#endif
    }
}


void SelectionModel::setCurrentItem( const QString & path )
{
    FileInfo * item = _dirTreeModel->tree()->locate( path,
						     true ); // findPseudoDirs
    if ( item )
	setCurrentItem( item, true );
    else
	logError() << "No item with path " << path << endl;
}


void SelectionModel::setCurrentBranch( FileInfo * item )
{
    _currentBranch = item;
    QModelIndex index = _dirTreeModel->modelIndex( item, 0 );

    emit currentBranchChanged( item  );
    emit currentBranchChanged( index );
}


void SelectionModel::prepareRefresh( const FileInfoSet & refreshSet )
{
    FileInfo * current = _currentItem ? _currentItem : refreshSet.first();
    DirInfo  * dir = 0;

    if ( current )
    {
	dir = current->isDirInfo() ? current->toDirInfo() : current->parent();

	if ( dir && dir->isPseudoDir() )
	    dir = dir->parent();

	// Go one directory up from the current item as long as there is an
	// ancestor (but not that item itself) in the refreshSet.

	while ( dir && refreshSet.containsAncestorOf( dir ) )
	{
	    dir = dir->parent();
	}
    }

    if ( _verbose )
	logDebug() << "Selecting " << dir << endl;

    setCurrentItem( dir, true );
    setCurrentBranch( dir );
}


void SelectionModel::deletingChildNotify( FileInfo * deletedChild )
{
    _selectedItemsDirty = true;
    _selectedItems.clear();

    if ( _currentItem->isInSubtree( deletedChild ) )
	setCurrentItem( 0 );
}


void SelectionModel::dumpSelectedItems()
{
    logDebug() << "Current item: " << _currentItem << endl;
    logDebug() << selectedItems().size() << " items selected" << endl;

    foreach ( FileInfo * item, selectedItems() )
    {
	logDebug() << "	 Selected: " << item << endl;
    }

    logNewline();
}






SelectionModelProxy::SelectionModelProxy( SelectionModel * master, QObject * parent ):
    QObject( parent )
{
    connect( master, SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
	     this,   SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ) );

    connect( master, SIGNAL( currentChanged( QModelIndex, QModelIndex ) ),
	     this,   SIGNAL( currentChanged( QModelIndex, QModelIndex ) ) );

    connect( master, SIGNAL( currentColumnChanged( QModelIndex, QModelIndex ) ),
	     this,   SIGNAL( currentColumnChanged( QModelIndex, QModelIndex ) ) );

    connect( master, SIGNAL( currentRowChanged	 ( QModelIndex, QModelIndex ) ),
	     this,   SIGNAL( currentRowChanged	 ( QModelIndex, QModelIndex ) ) );

    connect( master, SIGNAL( selectionChanged() ),
	     this,   SIGNAL( selectionChanged() ) );

    connect( master, SIGNAL( selectionChanged( FileInfoSet ) ),
	     this,   SIGNAL( selectionChanged( FileInfoSet ) ) );

    connect( master, SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,   SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ) );
}

