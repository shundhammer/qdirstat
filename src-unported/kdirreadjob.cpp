/*
 *   File name:	kdirreadjob.cpp
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2008-12-18
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <sys/errno.h>
#include <klocale.h>
#include <kapp.h>
#include <kio/job.h>
#include <kio/netaccess.h>

#include "kdirtree.h"
#include "kdirreadjob.h"
#include "kdirtreecache.h"
#include "kexcluderules.h"


using namespace KDirStat;


KDirReadJob::KDirReadJob( KDirTree * tree,
			  KDirInfo * dir  )
    : _tree( tree )
    , _dir( dir )
{
    _queue	= 0;
    _started	= false;

    if ( _dir )
	_dir->readJobAdded();
}


KDirReadJob::~KDirReadJob()
{
    if ( _dir )
	_dir->readJobFinished();
}


/**
 * Default implementation - derived classes should overwrite this method or
 * startReading() (or both).
 **/

void
KDirReadJob::read()
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
KDirReadJob::setDir( KDirInfo * dir )
{
    _dir = dir;
}


void
KDirReadJob::finished()
{
    if ( _queue )
	_queue->jobFinishedNotify( this );
    else
	kdError() << "No job queue for " << _dir << endl;
}


void
KDirReadJob::childAdded( KFileInfo *newChild )
{
    _tree->childAddedNotify( newChild );
}


void
KDirReadJob::deletingChild( KFileInfo *deletedChild )
{
    _tree->deletingChildNotify( deletedChild );
}





KLocalDirReadJob::KLocalDirReadJob( KDirTree *	tree,
				    KDirInfo *	dir )
    : KDirReadJob( tree, dir )
    , _diskDir( 0 )
{
}


KLocalDirReadJob::~KLocalDirReadJob()
{
}


