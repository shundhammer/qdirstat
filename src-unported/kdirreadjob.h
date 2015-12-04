/*
 *   File name: kdirreadjob.h
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2006-01-07
 */


#ifndef KDirReadJob_h
#define KDirReadJob_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <dirent.h>
#include <qptrlist.h>
#include <qtimer.h>
#include <kdebug.h>
#include <kio/jobclasses.h>

#ifndef NOT_USED
#    define NOT_USED(PARAM)	( (void) (PARAM) )
#endif

// Open a new name space since KDE's name space is pretty much cluttered
// already - all names that would even remotely match are already used up,
// yet the resprective classes don't quite fit the purposes required here.

namespace KDirStat
{
    // Forward declarations
    class KFileInfo;
    class KDirInfo;
    class KDirTree;
    class KCacheReader;
    class KDirReadJobQueue;


    /**
     * A directory read job that can be queued. This is mainly to prevent
     * buffer thrashing because of too many directories opened at the same time
     * because of simultaneous reads or even system resource consumption
     * (directory handles in this case).
     *
     * Objects of this kind are transient by nature: They live only as long as
     * the job is queued or executed. When it is done, the data is contained in
     * the corresponding @ref KDirInfo subtree of the corresponding @ref
     * KDirTree.
     *
     * For each entry automatically a @ref KFileInfo or @ref KDirInfo will be
     * created and added to the parent @ref KDirInfo. For each directory a new
     * @ref KDirReadJob will be created and added to the @ref KDirTree 's job
     * queue.
     *
     * Notice: This class contains pure virtuals - you cannot use it
     * directly. Derive your own class from it or use one of
     * @ref KLocalDirReadJob or @ref KioDirReadJob.
     *
     * @short Abstract base class for directory reading.
     **/
    class KDirReadJob
    {
    public:

	/**
	 * Constructor.
	 *
	 * This does not read anything yet. Call read() for that.
	 **/
	KDirReadJob( KDirTree *tree, KDirInfo *dir = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~KDirReadJob();

	/**
	 * Read the next couple of items from the directory.
	 * Call finished() when there is nothing more to read.
	 *
	 * Derived classes should overwrite this method or startReading().
	 * This default implementation calls startReading() if it has not been
	 * called yet.
	 **/
	virtual void read();

	/**
	 * Returns the corresponding @ref KDirInfo item.
	 * Caution: This may be 0.
	 **/
	virtual KDirInfo * dir() { return _dir; }

	/**
	 * Set the corresponding @ref KDirInfo item.
	 **/
	virtual void setDir( KDirInfo * dir );

	/**
	 * Return the job queue this job is in or 0 if it isn't queued.
	 **/
	KDirReadJobQueue * queue() const { return _queue; }

	/**
	 * Set the job queue this job is in.
	 **/
	void setQueue( KDirReadJobQueue * queue ) { _queue = queue; }


    protected:

	/**
	 * Initialize reading.
	 *
	 * Derived classes should overwrite this method or read().
	 **/
	virtual void startReading() {}

	/**
	 * Notification that a new child has been added.
	 *
	 * Derived classes are required to call this whenever a new child is
	 * added so this notification can be passed up to the @ref KDirTree
	 * which in turn emits a corresponding signal.
	 **/
	void childAdded( KFileInfo *newChild );

	/**
	 * Notification that a child is about to be deleted.
	 *
	 * Derived classes are required to call this just before a child is
	 * deleted so this notification can be passed up to the @ref KDirTree
	 * which in turn emits a corresponding signal.
	 *
	 * Derived classes are not required to handle child deletion at all,
	 * but if they do, calling this method is required.
	 **/
	void deletingChild( KFileInfo *deletedChild );

	/**
	 * Send job finished notification to the associated tree.
	 * This will delete this job.
	 **/
	void finished();


	KDirTree *		_tree;
	KDirInfo *		_dir;
	KDirReadJobQueue *	_queue;
	bool			_started;

    };	// class KDirReadJob



