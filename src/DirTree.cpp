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
#include "FileInfoSet.h"
#include "ExcludeRules.h"
#include "PkgReader.h"
#include "MountPoints.h"
#include "Exception.h"

#define VERBOSE_EXCLUDE_RULES	1

using namespace QDirStat;


DirTree::DirTree():
    QObject(),
    _excludeRules( 0 )
{
    _isBusy	      = false;
    _crossFileSystems = false;
    _root = new DirInfo( this );
    CHECK_NEW( _root );

    connect( & _jobQueue, SIGNAL( finished()	 ),
	     this,	  SLOT	( slotFinished() ) );

    connect( this,	  SIGNAL( deletingChild	     ( FileInfo * ) ),
	     & _jobQueue, SLOT	( deletingChildNotify( FileInfo * ) ) );
}


DirTree::~DirTree()
{
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

    _isBusy = false;
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
    const MountPoint * mountPoint = MountPoints::findNearestMountPoint( _url );
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
	emit finalizeLocal( 0 );
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
	    if( item->isDirInfo() )
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


void DirTree::slotFinished()
{
    _isBusy = false;
    emit finished();
}


void DirTree::childAddedNotify( FileInfo * newChild )
{
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


void DirTree::sendFinalizeLocal( DirInfo *dir )
{
    emit finalizeLocal( dir );
}


void DirTree::sendStartingReading()
{
    emit startingReading();
}


void DirTree::sendFinished()
{
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


FileInfo * DirTree::locate( QString url, bool findDotEntries )
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

    return _root->locate( url, findDotEntries );
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
    _filters << filter;
}


void DirTree::clearFilters()
{
    qDeleteAll( _filters );
    _filters.clear();
}


bool DirTree::ignore( const QString & path )
{
    foreach ( DirTreeFilter * filter, _filters )
    {
	if ( filter->ignore( path ) )
	    return true;
    }

    return false;
}



// EOF
