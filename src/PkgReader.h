/*
 *   File name: PkgReader.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef PkgReader_h
#define PkgReader_h

#include <QMap>
#include <QSharedPointer>

#include "DirReadJob.h"
#include "PkgInfo.h"
#include "PkgFilter.h"
#include "Process.h"


namespace QDirStat
{
    // Forward declarations
    class DirTree;
    class PkgFileListCache;


    /**
     * A class for reading information about installed packages.
     *
     * This uses PkgQuery and PkgManager to read first the installed packages
     * and then one by one the file list for each of those packages in a read
     * job very much like DirReadJob.
     **/
    class PkgReader
    {
    public:

	/**
	 * Constructor. Even though this object creates a PkgReadJob for each
	 * package it finds, it is not necessary to keep this reader around
	 * longer than after read() is finished: Once created and queued, the
	 * PkgReadJobs are self-sufficient. They don't need this reader.
	 **/
	PkgReader( DirTree * tree );

	/**
	 * Destructor.
	 **/
	~PkgReader();

	/**
	 * Read installed packages from the system's package manager(s), select
	 * those that match the specified filter and create a PkgReadJob for
	 * each one to read its file list.
         *
         * Like all read jobs, this is done with a zero duration timer in the
	 * Qt event loop, so whenever there is no user or X11 event to process,
	 * it will pick one read job and execute it.
	 **/
	void read( const PkgFilter & filter );

	/**
	 * Read parameters from the settings file.
	 **/
	void readSettings();

	/**
	 * Write parameters to the settings file.
	 **/
	void writeSettings();


    protected:

        /**
         * Filter the package list: Remove those package that don't match the
         * filter.
         **/
        void filterPkgList( const PkgFilter & filter );

	/**
	 * Handle packages that are installed in multiple versions or for
	 * multiple architectures: Assign a different display name to each of
	 * them.
	 **/
	void handleMultiPkg();

	/**
	 * Create a suitable display names for a package: Packages that are
	 * only installed in one version or for one architecture will simply
	 * keep their base name; others will have the version and/or the
	 * architecture appended so the user can tell them apart.
	 **/
	void createDisplayName( const QString & pkgName );

	/**
	 * Add the packages to the DirTree.
	 **/
	void addPkgToTree();

        /**
         * Create a read job for each package to read its file list from a file
         * list cache and add it to the read job queue.
         **/
        void createCachePkgReadJobs();

        /**
         * Create a read job for each package with a background process to read
         * its file list and add it as a blocked job to the read job queue.
         **/
        void createAsyncPkgReadJobs();

        /**
         * Create a process for reading the file list for 'pkg' with the
         * appropriate external command. The process is not started yet.
         **/
        Process * createReadFileListProcess( PkgInfo * pkg );


	// Data members

	DirTree *                       _tree;
	PkgInfoList			_pkgList;
	QMultiMap<QString, PkgInfo *>	_multiPkg;
        int                             _maxParallelProcesses;
        int                             _minCachePkgListSize;

    };	// class PkgReader




    /**
     * Read job class for reading information about a package. This is the base
     * class with a simplistic approach that just starts the external command
     * used for getting the file list when needed and then waits for it to
     * return a result.
     *
     * See also AsyncPkgReadJob and CachePkgReadJob.
     **/
    class PkgReadJob: public ObjDirReadJob
    {
        Q_OBJECT

    public:

	/**
	 * Constructor: Prepare to read the file list of existing PkgInfo node
	 * 'pkg' and create a DirInfo or FileInfo node for each item in the
	 * file list below 'pkg'.

         * process.  Reading is then started from the outside with
         * startReading().
	 **/
	PkgReadJob( DirTree * tree,
                    PkgInfo * pkg );

	/**
	 * Destructor.
	 **/
	virtual ~PkgReadJob();

	/**
	 * Start reading the file list of the package.
	 *
	 * Reimplemented from DirReadJob.
	 **/
	virtual void startReading() Q_DECL_OVERRIDE;

	/**
	 * Return the parent PkgInfo node.
	 **/
	PkgInfo * pkg() const { return _pkg; }


    protected:

        /**
         * Get the file list for this package.  This default implementation
         * does a simple PkgQuery::fileList() call.
         *
         * Derived classes might want to do something more sophisticated like
         * using a background process (AsyncPkgReader) or a file list cache
         * (CachePkgReader).
         **/
        virtual QStringList fileList();

        /**
         * Add all files belonging to 'path' to this package.
         * Create all directories as needed.
         **/
        void addFile( const QString & path );

	/**
	 * Obtain information about the file or directory specified in
         * 'pathComponents' and create a new FileInfo or a DirInfo (whatever is
         * appropriate) from that information. Use FileInfo::isDirInfo() to
         * find out which.
         *
         * If the underlying syscall fails, this returns 0.
	 **/
	FileInfo * createItem( const QStringList & pathComponents,
                               DirTree	         * tree,
                               DirInfo	         * parent );

        /**
         * Do an lstat() syscall for 'path' or fetch the result from a cache.
         * Return 0 if lstat() fails. Ownership of the returned value is not
         * transferred to the caller, so don't delete it!
         **/
        struct stat * lstat( const QString & path );

        /**
         * Recursively finalize all directories in the subtree.
         **/
        void finalizeAll( DirInfo * subtree );

        /**
         * Clear the stat cache and statistics
         **/
        static void clearStatCache();

        /**
         * Write statistics about the stat cache to the log.
         **/
        static void reportCacheStats();


        // Data members

	PkgInfo * _pkg;

        static QMap<QString, struct stat> _statCache;
        static int                        _activeJobs;
        static int                        _cacheHits;
        static int                        _lstatCalls;

    };	// class PkgReadJob




    /**
     * Read job class for reading information about a package that uses a
     * number of background processes to parallelize all the external commands
     * ("rpm -ql", "dpkg -L", "pacman -Qlp") to speed up getting all the file
     * lists. This is considerably faster than doing that one by one and
     * waiting for the result each time (which is what the more generic
     * PkgReadJob does).
     **/
    class AsyncPkgReadJob: public PkgReadJob
    {
        Q_OBJECT

    public:

	/**
	 * Constructor: Prepare to read the file list of existing PkgInfo node
	 * 'pkg' and create a DirInfo or FileInfo node for each item in the
	 * file list below 'pkg'. Operation starts when the
	 * 'readFileListProcess' has data to read, i.e. when it sends
	 * the 'readFileListFinished' signal.
         *
         * Create the readFileListProcess, then this read job, add the read job
         * to a DirReadJobQueue as a blocked job and then (!) start the
         * readFileListprocess. The job will unblock itself when it receives
         * file list data from the process so it will be put into the queue of
         * jobs that are ready to run.
         *
         * Reading is then started from the outside with startReading() when
         * the job is scheduled.
	 **/
	AsyncPkgReadJob( DirTree * tree,
                         PkgInfo * pkg,
                         Process * readFileListProcess );


	/**
	 * Destructor.
	 **/
	virtual ~AsyncPkgReadJob() {}


    protected slots:

        /**
         * Notification that the attached read file list process is finished.
         **/
        void readFileListFinished( int                  exitCode,
                                   QProcess::ExitStatus exitStatus );


    protected:

        /**
         * Get the file list for this package.
         *
         * Reimplemented from PkgReadJob.
         **/
        virtual QStringList fileList() Q_DECL_OVERRIDE;


        // Data members

        Process *   _readFileListProcess;
        QStringList _fileList;

    };  // class AsyncPkgReadJob




    class CachePkgReadJob: public PkgReadJob
    {
        Q_OBJECT

    public:

	/**
	 * Constructor: Prepare to read the file list of existing PkgInfo node
	 * 'pkg' and create a DirInfo or FileInfo node for each item in the
	 * file list below 'pkg'. This uses 'fileListCache' to get the file
	 * list.
	 *
         * Create this type of job and add it as a normal job (not blocked,
         * unlike AsyncPkgReadJob) to the read queue.
         *
         * Reading is then started from the outside with startReading() when
         * the job queue picks this job.
	 **/
	CachePkgReadJob( DirTree * tree,
                         PkgInfo * pkg,
                         QSharedPointer<PkgFileListCache> fileListCache );

        /**
         * Destructor.
         **/
        virtual ~CachePkgReadJob() {}


    protected:

        /**
         * Get the file list for this package.
         *
         * Reimplemented from PkgReadJob.
         **/
        virtual QStringList fileList() Q_DECL_OVERRIDE;


        // Data members

        QSharedPointer<PkgFileListCache> _fileListCache;

    };  // class CachePkgReadJob

}	// namespace QDirStat

#endif // ifndef PkgReader_h