    /**
     * Wrapper class between KDirReadJob and QObject
     **/
    class KObjDirReadJob: public QObject, public KDirReadJob
    {
	Q_OBJECT

    public:

	KObjDirReadJob( KDirTree *tree, KDirInfo *dir = 0 )
	    : QObject(), KDirReadJob( tree, dir ) {};
	virtual ~KObjDirReadJob() {}

    protected slots:

	void slotChildAdded   ( KFileInfo *child )	{ childAdded( child ); }
	void slotDeletingChild( KFileInfo *child )	{ deletingChild( child ); }
	void slotFinished()				{ finished(); }

    };	// KObjDirReadJob



    /**
     * Impementation of the abstract @ref KDirReadJob class that reads a local
     * directory.
     *
     * This will use lstat() system calls rather than KDE's network transparent
     * directory services since lstat() unlike the KDE services can obtain
     * information about the device (i.e. file system) a file or directory
     * resides on. This is important if you wish to limit directory scans to
     * one file system - which is most desirable when that one file system runs
     * out of space.
     *
     * @short Directory reader that reads one local directory.
     **/
    class KLocalDirReadJob: public KDirReadJob
    {
    public:
	/**
	 * Constructor.
	 **/
	KLocalDirReadJob( KDirTree * tree, KDirInfo * dir );

	/**
	 * Destructor.
	 **/
	virtual ~KLocalDirReadJob();

	/**
	 * Obtain information about the URL specified and create a new @ref
	 * KFileInfo or a @ref KDirInfo (whatever is appropriate) from that
	 * information. Use @ref KFileInfo::isDirInfo() to find out which.
	 * Returns 0 if such information cannot be obtained (i.e. the
	 * appropriate stat() call fails).
	 **/
	static KFileInfo * stat( const KURL &	url,
				 KDirTree  *	tree,
				 KDirInfo *	parent = 0 );

    protected:
	
	/**
	 * Read the directory. Prior to this nothing happens.
	 *
	 * Inherited and reimplemented from @ref KDirReadJob.
	 **/
	virtual void startReading();


	DIR * _diskDir;

    };	// KLocalDirReadJob



    /**
     * Generic impementation of the abstract @ref KDirReadJob class, using
     * KDE's network transparent KIO methods.
     *
     * This is much more generic than @ref KLocalDirReadJob since it supports
     * protocols like 'ftp', 'http', 'smb', 'tar' etc., too. Its only drawback
     * is that is cannot be prevented from crossing file system boundaries -
     * which makes it pretty useless for figuring out the cause of a 'file
     * system full' error.
     *
     * @short Generic directory reader that reads one directory, remote or local.
     **/
    class KioDirReadJob: public KObjDirReadJob
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 **/
	KioDirReadJob( KDirTree * tree, KDirInfo * dir );

	/**
	 * Destructor.
	 **/
	virtual ~KioDirReadJob();

	/**
	 * Obtain information about the URL specified and create a new @ref
	 * KFileInfo or a @ref KDirInfo (whatever is appropriate) from that
	 * information. Use @ref KFileInfo::isDirInfo() to find out which.
	 * Returns 0 if such information cannot be obtained (i.e. the
	 * appropriate stat() call fails).
	 **/
	static KFileInfo *	stat( const KURL &	url,
				      KDirTree	*	tree,
				      KDirInfo *	parent = 0 );

	/**
	 * Obtain the owner of the URL specified.
	 *
	 * This is a moderately expensive operation since it involves a network
	 * transparent stat() call.
	 **/
	static QString		owner( KURL url );


    protected slots:
	/**
	 * Receive directory entries from a KIO job.
	 **/
	void entries( KIO::Job *		job,
		      const KIO::UDSEntryList & entryList );

	/**
	 * KIO job is finished.
	 **/
	void finished( KIO::Job * job );

    protected:
	
