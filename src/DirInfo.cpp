/*
 *   File name: DirInfo.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <algorithm>

#include "DirInfo.h"
#include "DirTree.h"
#include "FileInfoIterator.h"
#include "FileInfoSorter.h"
#include "ExcludeRules.h"
#include "Exception.h"
#include "DebugHelpers.h"

#define DIRECT_CHILDREN_COUNT_SANITY_CHECK 1

using namespace QDirStat;


DirInfo::DirInfo( DirTree * tree,
		  DirInfo * parent,
		  bool	    asDotEntry )
    : FileInfo( tree, parent )
{
    init();

    if ( asDotEntry )
    {
	_isDotEntry = true;
	_name	    = dotEntryName();

	if ( parent )
	{
	    _device = parent->device();
	    _mode   = parent->mode();
	    _uid    = parent->uid();
	    _gid    = parent->gid();
	    _mtime  = parent->mtime();
	}
    }
    else
    {
	_readState  = DirFinished;
    }
}


DirInfo::DirInfo( const QString & filenameWithoutPath,
		  struct stat	* statInfo,
		  DirTree	* tree,
		  DirInfo	* parent )
    : FileInfo( filenameWithoutPath,
		statInfo,
		tree,
		parent )
{
    init();
    _dotEntry = new DirInfo( tree, this, true );
    _directChildrenCount++;
}


DirInfo::DirInfo( DirTree *	  tree,
		  DirInfo *	  parent,
		  const QString & filenameWithoutPath,
		  mode_t	  mode,
		  FileSize	  size,
		  time_t	  mtime )
    : FileInfo( tree,
		parent,
		filenameWithoutPath,
		mode,
		size,
		mtime )
{
    init();
    _dotEntry = new DirInfo( tree, this, true );
    _directChildrenCount++;
}


void DirInfo::init()
{
    _isDotEntry		 = false;
    _isMountPoint	 = false;
    _isExcluded		 = false;
    _summaryDirty	 = false;
    _deletingAll	 = false;
    _locked		 = false;
    _touched		 = false;
    _pendingReadJobs	 = 0;
    _dotEntry		 = 0;
    _firstChild		 = 0;
    _totalSize		 = _size;
    _totalBlocks	 = _blocks;
    _totalItems		 = 0;
    _totalSubDirs	 = 0;
    _totalFiles		 = 0;
    _directChildrenCount = 0;
    _latestMtime	 = _mtime;
    _readState		 = DirQueued;
    _sortedChildren	 = 0;
    _lastSortCol	 = UndefinedCol;
    _lastSortOrder	 = Qt::AscendingOrder;
}


DirInfo::~DirInfo()
{
    clear();
}


void DirInfo::clear()
{
    _deletingAll = true;

    // Recursively delete all children.

    while ( _firstChild )
    {
	FileInfo * nextChild = _firstChild->next();

	if ( _parent )
	    _parent->deletingChild( _firstChild );

	delete _firstChild;
	_firstChild = nextChild; // unlink the old first child
    }


    // Delete the dot entry.

    if ( _dotEntry )
    {
	delete _dotEntry;
	_dotEntry = 0;
    }

    _summaryDirty = true;
    dropSortCache();
}


void DirInfo::reset()
{
    if ( _isDotEntry )
	return;

    if ( _firstChild || _dotEntry )
	clear();

    _readState	     = DirQueued;
    _pendingReadJobs = 0;
    _summaryDirty    = true;

    if ( ! _dotEntry )
    {
	_dotEntry = new DirInfo( _tree, this, true );

	if ( _tree )
	    _tree->childAddedNotify( _dotEntry );
    }

    recalc();
    dropSortCache();
}


void DirInfo::recalc()
{
    // logDebug() << this << endl;

    _totalSize		 = _size;
    _totalBlocks	 = _blocks;
    _totalItems		 = 0;
    _totalSubDirs	 = 0;
    _totalFiles		 = 0;
    _directChildrenCount = 0;
    _latestMtime	 = _mtime;

    FileInfoIterator it( this );

    while ( *it )
    {
	_directChildrenCount++;
	_totalSize    += (*it)->totalSize();
	_totalBlocks  += (*it)->totalBlocks();
	_totalItems   += (*it)->totalItems() + 1;
	_totalSubDirs += (*it)->totalSubDirs();
	_totalFiles   += (*it)->totalFiles();

	if ( (*it)->isDir() )
	    _totalSubDirs++;

	if ( (*it)->isFile() )
	    _totalFiles++;

	time_t childLatestMtime = (*it)->latestMtime();

	if ( childLatestMtime > _latestMtime )
	    _latestMtime = childLatestMtime;

	++it;
    }

    _summaryDirty = false;
}


void DirInfo::setMountPoint( bool isMountPoint )
{
    _isMountPoint = isMountPoint;
}


FileSize DirInfo::totalSize()
{
    if ( _summaryDirty )
	recalc();

    return _totalSize;
}


FileSize DirInfo::totalBlocks()
{
    if ( _summaryDirty )
	recalc();

    return _totalBlocks;
}


int DirInfo::totalItems()
{
    if ( _summaryDirty )
	recalc();

    return _totalItems;
}


int DirInfo::totalSubDirs()
{
    if ( _summaryDirty )
	recalc();

    return _totalSubDirs;
}


int DirInfo::totalFiles()
{
    if ( _summaryDirty )
	recalc();

    return _totalFiles;
}


time_t DirInfo::latestMtime()
{
    if ( _summaryDirty )
	recalc();

    return _latestMtime;
}


int DirInfo::directChildrenCount()
{
    if ( _summaryDirty )
	recalc();

    return _directChildrenCount;
}


int DirInfo::countDirectChildren()
{
    // logDebug() << this << endl;

    _directChildrenCount = 0;

    FileInfo * child = _firstChild;

    while ( child )
    {
	++_directChildrenCount;
	child = child->next();
    }

    if ( _dotEntry )
	++_directChildrenCount;

    return _directChildrenCount;
}


bool DirInfo::isFinished()
{
    return ! isBusy();
}


void DirInfo::setReadState( DirReadState newReadState )
{
    // "aborted" has higher priority than "finished"

    if ( _readState == DirAborted && newReadState == DirFinished )
	return;

    _readState = newReadState;
}


bool DirInfo::isBusy()
{
    if ( _pendingReadJobs > 0 && _readState != DirAborted )
	return true;

    if ( readState() == DirReading ||
	 readState() == DirQueued    )
	return true;

    return false;
}


void DirInfo::setDotEntry( FileInfo *newDotEntry )
{
    if ( newDotEntry )
	_dotEntry = newDotEntry->toDirInfo();
    else
	_dotEntry = 0;

    countDirectChildren();
}


void DirInfo::insertChild( FileInfo *newChild )
{
    CHECK_PTR( newChild );

    if ( newChild->isDir() || _dotEntry == 0 || _isDotEntry )
    {
	/**
	 * Only directories are stored directly in pure directory nodes -
	 * unless something went terribly wrong, e.g. there is no dot entry to use.
	 * If this is a dot entry, store everything it gets directly within it.
	 *
	 * In any of those cases, insert the new child in the children list.
	 *
	 * We don't bother with this list's order - it's explicitly declared to
	 * be unordered, so be warned! We simply insert this new child at the
	 * list head since this operation can be performed in constant time
	 * without the need for any additional lastChild etc. pointers or -
	 * even worse - seeking the correct place for insertion first. This is
	 * none of our business; the corresponding "view" object for this tree
	 * will take care of such niceties.
	 **/
	newChild->setNext( _firstChild );
	_firstChild = newChild;
	newChild->setParent( this );	// make sure the parent pointer is correct

	childAdded( newChild );		// update summaries
    }
    else
    {
	/*
	 * If the child is not a directory, don't store it directly here - use
	 * this entry's dot entry instead.
	 */
	_dotEntry->insertChild( newChild );
    }
}


