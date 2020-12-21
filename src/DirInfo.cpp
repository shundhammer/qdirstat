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
#include "DotEntry.h"
#include "Attic.h"
#include "FileInfoIterator.h"
#include "FileInfoSorter.h"
#include "ExcludeRules.h"
#include "Exception.h"
#include "DebugHelpers.h"

#define DIRECT_CHILDREN_COUNT_SANITY_CHECK 0

using namespace QDirStat;


DirInfo::DirInfo( DirTree * tree,
		  DirInfo * parent )
    : FileInfo( tree, parent )
{
    init();
    _readState = DirFinished;
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
    ensureDotEntry();

    _directChildrenCount++;	// One for the newly created dot entry
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
    ensureDotEntry();

    _directChildrenCount++;	// One for the newly created dot entry
}


void DirInfo::init()
{
    _dotEntry		 = 0;
    _attic		 = 0;
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
    _totalAllocatedSize	 = _allocatedSize;
    _totalBlocks	 = _blocks;
    _totalItems		 = 0;
    _totalSubDirs	 = 0;
    _totalFiles		 = 0;
    _totalIgnoredItems	 = 0;
    _totalUnignoredItems = 0;
    _directChildrenCount = 0;
    _errSubDirCount	 = 0;
    _latestMtime	 = _mtime;
    _oldestFileMtime	 = 0;
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

    if ( _attic )
    {
	delete _attic;
	_attic = 0;
    }

    _summaryDirty = true;
    _deletingAll  = false;
    dropSortCache();
}


void DirInfo::reset()
{
    if ( _firstChild || _dotEntry || _attic )
	clear();

    _readState	     = DirQueued;
    _pendingReadJobs = 0;
    _summaryDirty    = true;

    ensureDotEntry();

    if ( _tree )
	_tree->childAddedNotify( _dotEntry );

    recalc();
    dropSortCache();
}


DotEntry * DirInfo::ensureDotEntry()
{
    if ( ! _dotEntry )
    {
	// logDebug() << "Creating dot entry for " << this << endl;

	_dotEntry = new DotEntry( _tree, this );
	CHECK_NEW( _dotEntry );
    }

    return _dotEntry;
}


void DirInfo::deleteEmptyDotEntry()
{
    if ( ! _dotEntry->firstChild() && ! _dotEntry->hasAtticChildren() )
    {
	delete _dotEntry;
	_dotEntry = 0;

	countDirectChildren();
    }
}


Attic * DirInfo::ensureAttic()
{
    if ( ! _attic )
    {
	// logDebug() << "Creating attic for " << this << endl;

	_attic = new Attic( _tree, this );
	CHECK_NEW( _attic );
    }

    return _attic;
}


void DirInfo::deleteEmptyAttic()
{
    if ( _attic && ! _attic->firstChild() )
    {
	delete _attic;
	_attic = 0;
    }
}


bool DirInfo::hasAtticChildren() const
{
    return _attic && _attic->hasChildren();
}


