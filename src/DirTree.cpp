/*
 *   File name: DirTree.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QDir>
#include <QFileInfo>

#include "DirTree.h"
#include "DirTreeCache.h"
#include "DirTreeFilter.h"
#include "DotEntry.h"
#include "Attic.h"
#include "FileInfoIterator.h"
#include "FileInfoSet.h"
#include "ExcludeRules.h"
#include "PkgReader.h"
#include "MountPoints.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

#define VERBOSE_EXCLUDE_RULES	1

using namespace QDirStat;


DirTree::DirTree():
    QObject(),
    _excludeRules( 0 ),
    _beingDestroyed( false ),
    _haveClusterSize( false ),
    _blocksPerCluster( 0 )
{
    _isBusy	      = false;
    _crossFilesystems = false;
    _root = new DirInfo( this );
    CHECK_NEW( _root );

    connect( & _jobQueue, SIGNAL( finished()	 ),
	     this,	  SLOT	( slotFinished() ) );

    connect( this,	  SIGNAL( deletingChild	     ( FileInfo * ) ),
	     & _jobQueue, SLOT	( deletingChildNotify( FileInfo * ) ) );
}


DirTree::~DirTree()
{
    _beingDestroyed = true;

    if ( _root )
	delete _root;

    if ( _excludeRules )
	delete _excludeRules;

    clearFilters();
}


void DirTree::setRoot( DirInfo *newRoot )
{
    if ( _root )
    {
	emit deletingChild( _root );
	delete _root;
	emit childDeleted();
    }

    _root = newRoot;

    FileInfo * realRoot = firstToplevel();
    _url = realRoot ? realRoot->url() : "";
}


FileInfo * DirTree::firstToplevel() const
{
    return _root ? _root->firstChild() : 0;
}


bool DirTree::isTopLevel( FileInfo *item ) const
{
    return item && item->parent() && ! item->parent()->parent();
}


QString DirTree::url() const
{
    return _url;
}


void DirTree::clear()
{
    _jobQueue.clear();

    if ( _root )
    {
	emit clearing();
	_root->clear();
    }

    _isBusy	      = false;
    _haveClusterSize  = false;
    _blocksPerCluster = 0;
    _device.clear();
}


void DirTree::reset()
{
    clear();
    clearExcludeRules();
    clearFilters();
}


void DirTree::startReading( const QString & rawUrl )
{
    QFileInfo fileInfo( rawUrl );
    _url = fileInfo.absoluteFilePath();
    // logDebug() << "rawUrl: \"" << rawUrl << "\"" << endl;
    logInfo() << "   url: \"" << _url	 << "\"" << endl;
    MountPoint * mountPoint = MountPoints::findNearestMountPoint( _url );
    _device = mountPoint ? mountPoint->device() : "";
    logInfo() << "device: " << _device << endl;

    if ( _root->hasChildren() )
	clear();

    _isBusy = true;
    emit startingReading();

    FileInfo * item = LocalDirReadJob::stat( _url, this, _root );
    CHECK_PTR( item );

    if ( item )
    {
	childAddedNotify( item );

	if ( item->isDirInfo() )
	{
	    addJob( new LocalDirReadJob( this, item->toDirInfo() ) );
	    emit readJobFinished( _root );
	}
	else
	{
	    finalizeTree();
	    _isBusy = false;
	    emit readJobFinished( _root );
	    emit finished();
	}
    }
    else	// stat() failed
    {
	logWarning() << "stat(" << _url << ") failed" << endl;
	_isBusy = false;
	emit finished();
    }
}


void DirTree::refresh( const FileInfoSet & refreshSet )
{
    FileInfoSet items = refreshSet.invalidRemoved().normalized();

    foreach ( FileInfo * item, items )
    {
	// Need to check the magic number here again because a previous
	// iteration step might have made the item invalid already

	if ( item && item->checkMagicNumber() )
	{
	    if ( item->isDirInfo() )
		refresh( item->toDirInfo() );
	    else if ( item->parent() )
		refresh( item->parent() );
	}
    }
}


void DirTree::refresh( DirInfo * subtree )
{
    if ( ! _root )
	return;

    if ( ! subtree->checkMagicNumber() )
    {
	// Not using CHECK_MAGIC() here which would throw an exception since
	// this might easily happen after cleanup actions with multi selection
	// if one selected item is in the subtree of another, and that parent
	// was already refreshed.

	logWarning() << "Item is no longer valid - not refreshing subtree" << endl;
	return;
    }

    if ( subtree->isDotEntry() )
	subtree = subtree->parent();

    if ( ! subtree || ! subtree->parent() )	// Refresh all (from first toplevel)
    {
	try
	{
	    startReading( QDir::cleanPath( firstToplevel()->url() ) );
	}
	catch ( const SysCallFailedException & ex )
	{
	    CAUGHT( ex );
	}
    }
    else	// Refresh subtree
    {
	// logDebug() << "Refreshing subtree " << subtree << endl;

	clearSubtree( subtree );

	subtree->reset();
	subtree->setExcluded( false );

	_isBusy = true;
	subtree->setReadState( DirReading );
	emit startingReading();
	addJob( new LocalDirReadJob( this, subtree ) );
    }
}


void DirTree::abortReading()
{
    if ( _jobQueue.isEmpty() )
	return;

    _jobQueue.abort();

    _isBusy = false;
    emit aborted();
}


void DirTree::finalizeTree()
{
    if ( _root && hasFilters() )
    {
	recalc( _root );
	ignoreEmptyDirs( _root );
	recalc( _root );
	moveIgnoredToAttic( _root );
	recalc( _root );
    }
}


void DirTree::slotFinished()
{
    finalizeTree();
    _isBusy = false;
    emit finished();
}


void DirTree::childAddedNotify( FileInfo * newChild )
{
    if ( ! _haveClusterSize )
        detectClusterSize( newChild );

    emit childAdded( newChild );

    if ( newChild->dotEntry() )
	emit childAdded( newChild->dotEntry() );
}


void DirTree::deletingChildNotify( FileInfo * deletedChild )
{
    logDebug() << "Deleting child " << deletedChild << endl;
    emit deletingChild( deletedChild );

    if ( deletedChild == _root )
	_root = 0;
}


void DirTree::childDeletedNotify()
{
    emit childDeleted();
}


void DirTree::deleteSubtree( FileInfo *subtree )
{
    // logDebug() << "Deleting subtree " << subtree << endl;
    DirInfo * parent = subtree->parent();

    // Send notification to anybody interested (e.g., to attached views)
    deletingChildNotify( subtree );

    if ( parent )
    {
	if ( parent->isDotEntry() && ! parent->hasChildren() )
	    // This was the last child of a dot entry
	{
	    // Get rid of that now empty and useless dot entry

	    if ( parent->parent() )
	    {
		if ( parent->parent()->isFinished() )
		{
		    // logDebug() << "Removing empty dot entry " << parent << endl;

		    deletingChildNotify( parent );
		    parent->parent()->deleteEmptyDotEntry();

		    delete parent;
		    parent = 0;
		}
	    }
	    else	// no parent - this should never happen (?)
	    {
		logError() << "Internal error: Killing dot entry without parent " << parent << endl;

		// Better leave that dot entry alone - we shouldn't have come
		// here in the first place. Who knows what will happen if this
		// thing is deleted now?!
		//
		// Intentionally NOT calling:
		//     delete parent;
	    }
	}
    }

    if ( parent )
    {
	// Give the parent of the child to be deleted a chance to unlink the
	// child from its children list and take care of internal summary
	// fields
	parent->deletingChild( subtree );
    }

    delete subtree;

    if ( subtree == _root )
    {
	_root = 0;
    }

    emit childDeleted();
}


void DirTree::clearSubtree( DirInfo * subtree )
{
    if ( subtree->hasChildren() )
    {
	emit clearingSubtree( subtree );
	subtree->clear();
	emit subtreeCleared( subtree );
    }
}


void DirTree::addJob( DirReadJob * job )
{
    _jobQueue.enqueue( job );
}


void DirTree::addBlockedJob( DirReadJob * job )
{
    _jobQueue.addBlocked( job );
}


void DirTree::unblock( DirReadJob * job )
{
    _jobQueue.unblock( job );
}


void DirTree::sendStartingReading()
{
    emit startingReading();
}


void DirTree::sendFinished()
{
    finalizeTree();
    _isBusy = false;
    emit finished();
}


void DirTree::sendAborted()
{
    _isBusy = false;
    emit aborted();
}


void DirTree::sendStartingReading( DirInfo * dir )
{
    emit startingReading( dir );
}


void DirTree::sendReadJobFinished( DirInfo * dir )
{
    // logDebug() << dir << endl;
    emit readJobFinished( dir );
}


FileInfo * DirTree::locate( QString url, bool findPseudoDirs )
{
    if ( ! _root )
	return 0;

    FileInfo * topItem = _root->firstChild();

    if ( topItem	      &&
	 topItem->isPkgInfo() &&
	 topItem->url() == url	)
    {
	return topItem;
    }

    return _root->locate( url, findPseudoDirs );
}


bool DirTree::writeCache( const QString & cacheFileName )
{
    CacheWriter writer( cacheFileName.toUtf8(), this );
    return writer.ok();
}


void DirTree::readCache( const QString & cacheFileName )
{
    _isBusy = true;
    emit startingReading();
    addJob( new CacheReadJob( this, 0, cacheFileName ) );
}


void DirTree::clearAndReadCache( const QString & cacheFileName )
{
    clear();
    readCache( cacheFileName );
}


void DirTree::readPkg( const PkgFilter & pkgFilter )
{
    clear();
    _isBusy = true;
    _url    = pkgFilter.url();
    emit startingReading();

    // logDebug() << "Reading " << pkgFilter << endl;
    PkgReader reader( this );
    reader.read( pkgFilter );
}


void DirTree::setExcludeRules( ExcludeRules * newRules )
{
    if ( _excludeRules )
	delete _excludeRules;

#if VERBOSE_EXCLUDE_RULES
    if ( newRules )
    {
	logDebug() << "New tmp exclude rules:" << endl;

	for ( ExcludeRuleListIterator it = newRules->begin(); it != newRules->end(); ++it )
	{
	    logDebug() << *it << endl;
	}
    }
    else
    {
	logDebug() << "Clearing tmp exclude rules" << endl;
    }
#endif

    _excludeRules = newRules;
}


void DirTree::addFilter( DirTreeFilter * filter )
{
    if ( filter )
	_filters << filter;
}


void DirTree::clearFilters()
{
    qDeleteAll( _filters );
    _filters.clear();
}


bool DirTree::checkIgnoreFilters( const QString & path )
{
    foreach ( DirTreeFilter * filter, _filters )
    {
	if ( filter->ignore( path ) )
	    return true;
    }

    return false;
}


void DirTree::moveIgnoredToAttic( DirInfo * dir )
{
    CHECK_PTR( dir );

    if ( dir->totalIgnoredItems() == 0 && dir->totalUnignoredItems() > 0 )
	return;

    // Not using FileInfoIterator because we don't want to iterate over the dot
    // entry as well, just the normal children.

    FileInfo * child = dir->firstChild();
    FileInfoList ignoredChildren;

    while ( child )
    {
	if ( child->isIgnored() )
	{
	    // Don't move the child right here, otherwise the iteration breaks
	    ignoredChildren << child;
	}
	else
	{
	    if ( child->isDirInfo() )
		moveIgnoredToAttic( child->toDirInfo() );
	}

	child = child->next();
    }


    foreach ( FileInfo * child, ignoredChildren )
    {
	// logDebug() << "Moving ignored " << child << " to attic" << endl;
	dir->moveToAttic( child );

	if ( child->isDirInfo() )
	    unatticAll( child->toDirInfo() );
    }

    if ( ! ignoredChildren.isEmpty() )
    {
	dir->recalc();

	if ( dir->attic() )
	    dir->attic()->recalc();
    }
}


void DirTree::ignoreEmptyDirs( DirInfo * dir )
{
    CHECK_PTR( dir );

    FileInfo * child = dir->firstChild();
    FileInfoList ignoredChildren;

    while ( child )
    {
	if ( ! child->isIgnored() && child->isDirInfo() )
	{
	    DirInfo * subDir = child->toDirInfo();

	    if ( subDir->totalUnignoredItems() == 0 )
		// && ! subDir->isMountPoint()
	    {
		// logDebug() << "Ignoring empty subdir " << subDir << endl;
		subDir->setIgnored( true );
	    }
	    else
	    {
		ignoreEmptyDirs( subDir );
	    }
	}

	child = child->next();
    }
}


void DirTree::unatticAll( DirInfo * dir )
{
    CHECK_PTR( dir );

    if ( dir->attic() )
    {
	// logDebug() << "Moving all attic children to the normal children list for " << dir << endl;
	dir->takeAllChildren( dir->attic() );
	dir->deleteEmptyAttic();
	dir->recalc();
    }

    FileInfoIterator it( dir );

    while ( *it )
    {
	if ( (*it)->isDirInfo() )
	    unatticAll( (*it)->toDirInfo() );

	++it;
    }
}


void DirTree::recalc( DirInfo * dir )
{
    CHECK_PTR( dir );

    FileInfo * child = dir->firstChild();

    while ( child )
    {
	if ( child->isDirInfo() )
	    recalc( child->toDirInfo() );

	child = child->next();
    }

    if ( dir->dotEntry() )
	recalc( dir->dotEntry() );

    if ( dir->attic() )
	recalc( dir->attic() );

    dir->recalc();
}


void DirTree::detectClusterSize( FileInfo * item )
{
    if ( item &&
         item->isFile()     &&
         item->blocks() > 1 &&          // 1..512 bytes fits into an NTFS fragment
         item->size()   < 2 * STD_BLOCK_SIZE )
    {
        _blocksPerCluster = item->blocks();
        _haveClusterSize  = true;

        logInfo() << "Cluster size: " << _blocksPerCluster << " blocks ("
                  << formatSize( clusterSize() ) << ")" << endl;
        logDebug() << "Derived from " << item << " " << formatSize( item->rawByteSize() )
                   << " (allocated: " << formatSize( item->rawAllocatedSize() ) << ")"
                   << endl;
    }
}
