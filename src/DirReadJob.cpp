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
#include "MountPoints.h"
#include "Exception.h"

using namespace QDirStat;


DirReadJob::DirReadJob( DirTree * tree,
			DirInfo * dir  ):
    _tree( tree ),
    _dir( dir ),
    _queue( 0 )
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

void DirReadJob::read()
{
    if ( ! _started )
    {
	_started = true;
	startReading();

	// Don't do anything after startReading() - startReading() might call
	// finished() which in turn makes the queue destroy this object
    }
}


void DirReadJob::setDir( DirInfo * dir )
{
    _dir = dir;
}


void DirReadJob::finished()
{
    if ( _queue )
	_queue->jobFinishedNotify( this );
    else
	logError() << "No job queue for " << _dir << endl;
}


void DirReadJob::childAdded( FileInfo *newChild )
{
    _tree->childAddedNotify( newChild );
}


void DirReadJob::deletingChild( FileInfo *deletedChild )
{
    _tree->deletingChildNotify( deletedChild );
}


bool DirReadJob::crossingFileSystems( DirInfo * parent, DirInfo * child )
{
    if ( parent->device() == child->device() )
        return false;

    QString childDevice  = device( child );
    QString parentDevice = device( parent->findNearestMountPoint() );;

    if ( parentDevice.isEmpty() )
        parentDevice = _tree->device();

    bool crossing = true;

    if ( ! parentDevice.isEmpty() && ! childDevice.isEmpty() )
        crossing = parentDevice != childDevice;

    if ( crossing )
    {
            logInfo() << "File system boundary at mount point " << child
                      << " on device " << ( childDevice.isEmpty() ? "<unknown>" : childDevice )
                      << endl;
    }
    else
    {
        logInfo() << "Mount point " << child
                  << " is still on the same device " << childDevice << endl;
    }

    return crossing;
}