void
KLocalDirReadJob::startReading()
{
    struct dirent *	entry;
    struct stat		statInfo;
    QString		dirName		 = _dir->url();
    QString		defaultCacheName = DEFAULT_CACHE_NAME;

    if ( ( _diskDir = opendir( dirName ) ) )
    {
	_tree->sendProgressInfo( dirName );
	_dir->setReadState( KDirReading );

	while ( ( entry = readdir( _diskDir ) ) )
	{
	    QString entryName = entry->d_name;

	    if ( entryName != "."  &&
		 entryName != ".."   )
	    {
		QString fullName = dirName + "/" + entryName;

		if ( lstat( fullName, &statInfo ) == 0 )	// lstat() OK
		{
		    if ( S_ISDIR( statInfo.st_mode ) )	// directory child?
		    {
			KDirInfo *subDir = new KDirInfo( entryName, &statInfo, _tree, _dir );
			_dir->insertChild( subDir );
			childAdded( subDir );

			if ( KExcludeRules::excludeRules()->match( fullName ) )
			{
			    subDir->setExcluded();
			    subDir->setReadState( KDirOnRequestOnly );
			    _tree->sendFinalizeLocal( subDir );
			    subDir->finalizeLocal();
			}
			else // No exclude rule matched
			{
			    if ( _dir->device() == subDir->device()	)	// normal case
			    {
				_tree->addJob( new KLocalDirReadJob( _tree, subDir ) );
			    }
			    else	// The subdirectory we just found is a mount point.
			    {
				// kdDebug() << "Found mount point " << subDir << endl;
				subDir->setMountPoint();

				if ( _tree->crossFileSystems() )
				{
				    _tree->addJob( new KLocalDirReadJob( _tree, subDir ) );
				}
				else
				{
				    subDir->setReadState( KDirOnRequestOnly );
				    _tree->sendFinalizeLocal( subDir );
				    subDir->finalizeLocal();
				}
			    }
			}
		    }
		    else		// non-directory child
		    {
			if ( entryName == defaultCacheName )	// .kdirstat.cache.gz found?
			{
			    //
			    // Read content of this subdirectory from cache file
			    //

			    
			    KCacheReadJob * cacheReadJob = new KCacheReadJob( _tree, _dir->parent(), fullName );
			    CHECK_PTR( cacheReadJob );
			    QString firstDirInCache = cacheReadJob->reader()->firstDir();

			    if ( firstDirInCache == dirName )	// Does this cache file match this directory?
			    {
				kdDebug() << "Using cache file " << fullName << " for " << dirName << endl;

				cacheReadJob->reader()->rewind();	// Read offset was moved by firstDir()
				_tree->addJob( cacheReadJob );	// Job queue will assume ownership of cacheReadJob

				//
				// Clean up partially read directory content
				//

				KDirTree * tree = _tree;	// Copy data members to local variables:
				KDirInfo * dir  = _dir;		// This object will be deleted soon by killAll()

				_queue->killAll( dir );		// Will delete this job as well!
				// All data members of this object are invalid from here on!

				tree->deleteSubtree( dir );

				return;
			    }
			    else
			    {
				kdDebug() << "NOT using cache file " << fullName
					  << " with dir " << firstDirInCache
					  << " for " << dirName
					  << endl;

				delete cacheReadJob;
			    }
			}
			else
			{
			    KFileInfo *child = new KFileInfo( entryName, &statInfo, _tree, _dir );
			    _dir->insertChild( child );
			    childAdded( child );
			}
		    }
		}
		else			// lstat() error
		{
		    kdWarning() << "lstat(" << fullName << ") failed: " << strerror( errno ) << endl;

		    /*
		     * Not much we can do when lstat() didn't work; let's at
		     * least create an (almost empty) entry as a placeholder.
		     */
		    KDirInfo *child = new KDirInfo( _tree, _dir, entry->d_name );
		    child->setReadState( KDirError );
		    _dir->insertChild( child );
		    childAdded( child );
		}
	    }
	}

	closedir( _diskDir );
	// kdDebug() << "Finished reading " << _dir << endl;
	_dir->setReadState( KDirFinished );
	_tree->sendFinalizeLocal( _dir );
	_dir->finalizeLocal();
    }
    else
    {
	_dir->setReadState( KDirError );
	_tree->sendFinalizeLocal( _dir );
	_dir->finalizeLocal();
	// kdWarning() << k_funcinfo << "opendir(" << dirName << ") failed" << endl;
	// opendir() doesn't set 'errno' according to POSIX  :-(
    }

    finished();
    // Don't add anything after finished() since this deletes this job!
}



KFileInfo *
KLocalDirReadJob::stat( const KURL & 	url,
			KDirTree  *	tree,
			KDirInfo * 	parent )
{
    struct stat statInfo;

    if ( lstat( url.path(), &statInfo ) == 0 )		// lstat() OK
    {
	QString name = parent ? url.filename() : url.path();

	if ( S_ISDIR( statInfo.st_mode ) )		// directory?
	{
	    KDirInfo * dir = new KDirInfo( name, &statInfo, tree, parent );

	    if ( dir && parent && dir->device() != parent->device() )
		dir->setMountPoint();

	    return dir;
	}
	else						// no directory
	    return new KFileInfo( name, &statInfo, tree, parent );
    }
    else	// lstat() failed
	return 0;
}






KioDirReadJob::KioDirReadJob( KDirTree *	tree,
			      KDirInfo *	dir )
    : KObjDirReadJob( tree, dir )
{
    _job = 0;
}


KioDirReadJob::~KioDirReadJob()
{
#if 0
    if ( _job )
	_job->kill( true );	// quietly
#endif
}