void DirInfo::recalc()
{
    // logDebug() << this << endl;

    _totalSize		 = _size;
    _totalAllocatedSize	 = _allocatedSize;
    _totalBlocks	 = _blocks;
    _totalItems		 = 0;
    _totalSubDirs	 = 0;
    _totalFiles		 = 0;
    _totalIgnoredItems	 = 0;
    _totalUnignoredItems = 0;
    _directChildrenCount = 0;
    _errSubDirCount	 = 0;
    _latestMtime	 = _mtime;
    _oldestFileMtime	 = 0;

    FileInfoIterator it( this );

    while ( *it )
    {
	_directChildrenCount++;
	_totalSize	     += (*it)->totalSize();
	_totalAllocatedSize  += (*it)->totalAllocatedSize();
	_totalBlocks	     += (*it)->totalBlocks();
	_totalItems	     += (*it)->totalItems() + 1;
	_totalSubDirs	     += (*it)->totalSubDirs();
	_errSubDirCount	     += (*it)->errSubDirCount();
	_totalFiles	     += (*it)->totalFiles();
	_totalIgnoredItems   += (*it)->totalIgnoredItems();
	_totalUnignoredItems += (*it)->totalUnignoredItems();

	if ( (*it)->isDir() )
	{
	    _totalSubDirs++;

	    if ( (*it)->readError() )
		_errSubDirCount++;
	}

	if ( (*it)->isFile() )
	    _totalFiles++;

	if ( ! (*it)->isDir() )
	{
	    if ( (*it)->isIgnored() )
		_totalIgnoredItems++;
	    else
		_totalUnignoredItems++;
	}

	time_t childLatestMtime = (*it)->latestMtime();

	if ( childLatestMtime > _latestMtime )
	    _latestMtime = childLatestMtime;

	time_t childOldestFileMTime = (*it)->oldestFileMtime();

	if ( childOldestFileMTime > 0 )
	{
	    if ( _oldestFileMtime == 0 ||
		 childOldestFileMTime < _oldestFileMtime )
	    {
		_oldestFileMtime = childOldestFileMTime;
	    }
	}

	++it;
    }

    if ( _attic )
    {
	_totalIgnoredItems += _attic->totalIgnoredItems();
	_errSubDirCount	   += _attic->errSubDirCount();
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


FileSize DirInfo::totalAllocatedSize()
{
    if ( _summaryDirty )
	recalc();

    return _totalAllocatedSize;
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


int DirInfo::totalNonDirItems()
{
    if ( _summaryDirty )
	recalc();

    return _totalItems - _totalSubDirs;
}


int DirInfo::totalIgnoredItems()
{
    if ( _summaryDirty )
	recalc();

    return _totalIgnoredItems;
}


int DirInfo::totalUnignoredItems()
{
    if ( _summaryDirty )
	recalc();

    return _totalUnignoredItems;
}


time_t DirInfo::latestMtime()
{
    if ( _summaryDirty )
	recalc();

    return _latestMtime;
}


time_t DirInfo::oldestFileMtime()
{
    if ( _summaryDirty )
	recalc();

    return _oldestFileMtime;
}


int DirInfo::totalUsedPercent()
{
    int percent = 100;

    if ( totalAllocatedSize() > 0 && totalSize() > 0 )
    {
        percent = qRound( ( 100.0 * totalSize() ) / totalAllocatedSize() );
    }

    return percent;
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


int DirInfo::errSubDirCount()
{
    if ( _summaryDirty )
	recalc();

    return _errSubDirCount;
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


void DirInfo::insertChild( FileInfo * newChild )
{
    CHECK_PTR( newChild );

    if ( newChild->isDir() || ! _dotEntry )
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


void DirInfo::moveToAttic( FileInfo * child )
{
    unlinkChild( child );
    addToAttic( child );
}


void DirInfo::addToAttic( FileInfo * newChild )
{
    CHECK_PTR( newChild );

    Attic * attic = 0;

    if ( ! newChild->isDir() && _dotEntry )
	attic = _dotEntry->ensureAttic();

    if ( ! attic )
	attic = ensureAttic();

    newChild->setIgnored( true );

    if ( newChild->isDir() )
	_totalIgnoredItems += newChild->totalIgnoredItems();
    else
	_totalIgnoredItems++;

    CHECK_PTR( attic );
    attic->insertChild( newChild );
}


void DirInfo::childAdded( FileInfo * newChild )
{
    bool addToTotal = true;

    if ( newChild->isIgnored() )
    {
	if ( newChild->isDir() )
	    _totalIgnoredItems += newChild->totalIgnoredItems();
	else
	    _totalIgnoredItems++;

	// Add ignored items only to all the totals if this directory is also
	// ignored or if this is the attic.

	if ( ! _isIgnored &&  ! isAttic() )
	    addToTotal = false;
    }
    else
    {
	if ( ! newChild->isDir() )
	    _totalUnignoredItems++;
    }

    if ( addToTotal )
    {
	if ( ! _summaryDirty )
	{
	    _totalSize		+= newChild->size();
	    _totalAllocatedSize += newChild->allocatedSize();
	    _totalBlocks	+= newChild->blocks();
	    _totalItems++;

	    if ( newChild->parent() == this )
		_directChildrenCount++;

	    if ( newChild->isDir() )
		_totalSubDirs++;

	    if ( newChild->isFile() )
		_totalFiles++;

	    if ( newChild->mtime() > _latestMtime )
		_latestMtime = newChild->mtime();

	    time_t childOldestFileMTime = newChild->oldestFileMtime();

	    if ( childOldestFileMTime > 0 && newChild->isFile() )
	    {
		if ( _oldestFileMtime == 0 ||
		     childOldestFileMTime < _oldestFileMtime )
		{
		    _oldestFileMtime = childOldestFileMTime;
		}
	    }
	}
	else
	{
	    // NOP

	    /*
	     * Don't bother updating the summary fields if the summary is dirty
	     * (i.e. outdated) anyway: As soon as anybody wants to know some
	     * exact value a complete recalculation of the entire subtree will
	     * be triggered. On the other hand, if nobody wants to know (which
	     * is very likely) we can save this effort.
	     */
	}
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


void DirInfo::unlinkChild( FileInfo * deletedChild )
{
    if ( deletedChild->parent() != this )
    {
	logError() << deletedChild << " is not a child of " << this
		   << " - cannot unlink from children list!" << endl;
	return;
    }

    dropSortCache();
    _summaryDirty = true;

    if ( deletedChild == _firstChild )
    {
	// logDebug() << "Unlinking first child " << deletedChild << endl;
	_firstChild = deletedChild->next();
	return;
    }

    FileInfo * child = firstChild();

    while ( child )
    {
	if ( child->next() == deletedChild )
	{
	    // logDebug() << "Unlinking " << deletedChild << endl;
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


void DirInfo::readJobFinished( DirInfo * dir )
{
    _pendingReadJobs--;

    if ( _lastSortCol == ReadJobsCol )
	dropSortCache();

    if ( dir && dir != this && dir->readError() )
	_errSubDirCount++;

    if ( _parent )
	_parent->readJobFinished( dir );
}


void DirInfo::readJobAborted( DirInfo * dir )
{
    _readState = DirAborted;

    if ( _parent )
	_parent->readJobAborted( dir );
}


DirReadState DirInfo::readState() const
{
    return _readState;
}


bool DirInfo::readError() const
{
    switch ( _readState )
    {
	case DirError:
	case DirPermissionDenied:
	    return true;

	case DirQueued:
	case DirReading:
	case DirFinished:
	case DirOnRequestOnly:
	case DirCached:
	case DirAborted:
	    return false;

	// No 'default' branch so the compiler can catch unhandled enum values
    }

    return false;
}


QString DirInfo::sizePrefix() const
{
    switch ( _readState )
    {
	case DirQueued:
	case DirReading:
	case DirOnRequestOnly:
	    return "";

	case DirError:
	case DirAborted:
	case DirPermissionDenied:
	    return ">";

	case DirFinished:
	case DirCached:
	    return _errSubDirCount > 0 ? ">" : "";

	// No 'default' branch so the compiler can catch unhandled enum values
    }

    return "";
}


void DirInfo::finalizeLocal()
{
    // logDebug() << this << endl;

    cleanupDotEntries();
    cleanupAttics();
    checkIgnored();
}


void DirInfo::finalizeAll()
{
    FileInfo * child = firstChild();

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

    finalizeLocal();
}


void DirInfo::cleanupDotEntries()
{
    if ( ! _dotEntry )
	return;

    // Reparent dot entry children if there are no subdirectories on this level

    if ( ! _firstChild && ! hasAtticChildren() )
    {
	takeAllChildren( _dotEntry );

	// Reparent the dot entry's attic's children to this item's attic

	if ( _dotEntry->hasAtticChildren() )
	{
	    ensureAttic();
	    _attic->takeAllChildren( _dotEntry->attic() );
	}
    }

    // Delete the dot entry if it is now empty.
    //
    // This also affects dot entries that were just disowned because they had
    // no siblings (i.e., there are no subdirectories on this level).
    //
    // Notice that this also checks if the dot entry's attic (if it has one) is
    // empty, and that its attic is deleted along with the dot entry.

    deleteEmptyDotEntry();
}


void DirInfo::cleanupAttics()
{
    if ( _dotEntry )
	_dotEntry->cleanupAttics();

    if ( _attic )
    {
	_attic->finalizeLocal();

	if ( ! _attic->firstChild() && ! _attic->dotEntry() )
	{
	    delete _attic;
	    _attic = 0;

	    if ( _lastIncludeAttic )
		dropSortCache();

	    _summaryDirty = true;
	}
    }
}


void DirInfo::checkIgnored()
{
    if ( _dotEntry )
	_dotEntry->checkIgnored();

    // Cascade the 'ignored' status up the tree:
    //
    // Display all directories as ignored that have any ignored items, but no
    // items that are not ignored.

    _isIgnored = ( totalIgnoredItems() > 0 && totalUnignoredItems() == 0 );

    if ( _isIgnored )
	ignoreEmptySubDirs();

    if ( ! isPseudoDir() && _parent )
	_parent->checkIgnored();
}


void DirInfo::ignoreEmptySubDirs()
{
    FileInfoIterator it( this );

    while ( *it )
    {
	if ( ! (*it)->isIgnored() && (*it)->isDirInfo() )
	{
	    if ( (*it)->totalUnignoredItems() == 0 )
	    {
		// logDebug() << "Ignoring empty subdir " << (*it) << endl;
		(*it)->setIgnored( true );
		_summaryDirty = true;
	    }
	}

	++it;
    }
}


void DirInfo::clearTouched( bool recursive )
{
    _touched = false;

    if ( recursive )
    {
	if ( ! isDotEntry() )
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

	if ( _attic )
	    _attic->clearTouched();
    }
}


const FileInfoList & DirInfo::sortedChildren( DataColumn    sortCol,
					      Qt::SortOrder sortOrder,
					      bool	    includeAttic )
{
    if ( _sortedChildren &&
	 sortCol      == _lastSortCol	   &&
	 sortOrder    == _lastSortOrder	   &&
	 includeAttic == _lastIncludeAttic    )
    {
	return *_sortedChildren;
    }


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

    if ( includeAttic && _attic )
	_sortedChildren->append( _attic );

    _lastSortCol      = sortCol;
    _lastSortOrder    = sortOrder;
    _lastIncludeAttic = includeAttic;


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

	if ( recursive )
	{
	    if ( ! isDotEntry() )
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

	    if ( _attic )
		_attic->dropSortCache( recursive );
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


void DirInfo::takeAllChildren( DirInfo * oldParent )
{
    FileInfo * child = oldParent->firstChild();

    if ( child )
    {
	// logDebug() << "Reparenting all children of " << oldParent << " to " << this << endl;

	FileInfo * oldFirstChild = _firstChild;
	_firstChild = child;
	FileInfo * lastChild = child;

	oldParent->setFirstChild( 0 );
	oldParent->recalc();

	_directChildrenCount = -1;
	_summaryDirty	     = true;

	while ( child )
	{
	    child->setParent( this );
	    lastChild = child;
	    child = child->next();
	}

	lastChild->setNext( oldFirstChild );
    }
}
