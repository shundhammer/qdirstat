/*
 *   File name: DirReadJob.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>	// AT_ constants (fstatat() flags)
#include <unistd.h>
#include <stdio.h>

#include <QMutableListIterator>
#include <QMultiMap>

#include "DirReadJob.h"
#include "DirTree.h"
#include "DirTreeCache.h"
#include "Attic.h"
#include "ExcludeRules.h"
#include "MountPoints.h"
#include "Exception.h"

#define DONT_TRUST_NTFS_HARD_LINKS      1
#define VERBOSE_NTFS_HARD_LINKS         0

using namespace QDirStat;


DirReadJob::DirReadJob( DirTree * tree,
			DirInfo * dir  ):
    _tree( tree ),
    _dir( dir ),
    _queue( 0 )
{
    _started = false;

    if ( _dir )
	_dir->readJobAdded();
}


DirReadJob::~DirReadJob()
{
    if ( ! _tree->beingDestroyed() )
    {
	// Only do this if the tree is not in the process of being destroyed;
	// otherwise all FileInfo / DirInfo pointers pointing into that tree
	// may already be invalid. And even if they are not, it is now
	// pointless to do all the housekeeping stuff to finalize the read job:
	// We'd be beautifying the tree content that is now being destroyed.
	//
	// https://github.com/shundhammer/qdirstat/issues/122

	if ( _dir )
	    _dir->readJobFinished( _dir );
    }
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


bool DirReadJob::crossingFilesystems( DirInfo * parent, DirInfo * child )
{
    if ( parent->device() == child->device() )
	return false;

    QString childDevice	 = device( child );
    QString parentDevice = device( parent->findNearestMountPoint() );

    if ( childDevice.isEmpty() && parent->readState() == DirCached )
    {
	// A DirInfo from a cache file always has device number 0, so the
	// initial check failed because of that. The child might still be a
	// mount point, but since that path was not found in the list of known
	// mount points from /proc/mounts or /etc/mtab, we assume that this was
	// not the case.
	//
	// See also https://github.com/shundhammer/qdirstat/issues/114

	return false;
    }

    if ( parentDevice.isEmpty() )
	parentDevice = _tree->device();

    bool crossing = true;

    if ( ! parentDevice.isEmpty() && ! childDevice.isEmpty() )
	crossing = parentDevice != childDevice;

    if ( crossing )
    {
	logInfo() << "Filesystem boundary at mount point " << child
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
	MountPoint * mountPoint = MountPoints::findByPath( dir->url() );

	if ( mountPoint )
	    device = mountPoint->device();
    }

    return device;
}


bool DirReadJob::shouldCrossIntoFilesystem( const DirInfo * dir ) const
{
    MountPoint * mountPoint = MountPoints::findByPath( dir->url() );

    bool doCross =
        mountPoint                     &&
	! mountPoint->isSystemMount()  &&	//  /dev, /proc, /sys, ...
	! mountPoint->isDuplicate()    &&	//  bind mount or multiple mounted
	! mountPoint->isNetworkMount();		//  NFS or CIFS (Samba)

    logDebug() << ( doCross ? "Reading" : "Not reading" )
	       << " mounted filesystem " << mountPoint->path() << endl;

    return doCross;
}





bool LocalDirReadJob::_warnedAboutNtfsHardLinks = false;


LocalDirReadJob::LocalDirReadJob( DirTree * tree,
				  DirInfo * dir ):
    DirReadJob( tree, dir ),
    _applyFileChildExcludeRules( false ),
    _checkedForNtfs( false ),
    _isNtfs( false )
{
    if ( _dir )
	_dirName = _dir->url();
}


LocalDirReadJob::~LocalDirReadJob()
{
    // NOP
}


void LocalDirReadJob::startReading()
{
    struct dirent * entry;
    struct stat	    statInfo;
    QString	    defaultCacheName = DEFAULT_CACHE_NAME;
    DIR *	    diskDir;

    // logDebug() << _dir << endl;

    bool ok = true;

    if ( access( _dirName.toUtf8(), X_OK | R_OK ) != 0 )
    {
	ok = false;
	logWarning() << "No permission to read directory " << _dirName << endl;
	finishReading( _dir, DirPermissionDenied );
    }

    if ( ok )
    {
	diskDir = ::opendir( _dirName.toUtf8() );

	if ( ! diskDir )
	{
	    logWarning() << "opendir(" << _dirName << ") failed" << endl;
	    ok = false;
	    // opendir() doesn't set 'errno' according to POSIX	 :-(
	    finishReading( _dir, DirError );
	}
    }

    if ( ok )
    {
	_dir->setReadState( DirReading );
	int dirFd = dirfd( diskDir );
	int flags = AT_SYMLINK_NOFOLLOW;

#ifdef AT_NO_AUTOMOUNT
	flags |= AT_NO_AUTOMOUNT;
#endif

	QMultiMap<ino_t, QString> entryMap;

	while ( ( entry = readdir( diskDir ) ) )
	{
	    QString entryName = QString::fromUtf8( entry->d_name );

	    if ( entryName != "."  &&
		 entryName != ".."   )
	    {
		entryMap.insert( entry->d_ino, entryName );
	    }
	}

	// QMultiMap (just like QMap) guarantees sort order by keys, so we are
	// now iterating over the directory entries by i-number order. Most
	// filesystems will benefit from that since they store i-nodes sorted
	// by i-number on disk, so (at least with rotational disks) seek times
	// are minimized by this strategy.
	//
	// Notice that we need a QMultiMap, not just a map: If a file has
	// multiple hard links in the same directory, a QMap would store only
	// one of them, all others would go missing in the DirTree.

	foreach ( QString entryName, entryMap )
	{
	    if ( fstatat( dirFd, entryName.toUtf8(), &statInfo, flags ) == 0 )	// OK?
	    {
		if ( S_ISDIR( statInfo.st_mode ) )	// directory child?
		{
		    DirInfo *subDir = new DirInfo( entryName, &statInfo, _tree, _dir );
		    CHECK_NEW( subDir );

		    processSubDir( entryName, subDir );

		}
		else  // non-directory child
		{
		    if ( entryName == defaultCacheName )	// .qdirstat.cache.gz found?
		    {
			logDebug() << "Found cache file " << defaultCacheName << endl;

			// Try to read the cache file. If that was successful and the toplevel
			// path in that cache file matches the path of the directory we are
			// reading right now, the directory is finished reading, the read job
			// (this object) was just deleted, and we may no longer access any
			// member variables; just return.

			if ( readCacheFile( entryName ) )
			    return;
		    }

#if DONT_TRUST_NTFS_HARD_LINKS

                    if ( statInfo.st_nlink > 1 && isNtfs() )
                    {
                        // NTFS seems to return bogus hard link counts; use 1 instead.
                        // See  https://github.com/shundhammer/qdirstat/issues/88

#if ! VERBOSE_NTFS_HARD_LINKS
                        if ( ! _warnedAboutNtfsHardLinks )
#endif
                        {
                            logWarning() << "Not trusting NTFS with hard links: \""
                                         << _dir->url() << "/" << entryName
                                         << "\" links: " << statInfo.st_nlink
                                         << " -> resetting to 1"
                                         << endl;
                            _warnedAboutNtfsHardLinks = true;
                        }

                        statInfo.st_nlink = 1;
                    }
#endif
		    FileInfo * child = new FileInfo( entryName, &statInfo, _tree, _dir );
		    CHECK_NEW( child );

		    if ( checkIgnoreFilters( entryName ) )
		    {
			// logDebug() << "Ignoring " << child << endl;
			_dir->addToAttic( child );
		    }
		    else
			_dir->insertChild( child );

		    childAdded( child );
		}
	    }
	    else  // lstat() error
	    {
		handleLstatError( entryName );
	    }
	}

	closedir( diskDir );
	DirReadState readState = DirFinished;

	//
	// Check all entries against exclude rules that match against any
	// direct non-directory entry.
	//
	// Doing this now is a performance optimization: This could also be
	// done immediately after each entry is read, but that would mean
	// iterating over all exclude rules for every single directory entry,
	// even if there are no exclude rules that match against any
	// files, so it would be a general performance penalty.
	//
	// Doing this after all entries are read means more cleanup if any
	// exclude rule does match, but that is the exceptional case; if there
	// are no such rules to begin with, the match function returns 'false'
	// immediately, so the performance impact is minimal.
	//
	// Also intentionally not also checking the DirTree specific exclude
	// rules here: They are meant strictly for directory exclude rules.

	if ( _applyFileChildExcludeRules &&
	     ExcludeRules::instance()->matchDirectChildren( _dir ) )
	{
	    excludeDirLate();
	    readState = DirOnRequestOnly;
	}

	finishReading( _dir, readState );
    }

    finished();
    // Don't add anything after finished() since this deletes this job!
}


void LocalDirReadJob::finishReading( DirInfo * dir, DirReadState readState )
{
    // logDebug() << dir << endl;
    CHECK_PTR( dir );

    dir->setReadState( readState );
    dir->finalizeLocal();
    _tree->sendReadJobFinished( dir );
}


void LocalDirReadJob::processSubDir( const QString & entryName, DirInfo * subDir )
{
    _dir->insertChild( subDir );
    childAdded( subDir );

    if ( matchesExcludeRule( entryName ) )
    {
	subDir->setExcluded();
	finishReading( subDir, DirOnRequestOnly );
    }
    else // No exclude rule matched
    {
	if ( ! crossingFilesystems(_dir, subDir ) ) // normal case
	{
	    LocalDirReadJob * job = new LocalDirReadJob( _tree, subDir );
	    CHECK_NEW( job );
	    job->setApplyFileChildExcludeRules( true );
	    _tree->addJob( job );
	}
	else	    // The subdirectory we just found is a mount point.
	{
	    subDir->setMountPoint();

	    if ( _tree->crossFilesystems() && shouldCrossIntoFilesystem( subDir ) )
	    {
		LocalDirReadJob * job = new LocalDirReadJob( _tree, subDir );
		CHECK_NEW( job );
		job->setApplyFileChildExcludeRules( true );
		_tree->addJob( job );
	    }
	    else
	    {
		finishReading( subDir, DirOnRequestOnly );
	    }
	}
    }
}


bool LocalDirReadJob::matchesExcludeRule( const QString & entryName ) const
{
    QString full = fullName( entryName );

    if ( ExcludeRules::instance()->match( full, entryName ) )
	return true;

    if ( ! _tree->excludeRules() )
	return false;

    return _tree->excludeRules()->match( full, entryName );
}


bool LocalDirReadJob::checkIgnoreFilters( const QString & entryName ) const
{
    if ( ! _tree->hasFilters() )
	return false;

    return _tree->checkIgnoreFilters( fullName( entryName ) );
}


bool LocalDirReadJob::readCacheFile( const QString & cacheFileName )
{
    QString cacheFullName = fullName( cacheFileName );
    CacheReadJob * cacheReadJob = new CacheReadJob( _tree, _dir->parent(), cacheFullName );
    CHECK_NEW( cacheReadJob );
    QString firstDirInCache = cacheReadJob->reader()->firstDir();

    if ( firstDirInCache == _dirName )	 // Does this cache file match this directory?
    {
	logDebug() << "Using cache file " << cacheFullName << " for " << _dirName << endl;

	DirTree * tree = _tree;	 // Copy data members to local variables:
	DirInfo * dir  = _dir;	 // This object might be deleted soon by killAll()

	if ( _tree->isTopLevel( _dir ) )
	{
	    logDebug() << "Clearing complete tree" << endl;

	    _tree->clearAndReadCache( cacheFullName );

	    // Since this clears the tree and thus the job queue and thus
	    // deletes this read job, it is important not to do anything after
	    // this point that might access any member variables or even just
	    // uses any virtual method.

	    return true;
	}
	else
	{
	    cacheReadJob->reader()->rewind();  // Read offset was moved by firstDir()
	    _tree->addJob( cacheReadJob );     // The job queue will assume ownership of cacheReadJob

	    if ( _dir->parent() )
		_dir->parent()->setReadState( DirReading );

	    //
	    // Clean up partially read directory content
	    //

	    _queue->killAll( _dir, cacheReadJob );	// Will delete this job as well!
	    // All data members of this object are invalid from here on!

	    logDebug() << "Deleting subtree " << dir << endl;
	    tree->deleteSubtree( dir );
	}

	return true;
    }
    else
    {
	logWarning() << "NOT using cache file " << cacheFullName
		     << " with dir " << firstDirInCache
		     << " for " << _dirName
		     << endl;

	delete cacheReadJob;

	return false;
    }
}


void LocalDirReadJob::excludeDirLate()
{
    logDebug() << "Excluding dir " << _dir << endl;

    // Kill all queued jobs for this dir except this one
    _queue->killAll( _dir, this );

    _tree->clearSubtree( _dir );
    _dir->setExcluded();
}


void LocalDirReadJob::handleLstatError( const QString & entryName )
{
    logWarning() << "lstat(" << fullName( entryName ) << ") failed: "
		 << formatErrno() << endl;

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


QString LocalDirReadJob::fullName( const QString & entryName ) const
{
    QString result = _dirName == "/" ? "" : _dirName;  // Avoid leading // when in root dir
    result += "/" + entryName;

    return result;
}


FileInfo * LocalDirReadJob::stat( const QString & url,
				  DirTree	* tree,
				  DirInfo	* parent,
				  bool		  doThrow )
{
    struct stat statInfo;
    // logDebug() << "url: \"" << url << "\"" << endl;

    if ( lstat( url.toUtf8(), &statInfo ) == 0 ) // lstat() OK
    {
	QString name = url;

	if ( parent && parent != tree->root() )
	{
	    QStringList components = url.split( "/", QString::SkipEmptyParts );
	    name = components.last();
	}

	if ( S_ISDIR( statInfo.st_mode ) )	// directory?
	{
	    DirInfo * dir = new DirInfo( name, &statInfo, tree, parent );
	    CHECK_NEW( dir );

	    if ( parent )
		parent->insertChild( dir );

	    if ( dir && parent &&
		 ! tree->isTopLevel( dir ) &&
		 ! parent->isPkgInfo() &&
		 dir->device() != parent->device() )
	    {
		logDebug() << dir << " is a mount point" << endl;
		dir->setMountPoint();
	    }

	    return dir;
	}
	else					// no directory
	{
	    FileInfo * file = new FileInfo( name, &statInfo, tree, parent );
	    CHECK_NEW( file );

	    if ( parent )
		parent->insertChild( file );

	    return file;
	}
    }
    else // lstat() failed
    {
	if ( doThrow )
	    THROW( SysCallFailedException( "lstat", url ) );

	return 0;
    }
}


bool LocalDirReadJob::isNtfs()
{
    if ( ! _checkedForNtfs )
    {
        _isNtfs         = false;
        _checkedForNtfs = true;

        if ( ! _dirName.isEmpty() )
        {
            MountPoint * mountPoint = MountPoints::findNearestMountPoint( _dirName );
            _isNtfs = mountPoint && mountPoint->isNtfs();
        }
    }

    return _isNtfs;
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
    DirReadJob * job = _queue.takeFirst();

    if ( job )
	job->setQueue( 0 );

    return job;
}


void DirReadJobQueue::clear()
{
    qDeleteAll( _queue );
    qDeleteAll( _blocked );
    _queue.clear();
    _blocked.clear();
}


void DirReadJobQueue::abort()
{
    foreach ( DirReadJob * job, _queue )
    {
	if ( job->dir() )
	    job->dir()->readJobAborted( job->dir() );
    }

    foreach ( DirReadJob * job, _blocked )
    {
	if ( job->dir() )
	    job->dir()->readJobAborted( job->dir() );
    }

    clear();
}


void DirReadJobQueue::killAll( DirInfo * subtree, DirReadJob * exceptJob )
{
    if ( ! subtree )
	return;

    QMutableListIterator<DirReadJob *> it( _queue );
    int count = 0;

    while ( it.hasNext() )
    {
	DirReadJob * job = it.next();

	if ( exceptJob && job == exceptJob )
	{
	    logDebug() << "NOT killing " << job << endl;
	    continue;
	}

	if ( job->dir() && job->dir()->isInSubtree( subtree ) )
	{
	    // logDebug() << "Killing " << job << endl;
	    ++count;
	    it.remove();
	    delete job;
	}
    }

    it = QMutableListIterator<DirReadJob *>( _blocked );

    while ( it.hasNext() )
    {
	DirReadJob * job = it.next();

	if ( exceptJob && job == exceptJob )
	{
	    logDebug() << "NOT killing " << job << endl;
	    continue;
	}

	if ( job->dir() && job->dir()->isInSubtree( subtree ) )
	{
	    // logDebug() << "Killing " << job << endl;
	    ++count;
	    it.remove();
	    delete job;
	}
    }

    logDebug() << "Killed " << count << " read jobs for " << subtree << endl;
}


void DirReadJobQueue::timeSlicedRead()
{
    if ( ! _queue.isEmpty() )
	_queue.first()->read();
}


void DirReadJobQueue::jobFinishedNotify( DirReadJob *job )
{
    if ( job )
    {
	// Get rid of the old (finished) job.

	_queue.removeOne( job );
	delete job;
    }

    // The timer will start a new job when it fires.

    if ( _queue.isEmpty() )	// No new job available - we're done.
    {
	_timer.stop();
	// logDebug() << "No more jobs - finishing" << endl;

	if ( _blocked.isEmpty() )
	    emit finished();
    }
}


void DirReadJobQueue::deletingChildNotify( FileInfo * child )
{
    if ( child && child->isDirInfo() )
    {
	logDebug() << "Killing all pending read jobs for " << child << endl;
	killAll( child->toDirInfo() );
    }
}


void DirReadJobQueue::addBlocked( DirReadJob * job )
{
    _blocked.append( job );
}


void DirReadJobQueue::unblock( DirReadJob * job )
{
    _blocked.removeAll( job );
    enqueue( job );

    if ( _blocked.isEmpty() )
	logDebug() << "No more jobs waiting for external processes" << endl;
}