void DirInfo::childAdded( FileInfo *newChild )
{
    if ( ! _summaryDirty )
    {
	_totalSize   += newChild->size();
	_totalBlocks += newChild->blocks();
	_totalItems++;

	if ( newChild->parent() == this )
	    _directChildrenCount++;

	if ( newChild->isDir() )
	    _totalSubDirs++;

	if ( newChild->isFile() )
	    _totalFiles++;

	if ( newChild->mtime() > _latestMtime )
	    _latestMtime = newChild->mtime();
    }
    else
    {
	// NOP

	/*
	 * Don't bother updating the summary fields if the summary is dirty
	 * (i.e. outdated) anyway: As soon as anybody wants to know some exact
	 * value a complete recalculation of the entire subtree will be
	 * triggered. On the other hand, if nobody wants to know (which is very
	 * likely) we can save this effort.
	 */
    }

    if ( _lastSortCol != ReadJobsCol )
	dropSortCache();

    if ( _parent )
	_parent->childAdded( newChild );
}


void DirInfo::deletingChild( FileInfo * child )
{
    /**
     * When children are deleted, things go downhill: Marking the summary
     * fields as dirty (i.e. outdated) is the only thing that can be done here.
     *
     * The accumulated sizes could be updated (by subtracting this deleted
     * child's values from them), but the latest mtime definitely has to be
     * recalculated: The child now being deleted might just be the one with the
     * latest mtime, and figuring out the second-latest cannot easily be
     * done. So we merely mark the summary as dirty and wait until a recalc()
     * will be triggered from outside - which might as well never happen if
     * nobody wants to know some summary field anyway.
     **/

    _summaryDirty = true;

    if ( _parent )
	_parent->deletingChild( child );

    if ( child->parent() == this )
    {
	if ( ! _deletingAll )
	{
	    /**
	     * Unlink the child from the children's list - but only if this
	     * doesn't happen recursively for all children of this object: No
	     * use bothering about the validity of the children's list if this
	     * will all be history anyway in a moment.
	     **/

	    unlinkChild( child );
	}
	else
	{
	    dropSortCache();
	}
    }
}


