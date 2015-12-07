/*
 *   File name: DirReadJob.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/errno.h>

#include <QMutableListIterator>

#include "DirTree.h"
#include "DirReadJob.h"
#include "DirTreeCache.h"
#include "ExcludeRules.h"
#include "Exception.h"


using namespace QDirStat;


DirReadJob::DirReadJob( DirTree * tree,
			DirInfo * dir  )
    : _tree( tree )
    , _dir( dir )
{
    _started	= false;

    if ( _dir )
	_dir->readJobAdded();
}


DirReadJob::~DirReadJob()
{
    if ( _dir )
	_dir->readJobFinished();
}


/**
 * Default implementation - derived classes should overwrite this method or
 * startReading() (or both).
 **/

void
DirReadJob::read()
{
    if ( ! _started )
    {
	_started = true;
	startReading();

	// Don't do anything after startReading() - startReading() might call
	// finished() which in turn makes the queue destroy this object
    }
}


void
DirReadJob::setDir( DirInfo * dir )
{
    _dir = dir;
}


void
DirReadJob::finished()
{
    if ( _queue )
	_queue->jobFinishedNotify( this );
    else
	logError() << "No job queue for " << _dir << endl;
}


void
DirReadJob::childAdded( FileInfo *newChild )
{
    _tree->childAddedNotify( newChild );
}


void
DirReadJob::deletingChild( FileInfo *deletedChild )
{
    _tree->deletingChildNotify( deletedChild );
}





LocalDirReadJob::LocalDirReadJob( DirTree * tree,
				  DirInfo * dir )
    : DirReadJob( tree, dir )
    , _diskDir( 0 )
{
}


LocalDirReadJob::~LocalDirReadJob()
{
}


void
LocalDirReadJob::startReading()
{
    struct dirent *	entry;
    struct stat		statInfo;
    QString		dirName		 = _dir->url();
    QString		defaultCacheName = DEFAULT_CACHE_NAME;

    logDebug() << _dir << endl;

    if ( ( _diskDir = opendir( dirName.toUtf8() ) ) )
    {
	_tree->sendProgressInfo( dirName );
	_dir->setReadState( DirReading );

	while ( ( entry = readdir( _diskDir ) ) )
	{
	    QString entryName = entry->d_name;

	    if ( entryName != "."  &&
		 entryName != ".."   )
	    {
		QString fullName = dirName + "/" + entryName;

		if ( lstat( fullName.toUtf8(), &statInfo ) == 0 )	      // lstat() OK
		{
		    if ( S_ISDIR( statInfo.st_mode ) )	// directory child?
		    {
			DirInfo *subDir = new DirInfo( entryName, &statInfo, _tree, _dir );
			_dir->insertChild( subDir );
			childAdded( subDir );

			if ( ExcludeRules::excludeRules()->match( fullName ) )
			{
			    subDir->setExcluded();
			    subDir->setReadState( DirOnRequestOnly );
			    _tree->sendFinalizeLocal( subDir );
			    subDir->finalizeLocal();
			}
			else // No exclude rule matched
			{
			    if ( _dir->device() == subDir->device()	)	// normal case
			    {
				_tree->addJob( new LocalDirReadJob( _tree, subDir ) );
			    }
			    else	// The subdirectory we just found is a mount point.
			    {
				logDebug() << "Found mount point " << subDir << endl;
				subDir->setMountPoint();

				if ( _tree->crossFileSystems() )
				{
				    _tree->addJob( new LocalDirReadJob( _tree, subDir ) );
				}
				else
				{
				    subDir->setReadState( DirOnRequestOnly );
				    _tree->sendFinalizeLocal( subDir );
				    subDir->finalizeLocal();
				}
			    }
			}
		    }
		    else		// non-directory child
		    {
			if ( entryName == defaultCacheName )	// .qdirstat.cache.gz found?
			{
			    logDebug() << "Found " << defaultCacheName << endl;

			    //
			    // Read content of this subdirectory from cache file
			    //


			    CacheReadJob * cacheReadJob = new CacheReadJob( _tree, _dir->parent(), fullName );
			    CHECK_NEW( cacheReadJob );
			    QString firstDirInCache = cacheReadJob->reader()->firstDir();

			    if ( firstDirInCache == dirName )	// Does this cache file match this directory?
			    {
				logDebug() << "Using cache file " << fullName << " for " << dirName << endl;

				cacheReadJob->reader()->rewind();	// Read offset was moved by firstDir()
				_tree->addJob( cacheReadJob );	// Job queue will assume ownership of cacheReadJob

				//
				// Clean up partially read directory content
				//

				DirTree * tree = _tree; // Copy data members to local variables:
				DirInfo * dir  = _dir;		// This object will be deleted soon by killAll()

				_queue->killAll( dir );		// Will delete this job as well!
				// All data members of this object are invalid from here on!

				tree->deleteSubtree( dir );

				return;
			    }
			    else
			    {
				logDebug() << "NOT using cache file " << fullName
					   << " with dir " << firstDirInCache
					   << " for " << dirName
					   << endl;

				delete cacheReadJob;
			    }
			}
			else
			{
			    FileInfo *child = new FileInfo( entryName, &statInfo, _tree, _dir );
			    _dir->insertChild( child );
			    childAdded( child );
			}
		    }
		}
		else			// lstat() error
		{
		    logWarning() << "lstat(" << fullName << ") failed: " << strerror( errno ) << endl;

		    /*
		     * Not much we can do when lstat() didn't work; let's at
		     * least create an (almost empty) entry as a placeholder.
		     */
		    DirInfo *child = new DirInfo( _tree, _dir, entry->d_name );
		    child->setReadState( DirError );
		    _dir->insertChild( child );
		    childAdded( child );
		}
	    }
	}

	closedir( _diskDir );
	logDebug() << "Finished reading " << _dir << endl;
	_dir->setReadState( DirFinished );
	_tree->sendFinalizeLocal( _dir );
	_dir->finalizeLocal();
    }
    else
    {
	_dir->setReadState( DirError );
	_tree->sendFinalizeLocal( _dir );
	_dir->finalizeLocal();
	logWarning() << "opendir(" << dirName << ") failed" << endl;
	// opendir() doesn't set 'errno' according to POSIX  :-(
    }

    finished();
    // Don't add anything after finished() since this deletes this job!
}



