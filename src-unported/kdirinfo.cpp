/*
 *   File name:	kdirinfo.cpp
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <kapp.h>
#include <klocale.h>
#include "kdirinfo.h"
#include "kdirtreeiterators.h"

using namespace KDirStat;



KDirInfo::KDirInfo( KDirTree *	tree,
		    KDirInfo *	parent,
		    bool	asDotEntry )
    : KFileInfo( tree, parent )
{
    init();

    if ( asDotEntry )
    {
	_isDotEntry	= true;
	_dotEntry	= 0;
	_name		= ".";
    }
    else
    {
	_isDotEntry	= false;
	_dotEntry	= new KDirInfo( tree, this, true );
    }
}


KDirInfo::KDirInfo( const QString &	filenameWithoutPath,
		    struct stat	*	statInfo,
		    KDirTree    *	tree,
		    KDirInfo	*	parent )
    : KFileInfo( filenameWithoutPath,
		 statInfo,
		 tree,
		 parent )
{
    init();
    _dotEntry	= new KDirInfo( tree, this, true );
}


KDirInfo::KDirInfo( const KFileItem	* fileItem,
		    KDirTree 		* tree,
		    KDirInfo		* parent )
    : KFileInfo( fileItem,
		 tree,
		 parent )
{
    init();
    _dotEntry	= new KDirInfo( tree, this, true );
}


KDirInfo::KDirInfo( KDirTree * 		tree,
		    KDirInfo * 		parent,
		    const QString &	filenameWithoutPath,
		    mode_t	 	mode,
		    KFileSize	 	size,
		    time_t	 	mtime )
    : KFileInfo( tree,
		 parent,
		 filenameWithoutPath,
		 mode,
		 size,
		 mtime )
{
    init();
    _dotEntry	= new KDirInfo( tree, this, true );
}


void
KDirInfo::init()
{
    _isDotEntry		= false;
    _pendingReadJobs	= 0;
    _dotEntry		= 0;
    _firstChild		= 0;
    _totalSize		= _size;
    _totalBlocks	= _blocks;
    _totalItems		= 0;
    _totalSubDirs	= 0;
    _totalFiles		= 0;
    _latestMtime	= _mtime;
    _isMountPoint	= false;
    _isExcluded		= false;
    _summaryDirty	= false;
    _beingDestroyed	= false;
    _readState		= KDirQueued;
}


KDirInfo::~KDirInfo()
{
    _beingDestroyed	= true;
    KFileInfo	*child	= _firstChild;


    // Recursively delete all children.

    while ( child )
    {
	KFileInfo * nextChild = child->next();
	delete child;
	child = nextChild;
    }


    // Delete the dot entry.

    if ( _dotEntry )
    {
	delete _dotEntry;
    }
}


void
KDirInfo::recalc()
{
    // kdDebug() << k_funcinfo << this << endl;

    _totalSize		= _size;
    _totalBlocks	= _blocks;
    _totalItems		= 0;
    _totalSubDirs	= 0;
    _totalFiles		= 0;
    _latestMtime	= _mtime;

    KFileInfoIterator it( this, KDotEntryAsSubDir );

    while ( *it )
    {
	_totalSize	+= (*it)->totalSize();
	_totalBlocks	+= (*it)->totalBlocks();
	_totalItems	+= (*it)->totalItems() + 1;
	_totalSubDirs	+= (*it)->totalSubDirs();
	_totalFiles	+= (*it)->totalFiles();

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


void
KDirInfo::setMountPoint( bool isMountPoint )
{
    _isMountPoint = isMountPoint;
}


KFileSize
KDirInfo::totalSize()
{
    if ( _summaryDirty )
	recalc();

    return _totalSize;
}


KFileSize
KDirInfo::totalBlocks()
{
    if ( _summaryDirty )
	recalc();

    return _totalBlocks;
}


int
KDirInfo::totalItems()
{
    if ( _summaryDirty )
	recalc();

    return _totalItems;
}


int
KDirInfo::totalSubDirs()
{
    if ( _summaryDirty )
	recalc();

    return _totalSubDirs;
}


int
KDirInfo::totalFiles()
{
    if ( _summaryDirty )
	recalc();

    return _totalFiles;
}


time_t
KDirInfo::latestMtime()
{
    if ( _summaryDirty )
	recalc();

    return _latestMtime;
}


bool
KDirInfo::isFinished()
{
    return ! isBusy();
}


void KDirInfo::setReadState( KDirReadState newReadState )
{
    // "aborted" has higher priority than "finished"

    if ( _readState == KDirAborted && newReadState == KDirFinished )
	return;

    _readState = newReadState;
}


bool
KDirInfo::isBusy()
{
    if ( _pendingReadJobs > 0 && _readState != KDirAborted )
	return true;

    if ( readState() == KDirReading ||
	 readState() == KDirQueued    )
	return true;

    return false;
}


void
KDirInfo::insertChild( KFileInfo *newChild )
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


void
KDirInfo::childAdded( KFileInfo *newChild )
{
    if ( ! _summaryDirty )
    {
	_totalSize	+= newChild->size();
	_totalBlocks	+= newChild->blocks();
	_totalItems++;

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

    if ( _parent )
	_parent->childAdded( newChild );
}


void
KDirInfo::deletingChild( KFileInfo *deletedChild )
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
     * will be triggered from outside - which might as well never happen when
     * nobody wants to know some summary field anyway.
     **/

    _summaryDirty = true;

    if ( _parent )
	_parent->deletingChild( deletedChild );

    if ( ! _beingDestroyed && deletedChild->parent() == this )
    {
	/**
	 * Unlink the child from the children's list - but only if this doesn't
	 * happen recursively in the destructor of this object: No use
	 * bothering about the validity of the children's list if this will all
	 * be history anyway in a moment.
	 **/

	unlinkChild( deletedChild );
    }
}


