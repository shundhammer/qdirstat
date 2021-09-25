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
    class MountPoint;


    /**
     * A directory read job that can be queued. This is mainly to prevent
     * buffer thrashing because of too many directories opened at the same time
     * because of simultaneous reads or even system resource consumption
     * (directory handles in this case).
     *
     * Objects of this kind are transient by nature: They live only as long as
     * the job is queued or executed. When it is done, the data is contained in
     * the corresponding DirInfo subtree of the corresponding DirTree.
     *
     * For each entry automatically a FileInfo or DirInfo will be created and
     * added to the parent DirInfo. For each directory a new DirReadJob will be
     * created and added to the DirTree 's job queue.
     *
     * Notice: This class contains pure virtuals - you cannot use it directly.
     * Derive your own class from it or use one of LocalDirReadJob or
     * CacheReadJob.
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
	 * Returns the corresponding DirInfo item.
	 * Caution: This may be 0.
	 **/
	virtual DirInfo * dir() { return _dir; }

	/**
	 * Set the corresponding DirInfo item.
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
	 * added so this notification can be passed up to the DirTree which in
	 * turn emits a corresponding signal.
	 **/
	void childAdded( FileInfo *newChild );

	/**
	 * Notification that a child is about to be deleted.
	 *
	 * Derived classes are required to call this just before a child is
	 * deleted so this notification can be passed up to the DirTree which
	 * in turn emits a corresponding signal.
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
	 * Check if going from 'parent' to 'child' would cross a filesystem
	 * boundary. This take Btrfs subvolumes into account.
	 **/
	bool crossingFilesystems( DirInfo * parent, DirInfo * child );

	/**
	 * Return the device name where 'dir' is on if it's a mount point.
	 * This uses MountPoints which reads /proc/mounts.
	 **/
	QString device( const DirInfo * dir ) const;

	/**
	 * Check if we really should cross into a mounted filesystem; don't do
	 * it if this is a system mount, a bind mount, a filesystem mounted
	 * multiple times, or a network mount (NFS / Samba).
	 **/
	bool shouldCrossIntoFilesystem( const DirInfo * dir ) const;


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
     * Implementation of the abstract DirReadJob class that reads a local
     * directory.
     *
     * This will use lstat() system calls rather than KDE's network transparent
     * directory services since lstat() unlike the KDE services can obtain
     * information about the device (i.e. filesystem) a file or directory
     * resides on. This is important if you wish to limit directory scans to
     * one filesystem - which is most desirable when that one filesystem runs
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
	 * Obtain information about the URL specified and create a new FileInfo
	 * or a DirInfo (whatever is appropriate) from that information. Use
	 * FileInfo::isDirInfo() to find out which.
	 *
	 * If the underlying syscall fails, this throws a SysCallException if
	 * 'doThrow' is 'true', and it just returns 0 if it is 'false'.
	 **/
	static FileInfo * stat( const QString & url,
				DirTree	      * tree,
				DirInfo	      * parent	= 0,
				bool		doThrow = true );

	/**
	 * Return 'true' if any exclude rules matching against any direct file
	 * child should be applied. This is generally useful only for
	 * second-level read jobs, not for the starting point of a directory
	 * scan, so it is easily possible to continue reading at an excluded
	 * directory.
	 *
	 * The default is 'false'.
	 **/
	bool applyFileChildExcludeRules() const
	    { return _applyFileChildExcludeRules; }

	/**
	 * Set the applyFileChildExcludeRules flag.
	 **/
	void setApplyFileChildExcludeRules( bool val )
	    { _applyFileChildExcludeRules = val; }

    protected:

	/**
	 * Read the directory. Prior to this nothing happens.
	 *
	 * Inherited and reimplemented from DirReadJob.
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
	 * Return 'true' if 'entryName' matches an exclude rule of the
	 * ExcludeRule singleton or a temporary exclude rule of the DirTree.
	 **/
	bool matchesExcludeRule( const QString & entryName ) const;

	/**
	 * Return 'true' if 'entryName' should be ignored.
	 **/
	bool checkIgnoreFilters( const QString & entryName ) const;

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
	 * Exclude the directory of this read job after it is almost completely
	 * read. This is used when checking for exclude rules matching direct
	 * file children of a directory.
	 *
	 * The main purpose of having this as a separate function is to have a
	 * clear backtrace if it segfaults.
	 **/
	void excludeDirLate();

	/**
	 * Return the full name with path of an entry of this directory.
	 **/
	QString fullName( const QString & entryName ) const;

	/**
	 * Return 'true' if the current filesystem is NTFS.
	 **/
	bool isNtfs();


	//
	// Data members
	//

	QString _dirName;
	bool	_applyFileChildExcludeRules;
	bool	_checkedForNtfs;
	bool	_isNtfs;

	static bool _warnedAboutNtfsHardLinks;

    };	// LocalDirReadJob



    class CacheReadJob: public ObjDirReadJob
    {
	Q_OBJECT

    public:

	/**
	 * Constructor for a cache reader that is already open.
	 *
	 * The CacheReadJob takes over ownership of the CacheReader. In
	 * particular, the CacheReader will be destroyed with 'delete' when the
	 * read job is done.
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
	 * Inherited and reimplemented from DirReadJob.
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
	int count() const   { return _queue.count() + _blocked.count(); }

	/**
	 * Check if the queue is empty.
	 **/
	bool isEmpty() const { return _queue.isEmpty() && _blocked.isEmpty(); }

	/**
	 * Add a job to the list of blocked jobs: Jobs that are not yet ready
	 * yet, e.g. because they are waiting for results from an external
	 * process.
	 **/
	void addBlocked( DirReadJob * job );

	/**
	 * Notification that a job that was blocked is now ready to be
	 * scheduled, so it will be taken out of the list of blocked jobs and
	 * added to the end of the queue.
	 **/
	void unblock( DirReadJob * job );

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
	QList<DirReadJob *>  _blocked;
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
