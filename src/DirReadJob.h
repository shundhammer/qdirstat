/*
 *   File name: DirReadJob.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirReadJob_h
#define DirReadJob_h


#include <dirent.h>
#include <QTimer>

#include "FileInfo.h"
#include "Logger.h"


namespace QDirStat
{
    // Forward declarations
    class DirInfo;
    class DirTree;
    class CacheReader;
    class DirReadJobQueue;


    /**
     * A directory read job that can be queued. This is mainly to prevent
     * buffer thrashing because of too many directories opened at the same time
     * because of simultaneous reads or even system resource consumption
     * (directory handles in this case).
     *
     * Objects of this kind are transient by nature: They live only as long as
     * the job is queued or executed. When it is done, the data is contained in
     * the corresponding @ref DirInfo subtree of the corresponding @ref
     * DirTree.
     *
     * For each entry automatically a @ref FileInfo or @ref DirInfo will be
     * created and added to the parent @ref DirInfo. For each directory a new
     * @ref DirReadJob will be created and added to the @ref DirTree 's job
     * queue.
     *
     * Notice: This class contains pure virtuals - you cannot use it
     * directly. Derive your own class from it or use one of
     * @ref LocalDirReadJob or @ref CacheReadJob.
     *
     * @short Abstract base class for directory reading.
     **/
    class DirReadJob
    {
    public:

	/**
	 * Constructor.
	 *
	 * This does not read anything yet. Call read() for that.
	 **/
	DirReadJob( DirTree *tree, DirInfo *dir = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~DirReadJob();

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
	 * Returns the corresponding @ref DirInfo item.
	 * Caution: This may be 0.
	 **/
	virtual DirInfo * dir() { return _dir; }

	/**
	 * Set the corresponding @ref DirInfo item.
	 **/
	virtual void setDir( DirInfo * dir );

	/**
	 * Return the corresponding DirTree.
	 **/
	DirTree * tree() const { return _tree; }

	/**
	 * Return the job queue this job is in or 0 if it isn't queued.
	 **/
	DirReadJobQueue * queue() const { return _queue; }

	/**
	 * Set the job queue this job is in.
	 **/
	void setQueue( DirReadJobQueue * queue ) { _queue = queue; }


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
	 * added so this notification can be passed up to the @ref DirTree
	 * which in turn emits a corresponding signal.
	 **/
	void childAdded( FileInfo *newChild );

	/**
	 * Notification that a child is about to be deleted.
	 *
	 * Derived classes are required to call this just before a child is
	 * deleted so this notification can be passed up to the @ref DirTree
	 * which in turn emits a corresponding signal.
	 *
	 * Derived classes are not required to handle child deletion at all,
	 * but if they do, calling this method is required.
	 **/
	void deletingChild( FileInfo *deletedChild );

	/**
	 * Send job finished notification to the associated tree.
	 * This will delete this job.
	 **/
	void finished();

	/**
	 * Check if going from 'parent' to 'child' would cross a file system
	 * boundary. This take Btrfs subvolumes into account.
	 **/
	bool crossingFileSystems( DirInfo * parent, DirInfo * child );

	/**
	 * Return the device name where 'dir' is on if it's a mount point.
	 * This uses MountPoints which reads /proc/mounts.
	 **/
	QString device( const DirInfo * dir ) const;


	DirTree *	   _tree;
	DirInfo *	   _dir;
	DirReadJobQueue *  _queue;
	bool		   _started;

    };	// class DirReadJob



    /**
     * Wrapper class between DirReadJob and QObject
     **/
    class ObjDirReadJob: public QObject, public DirReadJob
    {
	Q_OBJECT

    public:

	ObjDirReadJob( DirTree *tree, DirInfo *dir = 0 )
	    : QObject(), DirReadJob( tree, dir ) {};
	virtual ~ObjDirReadJob() {}

    protected slots:

	void slotChildAdded   ( FileInfo *child ) { childAdded( child ); }
	void slotDeletingChild( FileInfo *child ) { deletingChild( child ); }
	void slotFinished()			  { finished(); }

    };	// ObjDirReadJob



    /**
     * Impementation of the abstract @ref DirReadJob class that reads a local
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
    class LocalDirReadJob: public DirReadJob
    {
    public:
	/**
	 * Constructor.
	 **/
	LocalDirReadJob( DirTree * tree, DirInfo * dir );

	/**
	 * Destructor.
	 **/
	virtual ~LocalDirReadJob();

	/**
	 * Obtain information about the URL specified and create a new @ref
	 * FileInfo or a @ref DirInfo (whatever is appropriate) from that
	 * information. Use @ref FileInfo::isDirInfo() to find out which.
	 * Returns 0 if such information cannot be obtained (i.e. the
	 * appropriate stat() call fails).
	 **/
	static FileInfo * stat( const QString & url,
				DirTree	      * tree,
				DirInfo	      * parent = 0 );

    protected:

	/**
	 * Read the directory. Prior to this nothing happens.
	 *
	 * Inherited and reimplemented from @ref DirReadJob.
	 **/
	virtual void startReading();

	/**
	 * Finish reading the directory: Set the specified read state, send
	 * signals and finalize the directory (clean up dot entries etc.).
	 **/
	void finishReading( DirInfo * dir, DirReadState readState );

	/**
	 * Process one subdirectory entry.
	 **/
	void processSubDir( const QString & entryName,
			    DirInfo	  * subDir    );

	/**
	 * Read a cache file that was picked up along the way:
	 *
	 * If one of the non-directory entries of this directory was
	 * ".qdirstat.cache.gz", open it, and if the toplevel entry in that
	 * file matches the current path, read all the cache contents, kill all
	 * pending read jobs for subdirectories of this directory and return
	 * 'true'. In that case, the current read job is finished and deleted
	 * (!), control needs to be returned to the caller, and using any data
	 * members of this object is no longer safe (since they have just been
	 * deleted).
	 *
	 * In all other cases, consider that entry as a plain file and return
	 * 'false'.
	 **/
	bool readCacheFile( const QString & cacheFileName );

	/**
	 * Handle an error during lstat() of a directory entry.
	 **/
	void handleLstatError( const QString & entryName );

	/**
	 * Return the full name with path of an entry of this directory.
	 **/
	QString fullName( const QString & entryName ) const;


	//
	// Data members
	//

	QString _dirName;
	DIR *	_diskDir;

    };	// LocalDirReadJob



    class CacheReadJob: public ObjDirReadJob
    {
	Q_OBJECT

    public:

	/**
	 * Constructor for a cache reader that is already open.
	 *
	 * The CacheReadJob takes over ownership of the CacheReader. In
	 * particular, the CacheReader will be destroyed with 'delete' when
	 * the read job is done.
	 *
	 * If 'parent' is 0, the content of the cache file will replace all
	 * current tree items.
	 **/
	CacheReadJob( DirTree	  * tree,
		      DirInfo	  * parent,
		      CacheReader * reader );

	/**
	 * Constructor that uses a cache file that is not open yet.
	 *
	 * If 'parent' is 0, the content of the cache file will replace all
	 * current tree items.
	 **/
	CacheReadJob( DirTree *	       tree,
		      DirInfo *	       parent,
		      const QString &  cacheFileName );

	/**
	 * Destructor.
	 **/
	virtual ~CacheReadJob();

	/**
	 * Start reading the cache. Prior to this nothing happens.
	 *
	 * Inherited and reimplemented from @ref DirReadJob.
	 **/
	virtual void read();

	/**
	 * Return the associated cache reader.
	 **/
	CacheReader * reader() const { return _reader; }


    protected:

	/**
	 * Initializations common for all constructors.
	 **/
	void init();


	CacheReader * _reader;

    };	// class CacheReadJob



    /**
     * Queue for read jobs
     *
     * Handles time-sliced reading automatically.
     **/
    class DirReadJobQueue: public QObject
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	DirReadJobQueue();

	/**
	 * Destructor.
	 **/
	virtual ~DirReadJobQueue();

	/**
	 * Add a job to the end of the queue. Begin time-sliced reading if not
	 * in progress yet.
	 **/
	void enqueue( DirReadJob * job );

	/**
	 * Remove the head of the queue and return it.
	 **/
	DirReadJob * dequeue();

	/**
	 * Get the head of the queue (the next job that is due for processing).
	 **/
	DirReadJob * head() const { return _queue.first();}

	/**
	 * Count the number of pending jobs in the queue.
	 **/
	int count() const   { return _queue.count(); }

	/**
	 * Check if the queue is empty.
	 **/
	bool isEmpty() const { return _queue.isEmpty(); }

	/**
	 * Clear the queue: Remove all pending jobs from the queue and destroy
	 * them.
	 **/
	void clear();

	/**
	 * Abort all jobs in the queue.
	 **/
	void abort();

	/**
	 * Delete all jobs for a subtree except 'exceptJob'.
	 **/
	void killAll( DirInfo * subtree, DirReadJob * exceptJob = 0 );

	/**
	 * Notification that a job is finished.
	 * This takes that job out of the queue and deletes it.
	 * Read jobs are required to call this when they are finished.
	 **/
	void jobFinishedNotify( DirReadJob *job );

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


    public slots:

	/**
	 * Notification that a child node is about to be deleted from the
	 * outside (i.e., not from this ReadJobQueue), e.g. because of cleanup
	 * actions. This will remove all pending directory read jobs for that
	 * subtree from the job queue.
	 **/
	void deletingChildNotify( FileInfo * child );


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

	QList<DirReadJob *>  _queue;
	QTimer		     _timer;
    };


    /**
     * Human-readable output of a DirReadJob in a debug stream.
     **/
    inline QTextStream & operator<< ( QTextStream & str, DirReadJob * job )
    {
	if ( job )
	{
	    CacheReadJob * cacheReadJob = dynamic_cast<CacheReadJob *>( job );
	    QString jobType = cacheReadJob ? "CacheReadJob" : "DirReadJob";
	    str << "<" << jobType << " " << job->dir() << ">";
	}
	else
	    str << "<NULL DirReadJob *>";

	return str;
    }

}	// namespace QDirStat


#endif // ifndef DirReadJob_h