void
KioDirReadJob::startReading()
{
    KURL url( _dir->url() );

    if ( ! url.isValid() )
    {
	kdWarning() << k_funcinfo << "URL malformed: " << _dir->url() << endl;
    }

    _job = KIO::listDir( url,
			 false );	// showProgressInfo

    connect( _job, SIGNAL( entries( KIO::Job *, const KIO::UDSEntryList& ) ),
             this, SLOT  ( entries( KIO::Job *, const KIO::UDSEntryList& ) ) );

    connect( _job, SIGNAL( result  ( KIO::Job * ) ),
	     this, SLOT  ( finished( KIO::Job * ) ) );

    connect( _job, SIGNAL( canceled( KIO::Job * ) ),
	     this, SLOT  ( finished( KIO::Job * ) ) );
}


void
KioDirReadJob::entries ( KIO::Job *			job,
			 const KIO::UDSEntryList &	entryList )
{
    NOT_USED( job );
    KURL url( _dir->url() );	// Cache this - it's expensive!

    if ( ! url.isValid() )
    {
	kdWarning() << k_funcinfo << "URL malformed: " << _dir->url() << endl;
    }

    KIO::UDSEntryListConstIterator it = entryList.begin();

    while ( it != entryList.end() )
    {
	KFileItem entry( *it,
			 url,
			 true,		// determineMimeTypeOnDemand
			 true );	// URL is parent directory

	if ( entry.name() != "." &&
	     entry.name() != ".."  )
	{
	    // kdDebug() << "Found " << entry.url().url() << endl;

	    if ( entry.isDir()    &&	// Directory child
		 ! entry.isLink()   )	// and not a symlink?
	    {
		KDirInfo *subDir = new KDirInfo( &entry, _tree, _dir );
		_dir->insertChild( subDir );
		childAdded( subDir );

		if ( KExcludeRules::excludeRules()->match( url.path() ) )
		{
		    subDir->setExcluded();
		    subDir->setReadState( KDirOnRequestOnly );
		    _tree->sendFinalizeLocal( subDir );
		    subDir->finalizeLocal();
		}
		else // No exclude rule matched
		{
		    _tree->addJob( new KioDirReadJob( _tree, subDir ) );
		}
	    }
	    else	// non-directory child
	    {
		KFileInfo *child = new KFileInfo( &entry, _tree, _dir );
		_dir->insertChild( child );
		childAdded( child );
	    }
	}

	++it;
    }
}


void
KioDirReadJob::finished( KIO::Job * job )
{
    if ( job->error() )
	_dir->setReadState( KDirError );
    else
	_dir->setReadState( KDirFinished );

    _tree->sendFinalizeLocal( _dir );
    _dir->finalizeLocal();
    _job = 0;	// The job deletes itself after this signal!

    KDirReadJob::finished();
    // Don't add anything after finished() since this deletes this job!
}



KFileInfo *
KioDirReadJob::stat( const KURL & 	url,
		      KDirTree  * 	tree,
		      KDirInfo  * 	parent )
{
    KIO::UDSEntry uds_entry;

    if ( KIO::NetAccess::stat( url, uds_entry, qApp->mainWidget() ) )	// remote stat() OK?
    {
	KFileItem entry( uds_entry, url,
			 true,		// determine MIME type on demand
			 false );	// URL specifies parent directory

	return entry.isDir() ? new KDirInfo ( &entry, tree, parent ) : new KFileInfo( &entry, tree, parent );
    }
    else	// remote stat() failed
	return 0;
}


QString
KioDirReadJob::owner( KURL url )
{
    KIO::UDSEntry uds_entry;

    if ( KIO::NetAccess::stat( url, uds_entry, qApp->mainWidget() ) )	// remote stat() OK?
    {
	KFileItem entry( uds_entry, url,
			 true,		// determine MIME type on demand
			 false );	// URL specifies parent directory

	return entry.user();
    }

    return QString();
}





KCacheReadJob::KCacheReadJob( KDirTree *	tree,
			      KDirInfo *	parent,
			      KCacheReader *	reader )
    : KObjDirReadJob( tree, parent )
    , _reader( reader )
{
    if ( _reader )
	_reader->rewind();

    init();
}