QString DirReadJob::device( const DirInfo * dir ) const
{
    QString device;

    if ( dir )
    {
        const MountPoint * mountPoint = MountPoints::findByPath( dir->url() );

        if ( mountPoint )
            device = mountPoint->device();
    }

    return device;
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


void LocalDirReadJob::startReading()
{
    struct dirent *	entry;
    struct stat		statInfo;
    QString		dirName		 = _dir->url();
    QString		defaultCacheName = DEFAULT_CACHE_NAME;

    // logDebug() << _dir << endl;

    if ( ( _diskDir = opendir( dirName.toUtf8() ) ) )
    {
	_dir->setReadState( DirReading );

	while ( ( entry = readdir( _diskDir ) ) )
	{
	    QString entryName = QString::fromUtf8( entry->d_name );

	    if ( entryName != "."  &&
		 entryName != ".."   )
	    {
		QString fullName = dirName + "/" + entryName;

		if ( lstat( fullName.toUtf8(), &statInfo ) == 0 )	      // lstat() OK
		{
		    if ( S_ISDIR( statInfo.st_mode ) )	// directory child?
		    {
			DirInfo *subDir = new DirInfo( entryName, &statInfo, _tree, _dir );
			CHECK_NEW( subDir );

			_dir->insertChild( subDir );
			childAdded( subDir );

			if ( ExcludeRules::instance()->match( fullName, entryName ) )
			{
			    subDir->setExcluded();
			    subDir->setReadState( DirOnRequestOnly );
			    finishReading( subDir );
			}
			else // No exclude rule matched
			{
			    if ( ! crossingFileSystems(_dir, subDir ) )	// normal case
			    {
				LocalDirReadJob * job = new LocalDirReadJob( _tree, subDir );
				CHECK_NEW( job );
				_tree->addJob( job );
			    }
			    else	// The subdirectory we just found is a mount point.
			    {
				subDir->setMountPoint();

				if ( _tree->crossFileSystems() )
				{
				    LocalDirReadJob * job = new LocalDirReadJob( _tree, subDir );
				    CHECK_NEW( job );
				    _tree->addJob( job );
				}
				else
				{
				    subDir->setReadState( DirOnRequestOnly );
				    finishReading( subDir );
				}
			    }
			}
		    }
		    else		// non-directory child
		    {
			if ( entryName == defaultCacheName )	// .qdirstat.cache.gz found?
			{
			    logDebug() << "Found cache file " << defaultCacheName << endl;

			    //
			    // Read content of this subdirectory from cache file
			    //

			    CacheReadJob * cacheReadJob = new CacheReadJob( _tree, _dir->parent(), fullName );
			    CHECK_NEW( cacheReadJob );
			    QString firstDirInCache = cacheReadJob->reader()->firstDir();

			    if ( firstDirInCache == dirName )	// Does this cache file match this directory?
			    {
				logDebug() << "Using cache file " << fullName << " for " << dirName << endl;

				cacheReadJob->reader()->rewind();  // Read offset was moved by firstDir()
				_tree->addJob( cacheReadJob );	   // Job queue will assume ownership of cacheReadJob

				if ( _dir->parent() )
				    _dir->parent()->setReadState( DirReading );

				//
				// Clean up partially read directory content
				//

				DirTree * tree = _tree;	 // Copy data members to local variables:
				DirInfo * dir  = _dir;	 // This object will be deleted soon by killAll()

				_queue->killAll( _dir, cacheReadJob );	// Will delete this job as well!
				// All data members of this object are invalid from here on!

				logDebug() << "Deleting subtree " << dir << endl;
				tree->deleteSubtree( dir );

				return;
			    }
			    else
			    {
				logWarning() << "NOT using cache file " << fullName
					     << " with dir " << firstDirInCache
					     << " for " << dirName
					     << endl;

				delete cacheReadJob;
			    }
			}
			else
			{
			    FileInfo *child = new FileInfo( entryName, &statInfo, _tree, _dir );
			    CHECK_NEW( child );
			    _dir->insertChild( child );
			    childAdded( child );
			}
		    }
		}
		else			// lstat() error
		{
		    logWarning() << "lstat(" << fullName << ") failed: " << formatErrno() << endl;

		    /*
		     * Not much we can do when lstat() didn't work; let's at
		     * least create an (almost empty) entry as a placeholder.
		     */
		    DirInfo *child = new DirInfo( _tree, _dir, entryName,
						  0,   // mode
						  0,   // size
						  0 ); // mtime
		    CHECK_NEW( child );
		    child->finalizeLocal();
		    child->setReadState( DirError );
		    _dir->insertChild( child );
		    childAdded( child );
		}
	    }
	}

	closedir( _diskDir );
	_dir->setReadState( DirFinished );
	finishReading( _dir );
    }
    else
    {
	_dir->setReadState( DirError );
	logWarning() << "opendir(" << dirName << ") failed" << endl;
	// opendir() doesn't set 'errno' according to POSIX  :-(
	finishReading( _dir );
    }

    finished();
    // Don't add anything after finished() since this deletes this job!
}


void LocalDirReadJob::finishReading( DirInfo * dir )
{
    // logDebug() << dir << endl;

    _tree->sendFinalizeLocal( dir );
    dir->finalizeLocal();
    _tree->sendReadJobFinished( dir );
}



FileInfo * LocalDirReadJob::stat( const QString & url,
				  DirTree	* tree,
				  DirInfo	* parent )
{
    struct stat statInfo;
    logDebug() << "url: \"" << url << "\"" << endl;

    if ( lstat( url.toUtf8(), &statInfo ) == 0 ) // lstat() OK
    {
	if ( S_ISDIR( statInfo.st_mode ) )	// directory?
	{
	    DirInfo * dir = new DirInfo( url, &statInfo, tree, parent );
	    CHECK_NEW( dir );

	    if ( parent )
		parent->insertChild( dir );

	    if ( dir && parent &&
		 ! tree->isTopLevel( dir )
		 && dir->device() != parent->device() )
	    {
		logDebug() << dir << " is a mount point" << endl;
		dir->setMountPoint();
	    }

	    return dir;
	}
	else					// no directory
	{
	    FileInfo * file = new FileInfo( url, &statInfo, tree, parent );
	    CHECK_NEW( file );

	    if ( parent )
		parent->insertChild( file );

	    return file;
	}
    }
    else // lstat() failed
    {
        THROW( SysCallFailedException( "lstat", url ) );
	return 0; // NOTREACHED
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


void CacheReadJob::init()
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


void DirReadJobQueue::enqueue( DirReadJob * job )
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


DirReadJob * DirReadJobQueue::dequeue()
{
    DirReadJob * job = _queue.first();
    _queue.removeFirst();

    if ( job )
	job->setQueue( 0 );

    return job;
}


void DirReadJobQueue::clear()
{
    qDeleteAll( _queue );
    _queue.clear();
}


void DirReadJobQueue::abort()
{
    foreach ( DirReadJob * job, _queue )
    {
	if ( job->dir() )
	    job->dir()->readJobAborted();
    }

    clear();
}


void DirReadJobQueue::killAll( DirInfo * subtree, DirReadJob * exceptJob )
{
    if ( ! subtree )
	return;

    QMutableListIterator<DirReadJob *> it( _queue );

    while ( it.hasNext() )
    {
	DirReadJob * job = it.next();

	if ( exceptJob && job == exceptJob )
	{
	    logDebug() << "NOT killing read job " << job->dir() << endl;
	    continue;
	}

	if ( job->dir() && job->dir()->isInSubtree( subtree ) )
	{
	    // logDebug() << "Killing read job " << job->dir() << endl;
	    it.remove();
	    delete job;
	}
    }
}


void DirReadJobQueue::timeSlicedRead()
{
    if ( ! _queue.isEmpty() )
	_queue.first()->read();
}


void DirReadJobQueue::jobFinishedNotify( DirReadJob *job )
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