void
KDirInfo::unlinkChild( KFileInfo *deletedChild )
{
    if ( deletedChild->parent() != this )
    {
	kdError() << deletedChild << " is not a child of " << this
		  << " - cannot unlink from children list!" << endl;
	return;
    }

    if ( deletedChild == _firstChild )
    {
	// kdDebug() << "Unlinking first child " << deletedChild << endl;
	_firstChild = deletedChild->next();
	return;
    }

    KFileInfo *child = firstChild();

    while ( child )
    {
	if ( child->next() == deletedChild )
	{
	    // kdDebug() << "Unlinking " << deletedChild << endl;
	    child->setNext( deletedChild->next() );

	    return;
	}

	child = child->next();
    }

    kdError() << "Couldn't unlink " << deletedChild << " from "
	      << this << " children list" << endl;
}


void
KDirInfo::readJobAdded()
{
    _pendingReadJobs++;

    if ( _parent )
	_parent->readJobAdded();
}


void
KDirInfo::readJobFinished()
{
    _pendingReadJobs--;

    if ( _parent )
	_parent->readJobFinished();
}


void
KDirInfo::readJobAborted()
{
    _readState = KDirAborted;

    if ( _parent )
	_parent->readJobAborted();
}


void
KDirInfo::finalizeLocal()
{
    cleanupDotEntries();
}


void
KDirInfo::finalizeAll()
{
    if ( _isDotEntry )
	return;

    KFileInfo *child = firstChild();

    while ( child )
    {
	KDirInfo * dir = dynamic_cast<KDirInfo *> (child);

	if ( dir && ! dir->isDotEntry() )
	    dir->finalizeAll();

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

    _tree->sendFinalizeLocal( this ); // Must be sent _before_ finalizeLocal()!
    finalizeLocal();
}


KDirReadState
KDirInfo::readState() const
{
    if ( _isDotEntry && _parent )
	return _parent->readState();
    else
	return _readState;
}


void
KDirInfo::cleanupDotEntries()
{
    if ( ! _dotEntry || _isDotEntry )
	return;

    // Reparent dot entry children if there are no subdirectories on this level

    if ( ! _firstChild )
    {
	// kdDebug() << "Reparenting children of solo dot entry " << this << endl;

	KFileInfo *child = _dotEntry->firstChild();
	_firstChild = child;		// Move the entire children chain here.
	_dotEntry->setFirstChild( 0 );	// _dotEntry will be deleted below.

	while ( child )
	{
	    child->setParent( this );
	    child = child->next();
	}
    }


    // Delete dot entries without any children

    if ( ! _dotEntry->firstChild() )
    {
	// kdDebug() << "Removing empty dot entry " << this << endl;

	delete _dotEntry;
	_dotEntry = 0;
    }
}



// EOF