FileInfo *
LocalDirReadJob::stat( const QString & url,
		       DirTree       * tree,
		       DirInfo       * parent )
{
    struct stat statInfo;
    logDebug() << "url: \"" << url << "\"" << endl;

    if ( lstat( url.toUtf8(), &statInfo ) == 0 ) // lstat() OK
    {
	QString name = parent ? baseName( url ) : url;

	if ( S_ISDIR( statInfo.st_mode ) )	// directory?
	{
	    DirInfo * dir = new DirInfo( name, &statInfo, tree, parent );
	    CHECK_NEW( dir );

	    if ( dir && parent && dir->device() != parent->device() )
		dir->setMountPoint();

	    return dir;
	}
	else					// no directory
	    return new FileInfo( name, &statInfo, tree, parent );
    }
    else // lstat() failed
    {
	logError() << "lstat() failed for \"" << url << "\"" << endl;
	return 0;
    }
}






CacheReadJob::CacheReadJob( DirTree	* tree,
			    DirInfo	* parent,
			    CacheReader * reader )
    : ObjDirReadJob( tree, parent )
    , _reader( reader )
{
    if ( _reader )
	_reader->rewind();

    init();
}


CacheReadJob::CacheReadJob( DirTree	  * tree,
			    DirInfo	  * parent,
			    const QString & cacheFileName )
    : ObjDirReadJob( tree, parent )
{
    _reader = new CacheReader( cacheFileName, tree, parent );
    CHECK_NEW( _reader );

    init();
}


void
CacheReadJob::init()
{
    if ( _reader )
    {
	if ( _reader->ok() )
	{
	    connect( _reader,	SIGNAL( childAdded    ( FileInfo * ) ),
		     this,	SLOT  ( slotChildAdded( FileInfo * ) ) );
	}
	else
	{
	    delete _reader;
	    _reader = 0;
	}
    }
}


CacheReadJob::~CacheReadJob()
{
    if ( _reader )
	delete _reader;
}


void
CacheReadJob::read()
{
    /*
     * This will be called repeatedly from DirTree::timeSlicedRead() until
     * finished() is called.
     */

    if ( ! _reader )
	finished();

    // logDebug() << "Reading 1000 cache lines" << endl;
    _reader->read( 1000 );
    _tree->sendProgressInfo( "" );

    if ( _reader->eof() || ! _reader->ok() )
    {
	// logDebug() << "Cache reading finished - ok: " << _reader->ok() << endl;
	finished();
    }
}





DirReadJobQueue::DirReadJobQueue()
    : QObject()
{
    connect( &_timer, SIGNAL( timeout() ),
	     this,    SLOT  ( timeSlicedRead() ) );
}


DirReadJobQueue::~DirReadJobQueue()
{
    clear();
}


void
DirReadJobQueue::enqueue( DirReadJob * job )
{
    if ( job )
    {
	_queue.append( job );
	job->setQueue( this );

	if ( ! _timer.isActive() )
	{
	    // logDebug() << "First job queued" << endl;
	    emit startingReading();
	    _timer.start( 0 );
	}
    }
}


DirReadJob *
DirReadJobQueue::dequeue()
{
    DirReadJob * job = _queue.first();
    _queue.removeFirst();

    if ( job )
	job->setQueue( 0 );

    return job;
}


void
DirReadJobQueue::clear()
{
    qDeleteAll( _queue );
    _queue.clear();
}


void
DirReadJobQueue::abort()
{
    foreach ( DirReadJob * job, _queue )
    {
	if ( job->dir() )
	    job->dir()->readJobAborted();
    }

    clear();
}


void
DirReadJobQueue::killAll( DirInfo * subtree )
{
    if ( ! subtree )
	return;

    QMutableListIterator<DirReadJob *> it( _queue );

    while ( it.hasNext() )
    {
	DirReadJob * job = it.next();
	if ( job->dir() && job->dir()->isInSubtree( subtree ) )
	{
	    // logDebug() << "Killing read job " << job->dir() << endl;

	    it.remove();
	    delete job;
	}
    }
}


void
DirReadJobQueue::timeSlicedRead()
{
    if ( ! _queue.isEmpty() )
	_queue.first()->read();
}


void
DirReadJobQueue::jobFinishedNotify( DirReadJob *job )
{
    // Get rid of the old (finished) job.

    _queue.removeOne( job );
    delete job;

    // The timer will start a new job when it fires.

    if ( _queue.isEmpty() )	// No new job available - we're done.
    {
	_timer.stop();
	// logDebug() << "No more jobs - finishing" << endl;
	emit finished();
    }
}