KCacheReadJob::KCacheReadJob( KDirTree *	tree,
			      KDirInfo *	parent,
			      const QString &	cacheFileName )
    : KObjDirReadJob( tree, parent )
{
    _reader = new KCacheReader( cacheFileName, tree, parent );
    CHECK_PTR( _reader );

    init();
}


void
KCacheReadJob::init()
{
    if ( _reader )
    {
	if ( _reader->ok() )
	{
	    connect( _reader,	SIGNAL( childAdded    ( KFileInfo * ) ),
		     this,	SLOT  ( slotChildAdded( KFileInfo * ) ) );
	}
	else
	{
	    delete _reader;
	    _reader = 0;
	}
    }
}


KCacheReadJob::~KCacheReadJob()
{
    if ( _reader )
	delete _reader;
}


void
KCacheReadJob::read()
{
    /*
     * This will be called repeatedly from KDirTree::timeSlicedRead() until
     * finished() is called.
     */

    if ( ! _reader )
	finished();

    // kdDebug() << "Reading 1000 cache lines" << endl;
    _reader->read( 1000 );
    _tree->sendProgressInfo( "" );

    if ( _reader->eof() || ! _reader->ok() )
    {
	// kdDebug() << "Cache reading finished - ok: " << _reader->ok() << endl;
	finished();
    }
}





KDirReadJobQueue::KDirReadJobQueue()
    : QObject()
{
    _queue.setAutoDelete( false );

    connect( &_timer, SIGNAL( timeout() ),
	     this,    SLOT  ( timeSlicedRead() ) );
}


KDirReadJobQueue::~KDirReadJobQueue()
{
    clear();
}


void
KDirReadJobQueue::enqueue( KDirReadJob * job )
{
    if ( job )
    {
	_queue.append( job );
	job->setQueue( this );

	if ( ! _timer.isActive() )
	{
	    // kdDebug() << "First job queued" << endl;
	    emit startingReading();
	    _timer.start( 0 );
	}
    }
}


KDirReadJob *
KDirReadJobQueue::dequeue()
{
    KDirReadJob * job = _queue.getFirst();
    _queue.removeFirst();

    if ( job )
	job->setQueue( 0 );

    return job;
}


void
KDirReadJobQueue::clear()
{
    _queue.first();		// set _queue.current() to the first position

    while ( KDirReadJob * job = _queue.current() )
    {
	_queue.remove();	// remove current() and move current() to next
	delete job;

	_queue.next();		// move current() on
    }
}


void
KDirReadJobQueue::abort()
{
    while ( ! _queue.isEmpty() )
    {
	KDirReadJob * job = _queue.getFirst();

	if ( job->dir() )
	    job->dir()->readJobAborted();

	_queue.removeFirst();
	delete job;
    }
}


void
KDirReadJobQueue::killAll( KDirInfo * subtree )
{
    if ( ! subtree )
	return;
    
    _queue.first();		// set _queue.current() to the first position

    while ( KDirReadJob * job = _queue.current() )
    {
	if ( job->dir() && job->dir()->isInSubtree( subtree ) )
	{
	    // kdDebug() << "Killing read job " << job->dir() << endl;
	    
	    _queue.remove();	// remove current() and move current() to next
	    delete job;
	}
	else
	{
	    _queue.next();	// move current() on
	}
    }
}


void
KDirReadJobQueue::timeSlicedRead()
{
    if ( ! _queue.isEmpty() )
	_queue.getFirst()->read();
}


void
KDirReadJobQueue::jobFinishedNotify( KDirReadJob *job )
{
    // Get rid of the old (finished) job.

    _queue.removeFirst();
    delete job;

    // Look for a new job.

    if ( _queue.isEmpty() )	// No new job available - we're done.
    {
	_timer.stop();
	// kdDebug() << "No more jobs - finishing" << endl;
	emit finished();
    }
}




// EOF