	/**
	 * Start reading the directory. Prior to this nothing happens.
	 *
	 * Inherited and reimplemented from @ref KDirReadJob.
	 **/
	virtual void startReading();


	KIO::ListJob *	_job;

    };	// KioDirReadJob



    class KCacheReadJob: public KObjDirReadJob
    {
	Q_OBJECT

    public:

	/**
	 * Constructor for a cache reader that is already open.
	 *
	 * The KCacheReadJob takes over ownership of the KCacheReader. In
	 * particular, the KCacheReader will be destroyed with 'delete' when
	 * the read job is done.
	 *
	 * If 'parent' is 0, the content of the cache file will replace all
	 * current tree items.
	 **/
	KCacheReadJob( KDirTree *	tree,
		       KDirInfo *	parent,
		       KCacheReader *	reader );

	/**
	 * Constructor that uses a cache file that is not open yet.
	 *
	 * If 'parent' is 0, the content of the cache file will replace all
	 * current tree items.
	 **/
	KCacheReadJob( KDirTree *	tree,
		       KDirInfo *	parent,
		       const QString &	cacheFileName );

	/**
	 * Destructor.
	 **/
	virtual ~KCacheReadJob();

	/**
	 * Start reading the cache. Prior to this nothing happens.
	 *
	 * Inherited and reimplemented from @ref KDirReadJob.
	 **/
	virtual void read();

	/**
	 * Return the associated cache reader.
	 **/
	KCacheReader * reader() const { return _reader; }


    protected:

	/**
	 * Initializations common for all constructors.
	 **/
	void init();


	KCacheReader * _reader;

    };	// class KCacheReadJob



    /**
     * Queue for read jobs
     *
     * Handles time-sliced reading automatically.
     **/
    class KDirReadJobQueue: public QObject
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	KDirReadJobQueue();

	/**
	 * Destructor.
	 **/
	virtual ~KDirReadJobQueue();

	/**
	 * Add a job to the end of the queue. Begin time-sliced reading if not
	 * in progress yet.
	 **/
	void enqueue( KDirReadJob * job );

	/**
	 * Remove the head of the queue and return it.
	 **/
	KDirReadJob * dequeue();

	/**
	 * Get the head of the queue (the next job that is due for processing).
	 **/
	KDirReadJob * head()	const	{ return _queue.getFirst();	}

	/**
	 * Count the number of pending jobs in the queue.
	 **/
	int count()		const	{ return _queue.count();	}

	/**
	 * Check if the queue is empty.
	 **/
	bool isEmpty()		const	{ return _queue.isEmpty();	}

	/**
	 * Clear the queue: Remove all pending jobs from the queue and destroy them.
	 **/
	void clear();

	/**
	 * Abort all jobs in the queue.
	 **/
	void abort();

	/**
	 * Delete all jobs for a subtree.
	 **/
	void killAll( KDirInfo * subtree );

	/**
	 * Notification that a job is finished.
	 * This takes that job out of the queue and deletes it.
	 * Read jobs are required to call this when they are finished.
	 **/
	void jobFinishedNotify( KDirReadJob *job );

    signals:

	/**
	 * Emitted when job reading starts, i.e. when a new job is inserted
	 * into a queue that was empty
	 **/
	void startingReading();

	/**
	 * Emitted when reading is finished, i.e. when the last read job of the
	 * queue is finished.
	 **/
	void finished();


    protected slots:

	/**
	 * Time-sliced work procedure to be performed while the application is
	 * in the main loop: Read some directory entries, but relinquish
	 * control back to the application so it can maintain some
	 * responsiveness. This method uses a timer of minimal duration to
	 * activate itself as soon as there are no more user events to
	 * process. Call this only once directly after inserting a read job
	 * into the job queue.
	 **/
	void timeSlicedRead();


    protected:

	QPtrList<KDirReadJob>	_queue;
	QTimer			_timer;
    };


}	// namespace KDirStat


#endif // ifndef KDirReadJob_h


// EOF