void DirInfo::unlinkChild( FileInfo *deletedChild )
{
    if ( deletedChild->parent() != this )
    {
	logError() << deletedChild << " is not a child of " << this
		   << " - cannot unlink from children list!" << endl;
	return;
    }

    dropSortCache();

    if ( deletedChild == _firstChild )
    {
	logDebug() << "Unlinking first child " << deletedChild << endl;
	_firstChild = deletedChild->next();
	return;
    }

    FileInfo *child = firstChild();

    while ( child )
    {
	if ( child->next() == deletedChild )
	{
	    logDebug() << "Unlinking " << deletedChild << endl;
	    child->setNext( deletedChild->next() );

	    return;
	}

	child = child->next();
    }

    logError() << "Couldn't unlink " << deletedChild << " from "
	       << this << " children list" << endl;
}


void DirInfo::readJobAdded()
{
    _pendingReadJobs++;

    if ( _lastSortCol == ReadJobsCol )
	dropSortCache();

    if ( _parent )
	_parent->readJobAdded();
}


void DirInfo::readJobFinished()
{
    _pendingReadJobs--;

    if ( _lastSortCol == ReadJobsCol )
	dropSortCache();

    if ( _parent )
	_parent->readJobFinished();
}


void DirInfo::readJobAborted()
{
    _readState = DirAborted;

    if ( _parent )
	_parent->readJobAborted();
}


void DirInfo::finalizeLocal()
{
    // logDebug() << this << endl;
    cleanupDotEntries();
}


void DirInfo::finalizeAll()
{
    if ( _isDotEntry )
	return;

    FileInfo *child = firstChild();

    while ( child )
    {
	if ( child->isDirInfo() && ! child->isDotEntry() )
	{
	    child->toDirInfo()->finalizeAll();
	}

	child = child->next();
    }

    // Optimization: As long as this directory is not finalized yet, it does
    // (very likely) have a dot entry and thus all direct children are
    // subdirectories, not plain files, so we don't need to bother checking
    // plain file children as well - so do finalizeLocal() only after all
    // children are processed. If this step were the first, for directories
    // that don't have any subdirectories finalizeLocal() would immediately
    // get all their plain file children reparented to themselves, so they
    // would need to be processed in the loop, too.

    if ( _tree )
	_tree->sendFinalizeLocal( this ); // Must be sent _before_ finalizeLocal()!
    finalizeLocal();
}


DirReadState DirInfo::readState() const
{
    if ( _isDotEntry && _parent )
	return _parent->readState();
    else
	return _readState;
}


