/*
 *   File name: DirInfo.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirInfo_h
#define DirInfo_h


#include "Logger.h"
#include "FileInfo.h"
#include "DataColumns.h"


namespace QDirStat
{
    // Forward declarations
    class DirTree;

    /**
     * A more specialized version of @ref FileInfo: This class can actually
     * manage children. The base class (@ref FileInfo) has only stubs for the
     * respective methods to integrate seamlessly with the abstraction of a
     * file / directory tree; this class fills those stubs with life.
     *
     * @short directory item within a @ref DirTree.
     **/
    class DirInfo: public FileInfo
    {
    public:
	/**
	 * Default constructor.
	 *
	 * If "asDotEntry" is set, this will be used as the parent's
	 * "dot entry", i.e. the pseudo directory that holds all the parent's
	 * non-directory children. This is the only way to create a "dot
	 * entry"!
	 **/
	DirInfo( DirTree * tree,
		 DirInfo * parent     = 0,
		 bool	   asDotEntry = false );

	/**
	 * Constructor from a stat buffer (i.e. based on an lstat() call).
	 **/
	DirInfo( const QString & filenameWithoutPath,
		 struct stat   * statInfo,
		 DirTree       * tree,
		 DirInfo       * parent = 0 );

	/**
	 * Constructor from the bare neccessary fields
	 * for use from a cache file reader
	 **/
	DirInfo( DirTree *	 tree,
		 DirInfo *	 parent,
		 const QString & filenameWithoutPath,
		 mode_t		 mode,
		 FileSize	 size,
		 time_t		 mtime );

	/**
	 * Destructor.
	 **/
	virtual ~DirInfo();

	/**
	 * Returns the total size in bytes of this subtree.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual FileSize totalSize() Q_DECL_OVERRIDE;

	/**
	 * Returns the total size in blocks of this subtree.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual FileSize totalBlocks() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of children in this subtree, excluding this
	 * item.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual int totalItems() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of subdirectories in this subtree,
	 * excluding this item. Dot entries and "." or ".." are not counted.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual int totalSubDirs() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of plain file children in this subtree,
	 * excluding this item.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual int totalFiles() Q_DECL_OVERRIDE;

	/**
	 * Returns the latest modification time of this subtree.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual time_t latestMtime() Q_DECL_OVERRIDE;

	/**
	 * Returns 'true' if this had been excluded while reading.
	 **/
	virtual bool isExcluded() const Q_DECL_OVERRIDE
	    { return _isExcluded; }

	/**
	 * Set the 'excluded' status.
	 **/
	virtual void setExcluded( bool excl =true ) Q_DECL_OVERRIDE
	    { _isExcluded = excl; }

	/**
	 * Returns whether or not this is a mount point.
	 *
	 * This will return 'false' only if this information can be obtained at
	 * all, i.e. if local directory reading methods are used.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual bool isMountPoint() const  Q_DECL_OVERRIDE
	    { return _isMountPoint; }

	/**
	 * Sets the mount point state, i.e. whether or not this is a mount
	 * point.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual void setMountPoint( bool isMountPoint = true ) Q_DECL_OVERRIDE;

        /**
         * Find the nearest parent that is a mount point or 0 if there is
         * none. This may return this DirInfo itself.
         **/
        const DirInfo * findNearestMountPoint() const;

	/**
	 * Returns true if this subtree is finished reading.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual bool isFinished() Q_DECL_OVERRIDE;

	/**
	 * Returns true if this subtree is busy, i.e. it is not finished
	 * reading yet.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual bool isBusy() Q_DECL_OVERRIDE;

	/**
	 * Returns the number of pending read jobs in this subtree. When this
	 * number reaches zero, the entire subtree is done.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual int pendingReadJobs() Q_DECL_OVERRIDE
	    { return _pendingReadJobs;	}

	/**
	 * Returns the first child of this item or 0 if there is none.
	 * Use the child's next() method to get the next child.
	 **/
	virtual FileInfo * firstChild() const Q_DECL_OVERRIDE
	    { return _firstChild;	}

	/**
	 * Set this entry's first child.
	 * Use this method only if you know exactly what you are doing.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual void setFirstChild( FileInfo *newfirstChild ) Q_DECL_OVERRIDE
	    { _firstChild = newfirstChild; }

	/**
	 * Insert a child into the children list.
	 *
	 * The order of children in this list is absolutely undefined;
	 * don't rely on any implementation-specific order.
	 **/
	virtual void insertChild( FileInfo *newChild ) Q_DECL_OVERRIDE;

	/**
	 * Get the "Dot Entry" for this node if there is one (or 0 otherwise):
	 * This is a pseudo entry that directory nodes use to store
	 * non-directory children separately from directories. This way the end
	 * user can easily tell which summary fields belong to the directory
	 * itself and which are the accumulated values of the entire subtree.
	 **/
	virtual DirInfo * dotEntry() const Q_DECL_OVERRIDE
	    { return _dotEntry; }

	/**
	 * Set a "Dot Entry". This makes sense for directories only.
	 **/
	virtual void setDotEntry( FileInfo *newDotEntry ) Q_DECL_OVERRIDE;

	/**
	 * Returns true if this is a "Dot Entry". See @ref dotEntry() for
	 * details.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual bool isDotEntry() const Q_DECL_OVERRIDE
	    { return _isDotEntry; }

	/**
	 * Notification that a child has been added somewhere in the subtree.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual void childAdded( FileInfo *newChild ) Q_DECL_OVERRIDE;

	/**
	 * Remove a child from the children list.
	 *
	 * IMPORTANT: This MUST be called just prior to deleting an object of
	 * this class. Regrettably, this cannot simply be moved to the
	 * destructor: Important parts of the object might already be destroyed
	 * (e.g., the virtual table - no more virtual methods).
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual void unlinkChild( FileInfo *deletedChild ) Q_DECL_OVERRIDE;

	/**
	 * Notification that a child is about to be deleted somewhere in the
	 * subtree.
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual void deletingChild( FileInfo *deletedChild ) Q_DECL_OVERRIDE;

	/**
	 * Notification of a new directory read job somewhere in the subtree.
	 **/
	void readJobAdded();

	/**
	 * Notification of a finished directory read job somewhere in the
	 * subtree.
	 **/
	void readJobFinished();

	/**
	 * Notification of an aborted directory read job somewhere in the
	 * subtree.
	 **/
	void readJobAborted();

	/**
	 * Finalize this directory level after reading it is completed.
	 * This does _not_ mean reading reading all subdirectories is completed
	 * as well!
	 *
	 * Clean up unneeded dot entries.
	 **/
	virtual void finalizeLocal();

	/**
	 * Recursively finalize all directories from here on -
	 * call finalizeLocal() recursively.
	 **/
	void finalizeAll();

	/**
	 * Get the current state of the directory reading process:
	 *
	 *    DirQueued	   waiting in the directory read queue
	 *    DirReading   reading in progress
	 *    DirFinished  reading finished and OK
	 *    DirAborted   reading aborted upon user request
	 *    DirError	   error while reading
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual DirReadState readState() const Q_DECL_OVERRIDE;

	/**
	 * Set the state of the directory reading process.
	 * See @ref readState() for details.
	 **/
	void setReadState( DirReadState newReadState );

	/**
	 * Return a list of (direct) children sorted by 'sortCol' and
	 * 'sortOrder' (Qt::AscendingOrder or Qt::DescendingOrder).
	 *
	 * This might return cached information if the sort column and order
	 * are the same as for the last call to this function, and there were
	 * no children added or removed in the meantime.
	 **/
	const FileInfoList & sortedChildren( DataColumn	   sortCol,
					     Qt::SortOrder sortOrder );

	/**
	 * Drop all cached information about children sorting.
	 **/
	void dropSortCache( bool recursive = false );

	/**
	 * Check if this directory is locked. This is purely a user lock
	 * that can be used by the application. The DirInfo does not care
	 * about it at all.
	 **/
	bool isLocked() const { return _locked; }

	/**
	 * Set the user lock.
	 **/
	void lock() { _locked = true; }

	/**
	 * Unlock the user lock.
	 **/
	void unlock() { _locked = false; }

	/**
	 * Recursively delete all children, including the dot entry.
	 **/
	void clear();

	/**
	 * Reset to the same status like just after construction in preparation
	 * of refreshing the tree from this point on:
	 *
	 * Delete all children if there are any, delete the dot entry's
	 * children if there are any, restore the dot entry if it was removed
	 * (e.g. in finalizeLocal()), set the read state to DirQueued.
	 **/
	void reset();

	/**
	 * Mark this directory as 'touched'. Item models can use this to keep
	 * track of which items were ever used by a connected view to minimize
	 * any update events: If no information about an item was ever
	 * requested by the view, it is not necessary to tell it that that that
	 * data is now outdated.
	 **/
	void touch() { _touched = true; }

	/**
	 * Check the 'touched' flag.
	 **/
	bool isTouched() const { return _touched; }

	/**
	 * Recursively clear the 'touched' flag.
	 **/
	void clearTouched( bool recursive = false );

        /**
         * Return the custom sum that other parts of the application can use to
         * do their own calculations. The treemap view uses this for the sums
         * of the logarithms of files. Notice that it is entirely up to the
         * application to calculate this value whenever appropriate, and also
         * to traverse the tree while doing the calculations. This is just a
         * convenience to allow caching expensive values within the tree.
         **/
        double customSum() const { return _customSum; }

        /**
         * Set the custom sum.
         **/
        void setCustomSum( double newVal ) { _customSum = newVal; }

	/**
	 * Returns true if this is a @ref DirInfo object.
	 *
	 * Don't confuse this with @ref isDir() which tells whether or not this
	 * is a disk directory! Both should return the same, but you'll never
	 * know - better be safe than sorry!
	 *
	 * Reimplemented - inherited from @ref FileInfo.
	 **/
	virtual bool isDirInfo() const Q_DECL_OVERRIDE
	    { return true; }


    protected:

	/**
	 * Recursively recalculate the summary fields when they are dirty.
	 *
	 * This is a _very_ expensive operation since the entire subtree may
	 * recursively be traversed.
	 **/
	void recalc();

	/**
	 * Clean up unneeded / undesired dot entries:
	 * Delete dot entries that don't have any children,
	 * reparent dot entry children to the "real" (parent) directory if
	 * there are not subdirectory siblings at the level of the dot entry.
	 **/
	void cleanupDotEntries();


	//
	// Data members
	//

	bool		_isDotEntry:1;		// Flag: is this entry a "dot entry"?
	bool		_isMountPoint:1;	// Flag: is this a mount point?
	bool		_isExcluded:1;		// Flag: was this directory excluded?
	bool		_summaryDirty:1;	// dirty flag for the cached values
	bool		_deletingAll:1;		// Deleting complete children tree?
	bool		_locked:1;		// App lock
	bool		_touched:1;		// App 'touch' flag
	int		_pendingReadJobs;	// number of open directories in this subtree

	// Children management

	FileInfo *	_firstChild;		// pointer to the first child
	DirInfo	 *	_dotEntry;		// pseudo entry to hold non-dir children

	// Some cached values

	FileSize	_totalSize;
	FileSize	_totalBlocks;
	int		_totalItems;
	int		_totalSubDirs;
	int		_totalFiles;
	time_t		_latestMtime;
        double          _customSum;             // App custom sum

	FileInfoList *	_sortedChildren;
	DataColumn	_lastSortCol;
	Qt::SortOrder	_lastSortOrder;

	DirReadState	_readState;


    private:

	void init();

    };	// class DirInfo

}	// namespace QDirStat


#endif // ifndef DirInfo_h

