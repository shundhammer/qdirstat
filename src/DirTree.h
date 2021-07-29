/*
 *   File name: DirTree.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirTree_h
#define DirTree_h


#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>

#include <QList>

#include "DirReadJob.h"
#include "PkgFilter.h"


namespace QDirStat
{
    class DirInfo;
    class DirReadJob;
    class FileInfoSet;
    class ExcludeRules;
    class DirTreeFilter;


    /**
     * This class provides some infrastructure as well as global data for a
     * directory tree. It acts as the glue that holds things together: The root
     * item from which to descend into the subtrees, the read queue and some
     * global policies (like whether or not to cross filesystems while reading
     * directories).
     *
     * Notice that this class uses a "pseudo root" to better conform with Qt's
     * notion of tree views and the corresponding data models: They use an
     * invisible root item to support multiple toplevel items.
     *
     * @short Directory tree global data and infrastructure
     **/
    class DirTree: public QObject
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 *
	 * Remember to call startReading() after the constructor and
	 * setting up connections.
	 **/
	DirTree();

	/**
	 * Destructor.
	 **/
	virtual ~DirTree();


     public slots:

	/**
	 * Actually start reading.
	 *
	 * It's not very pretty that this is required as a separate method, but
	 * this cannot simply be done in the constructor: We need to give the
	 * caller a chance to set up Qt signal connections, and for this the
	 * constructor must return before any signals are sent, i.e. before
	 * anything is read.
	 **/
	void startReading( const QString & path );

	/**
	 * Forcefully stop a running read process.
	 **/
	void abortReading();

	/**
	 * Refresh a subtree, i.e. read its contents from disk again.
	 *
	 * All children of the old subtree will be deleted and rebuilt from
	 * scratch, i.e. all pointers to elements within this subtree will
	 * become invalid (a subtreeDeleted() signal will be emitted to notify
	 * about that fact).
	 *
	 * When 0 is passed, the entire tree will be refreshed, i.e. from the
	 * first toplevel element on.
	 **/
	void refresh( DirInfo * subtree = 0 );

	/**
	 * Refresh a number of subtrees.
	 **/
	void refresh( const FileInfoSet & refreshSet );

	/**
	 * Delete a subtree.
	 **/
	void deleteSubtree( FileInfo * subtree );

	/**
	 * Delete all children of a subtree, but leave the subtree inself
	 * intact.
	 **/
	void clearSubtree( DirInfo * subtree );

	/**
	 * Finalize the complete tree after all read jobs are done.
	 **/
	void finalizeTree();


    public:

	/**
	 * Return the URL of this tree if it has any elements or an empty
	 * string if it doesn't.
	 **/
	QString url() const;

	/**
	 * Return the root item of this tree. Notice that this is a pseudo root
	 * that does not really correspond to a filesystem object.
	 **/
	DirInfo * root() const { return _root; }

	/**
	 * Sets the root item of this tree.
	 **/
	void setRoot( DirInfo * newRoot );

	/**
	 * Return the first toplevel item of this tree or 0 if there is
	 * none. This is the logical root item.
	 **/
	FileInfo * firstToplevel() const;

	/**
	 * Return 'true' if 'item' is a toplevel item, i.e. a direct child of
	 * the root item.
	 **/
	bool isTopLevel( FileInfo *item ) const;

	/**
	 * Return the device of this tree's root item ("/dev/sda3" etc.).
	 **/
	QString device() const { return _device; }

	/**
	 * Clear all items of this tree.
	 **/
	void clear();

	/**
	 * Clear all items, exclude rules and filters of this tree.
	 **/
	void reset();

	/**
	 * Locate a child somewhere in the tree whose URL (i.e. complete path)
	 * matches the URL passed. Returns 0 if there is no such child.
	 *
	 * Notice: This is a very expensive operation since the entire tree is
	 * searched recursively.
	 *
	 * 'findPseudoDirs' specifies if locating pseudo directories like "dot
	 * entries" (".../<Files>") or "attics" (".../<Ignored>") is desired.
	 **/
	FileInfo * locate( QString url, bool findPseudoDirs = false );

	/**
	 * Add a new directory read job to the queue.
	 **/
	void addJob( DirReadJob * job );

	/**
	 * Add a new directory read job to the list of blocked jobs. A job may
	 * be blocked because it may be waiting for an external process to
	 * finish.
	 **/
	void addBlockedJob( DirReadJob * job );

	/**
	 * Unblock a previously blocked read job so it is scheduled along with
	 * the other pending jobs.
	 **/
	void unblock( DirReadJob * job );

	/**
	 * Should directory scans cross filesystems?
	 *
	 * Notice: This can only be avoided with local directories where the
	 * device number a file resides on can be obtained.
	 * Remember, that's what this QDirStat business is all about.  ;-)
	 **/
	bool crossFilesystems() const { return _crossFilesystems; }

	/**
	 * Set or unset the "cross filesystems" flag.
	 **/
	void setCrossFilesystems( bool doCross )
	    { _crossFilesystems = doCross; }

	/**
	 * Notification that a child has been added.
	 *
	 * Directory read jobs are required to call this for each child added
	 * so the tree can emit the corresponding childAdded() signal.
	 **/
	virtual void childAddedNotify( FileInfo *newChild );

	/**
	 * Notification that a child is about to be deleted.
	 *
	 * Directory read jobs are required to call this for each deleted child
	 * so the tree can emit the corresponding deletingChild() signal.
	 **/
	virtual void deletingChildNotify( FileInfo *deletedChild );

	/**
	 * Notification that one or more children have been deleted.
	 *
	 * Directory read jobs are required to call this when one or more
	 * children are deleted so the tree can emit the corresponding
	 * deletingChild() signal. For multiple deletions (e.g. entire
	 * subtrees) this should only happen once at the end.
	 **/
	virtual void childDeletedNotify();

	/**
	 * Send a startingReading() signal.
	 **/
	void sendStartingReading();

	/**
	 * Send a finished() signal.
	 **/
	void sendFinished();

	/**
	 * Send a aborted() signal.
	 **/
	void sendAborted();

	/**
	 * Send a startingReading( DirInfo * ) signal.
	 **/
	void sendStartingReading( DirInfo * dir );

	/**
	 * Send a readJobFinished( DirInfo * ) signal.
	 **/
	void sendReadJobFinished( DirInfo * dir );

	/**
	 * Returns 'true' if directory reading is in progress in this tree.
	 **/
	bool isBusy() { return _isBusy; }

	/**
	 * Write the complete tree to a cache file.
	 *
	 * Returns true if OK, false upon error.
	 **/
	bool writeCache( const QString & cacheFileName );

	/**
	 * Read a cache file.
	 **/
	void readCache( const QString & cacheFileName );

	/**
	 * Clear the tree and read a cache file.
	 **/
	void clearAndReadCache( const QString & cacheFileName );

	/**
	 * Read installed packages that match the specified PkgFilter and their
	 * file lists from the system's package manager(s).
	 **/
	void readPkg( const PkgFilter & pkgFilter );

	/**
	 * Return exclude rules specific to this tree (as opposed to the global
	 * ones stored in the ExcludeRules singleton) or 0 if there are none.
	 **/
	ExcludeRules * excludeRules() const { return _excludeRules; }

	/**
	 * Set exclude rules specific to this tree. They are additional rules
	 * to the ones in the ExcludeRules singleton. This can be used for
	 * temporary exclude rules that are not to be written to the config
	 * file.
	 *
	 * The DirTree takes over ownership of this object and will delete it
	 * when appropriate (i.e. in its destructor or when new ExcludeRules
	 * are set with this function). Call this with 0 to remove the existing
	 * exclude rules.
	 **/
	void setExcludeRules( ExcludeRules * newRules );

	/**
	 * Clear all temporary exclude rules.
	 **/
	void clearExcludeRules() { setExcludeRules( 0 ); }

	/**
	 * Add a filter to ignore files during directory reading.
	 *
	 * The DirTree takes over ownership of the filter object and will
	 * delete it when appropriate.
	 **/
	void addFilter( DirTreeFilter * filter );

	/**
	 * Clear all filters.
	 **/
	void clearFilters();

	/**
	 * Iterate over all filters and return 'true' if any of them wants a
	 * filesystem object to be ignored during directory reading, 'false'
	 * if not.
	 **/
	bool checkIgnoreFilters( const QString & path );

	/**
	 * Return 'true' if there is any filter, 'false' if not.
	 **/
	bool hasFilters() const { return ! _filters.isEmpty(); }

	/**
	 * Return 'true' if this DirTree is in the process of being destroyed,
	 * so any FileInfo / DirInfo pointers stored outside the tree might
	 * have become invalid.
	 **/
	bool beingDestroyed() const { return _beingDestroyed; }

        /**
         * Return the number of 512-bytes blocks per cluster.
         *
         * This may be 0 if no small file (< 512 bytes) was found in this tree
         * yet.
         **/
        int blocksPerCluster() const { return _blocksPerCluster; }

        /**
         * Return the cluster size of this tree, i.e. the disk space allocation
         * unit. No non-zero file can have an allocated size smaller than this.
         *
         * This may be 0 if no small file (< 512 bytes) was found in this tree
         * yet.
         **/
        FileSize clusterSize() const { return _blocksPerCluster * STD_BLOCK_SIZE; }


    signals:

	/**
	 * Emitted when a child has been added.
	 **/
	void childAdded( FileInfo * newChild );

	/**
	 * Emitted when the tree is about to be cleared.
	 **/
	void clearing();

	/**
	 * Emitted when a child is about to be deleted.
	 **/
	void deletingChild( FileInfo * deletedChild );

	/**
	 * Emitted after a child is deleted. If you are interested which child
	 * it was, better use the deletingChild() signal.
	 *
	 * childDeleted() is only useful to rebuild a view etc. completely.
	 * If possible, this signal is sent only once for multiple deletions -
	 * e.g., when entire subtrees are deleted.
	 **/
	void childDeleted();

	/**
	 * Emitted when a subtree is about to be cleared, i.e. all its children
	 * will be deleted (but not the subtree node itself).
	 **/
	void clearingSubtree( DirInfo * subtree );

	/**
	 * Emitted when clearing a subtree is finished.
	 **/
	void subtreeCleared( DirInfo * subtree );

	/**
	 * Emitted when reading is started.
	 **/
	void startingReading();

	/**
	 * Emitted when reading this directory tree is completely finished.
	 **/
	void finished();

	/**
	 * Emitted when reading this directory tree has been aborted.
	 **/
	void aborted();

	/**
	 * Emitted when reading the specified directory is started.
	 **/
	void startingReading( DirInfo * dir );

	/**
	 * Emitted when reading the specified directory has been finished.
	 * This is sent AFTER finalizeLocal( DirInfo * dir ).
	 **/
	void readJobFinished( DirInfo * dir );

	/**
	 * Single line progress information, emitted when the read status
	 * changes - typically when a new directory is being read. Connect to a
	 * status bar etc. to keep the user entertained.
	 **/
	void progressInfo( const QString & infoLine );


    protected slots:

	/**
	 * Notification that all jobs in the job queue are finished.
	 * This will emit the finished() signal.
	 **/
	void slotFinished();


    protected:

	/**
	 * Recurse through the tree from 'dir' on and move any ignored items to
	 * the attic on the same level.
	 **/

	void moveIgnoredToAttic( DirInfo * dir );

	/**
	 * Recurse through the tree from 'dir' on and ignore any empty dirs
	 * (i.e. dirs without any unignored non-directory child) that are not
	 * ignored yet.
	 **/
	void ignoreEmptyDirs( DirInfo * dir );

	/**
	 * Move all items from the attic to the normal children list.
	 **/
	void unatticAll( DirInfo * dir );

	/**
	 * Recursively force a complete recalculation of all sums.
	 **/
	void recalc( DirInfo * dir );

        /**
         * Try to derive the cluster size from 'item'.
         **/
        void detectClusterSize( FileInfo * item );



	// Data members

	DirInfo *		_root;
	DirReadJobQueue		_jobQueue;
	bool			_crossFilesystems;
	bool			_isBusy;
	QString			_device;
	QString			_url;
	ExcludeRules *		_excludeRules;
	QList<DirTreeFilter *>	_filters;
	bool			_beingDestroyed;
        bool                    _haveClusterSize;
        int                     _blocksPerCluster;

    };	// class DirTree

}	// namespace QDirStat


#endif // ifndef DirTree_h