void DirInfo::cleanupDotEntries()
{
    if ( ! _dotEntry || _isDotEntry )
	return;

    // Reparent dot entry children if there are no subdirectories on this level

    if ( ! _firstChild )
    {
	FileInfo *child = _dotEntry->firstChild();

	if ( child )
	{
	    // logDebug() << "Reparenting children of solo dot entry " << this << endl;

	    _firstChild = child;	    // Move the entire children chain here.
	    _dotEntry->setFirstChild( 0 );  // _dotEntry will be deleted below.
	    _directChildrenCount = -1;

	    while ( child )
	    {
		child->setParent( this );
		child = child->next();
	    }
	}
    }


    // Delete dot entries without any children.
    //
    // This also affects dot entries that were just disowned because they had
    // no siblings (i.e., there are no subdirectories on this level).

    if ( ! _dotEntry->firstChild() )
    {
	// logDebug() << "Removing empty dot entry " << this << endl;

	delete _dotEntry;
	_dotEntry = 0;
	_directChildrenCount = -1;
    }

    if ( _directChildrenCount < 0 )
	countDirectChildren();
}


void DirInfo::clearTouched( bool recursive )
{
    _touched = false;

    if ( recursive && ! _isDotEntry )
    {
	FileInfo * child = _firstChild;

	while ( child )
	{
	    if ( child->isDirInfo() )
		child->toDirInfo()->clearTouched();

	    child = child->next();
	}

	if ( _dotEntry )
	    _dotEntry->clearTouched();
    }
}


const FileInfoList & DirInfo::sortedChildren( DataColumn    sortCol,
					      Qt::SortOrder sortOrder )
{
    if ( _sortedChildren && sortCol == _lastSortCol && sortOrder == _lastSortOrder )
	return *_sortedChildren;


    // Clean old sorted children list and create a new one

    dropSortCache( true ); // recursive
    _sortedChildren = new FileInfoList();
    CHECK_NEW( _sortedChildren );


    // Populate with unsorted children list

    FileInfo * child = _firstChild;

    while ( child )
    {
	_sortedChildren->append( child );
	child = child->next();
    }

    if ( _dotEntry )
	_sortedChildren->append( _dotEntry );


    // Sort

    // logDebug() << "Sorting children of " << this << " by " << sortCol << endl;

    if ( sortCol != NameCol )
    {
	// Do secondary sorting by NameCol (always in ascending order)

	std::stable_sort( _sortedChildren->begin(),
			  _sortedChildren->end(),
			  FileInfoSorter( NameCol, Qt::AscendingOrder ) );
    }


    // Primary sorting by sortCol ascending or descending (as specified in sortOrder)

    std::stable_sort( _sortedChildren->begin(),
		      _sortedChildren->end(),
		      FileInfoSorter( sortCol, sortOrder ) );

    _lastSortCol   = sortCol;
    _lastSortOrder = sortOrder;


#if DIRECT_CHILDREN_COUNT_SANITY_CHECK

    if ( _sortedChildren->size() != _directChildrenCount )
    {
	Debug::dumpChildrenList( this, *_sortedChildren );

	THROW( Exception( QString( "_directChildrenCount of %1 corrupted; is %2, should be %3" )
			  .arg( debugUrl() )
			  .arg( _directChildrenCount )
			  .arg( _sortedChildren->size() ) ) );
    }
#endif

    return *_sortedChildren;
}


void DirInfo::dropSortCache( bool recursive )
{
    if ( _sortedChildren )
    {
	// logDebug() << "Dropping sort cache for " << this << endl;

	// Intentionally deleting the list and creating a new one since
	// QList never shrinks, it always just grows (this is documented):
	// QList.clear() would not free the allocated space.
	//
	// If we get lucky, we won't even need the _sortedChildren list any
	// more if nobody asks for it. This prevents pathological cases where
	// the user opened all tree branches at once (there are menu entries to
	// open to a certain tree level), then closed them again and now opens
	// select branches manually.

	delete _sortedChildren;
	_sortedChildren = 0;

	// Optimization: If this dir didn't have any sort cache, there won't be
	// any in the subtree, either. And dot entries don't have dir children
	// that could have a sort cache.

	if ( recursive && ! _isDotEntry )
	{
	    FileInfo * child = _firstChild;

	    while ( child )
	    {
		if ( child->isDirInfo() )
		    child->toDirInfo()->dropSortCache( recursive );

		child = child->next();
	    }

	    if ( _dotEntry )
		_dotEntry->dropSortCache( recursive );
	}
    }
}


const DirInfo * DirInfo::findNearestMountPoint() const
{
    const DirInfo * dir = this;

    while ( dir && ! dir->isMountPoint() )
	dir = dir->parent();

    return dir;
}
