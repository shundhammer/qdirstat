/*
 *   File name: DirInfo.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirInfo_h
#define DirInfo_h


#include "FileInfo.h"
#include "DataColumns.h"


namespace QDirStat
{
    // Forward declarations
    class DirTree;
    class DotEntry;

    /**
     * A more specialized version of FileInfo: This class can actually manage
     * children. The base class (FileInfo) has only stubs for the respective
     * methods to integrate seamlessly with the abstraction of a file /
     * directory tree; this class fills those stubs with life.
     *
     * @short directory item within a DirTree.
     **/
    class DirInfo: public FileInfo
    {
    public:

	/**
	 * Constructor from a stat buffer (i.e. based on an lstat() call).
	 **/
	DirInfo( const QString & filenameWithoutPath,
		 struct stat   * statInfo,
		 DirTree       * tree,
		 DirInfo       * parent = 0 );

	/**
	 * Constructor from the bare necessary fields for use from a cache file
	 * reader.
	 **/
	DirInfo( DirTree *	 tree,
		 DirInfo *	 parent,
		 const QString & filenameWithoutPath,
		 mode_t		 mode,
		 FileSize	 size,
		 time_t		 mtime );

	/**
	 * Default constructor.
	 *
	 * This is the only constructor that will not create a dot entry
	 * immediately. If that is desired, you can always use ensureDotEntry()
	 * later.
	 **/
	DirInfo( DirTree * tree,
		 DirInfo * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~DirInfo();

	/**
	 * Returns the total size in bytes of this subtree.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual FileSize totalSize() Q_DECL_OVERRIDE;

	/**
	 * Returns the total allocated size in bytes of this subtree.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual FileSize totalAllocatedSize() Q_DECL_OVERRIDE;

        /**
         * The ratio of totalSize() / totalAllocatedSize() in percent.
         **/
        int totalUsedPercent();

	/**
	 * Returns the total size in blocks of this subtree.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual FileSize totalBlocks() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of children in this subtree, excluding this
	 * item.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual int totalItems() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of subdirectories in this subtree,
	 * excluding this item. Dot entries and "." or ".." are not counted.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual int totalSubDirs() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of plain file children in this subtree,
	 * excluding this item.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual int totalFiles() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of non-directory items in this subtree,
	 * excluding this item.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual int totalNonDirItems() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of ignored (non-directory!) items in this
	 * subtree, excluding this item.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual int totalIgnoredItems() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of not ignored (non-directory!) items in
	 * this subtree, excluding this item.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual int totalUnignoredItems() Q_DECL_OVERRIDE;

	/**
	 * Returns the total number of direct children of this directory.
	 *
	 * If this directory has a dot entry, the dot entry itself is counted,
	 * but not the file children of the dot entry.
	 *
	 * This method uses a cached value whenever possible, so it is
	 * considerably faster than the unconditional countDirectChildren()
	 * method.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual int directChildrenCount() Q_DECL_OVERRIDE;

	/**
	 * Returns the number of subdirectories below this item that could not
	 * be read (typically due to insufficient permissions).
	 *
	 * Notice that this does NOT include this item if it is a directory
	 * that could not be read.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual int errSubDirCount() Q_DECL_OVERRIDE;

	/**
	 * Returns the latest modification time of this subtree.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual time_t latestMtime() Q_DECL_OVERRIDE;

	/**
	 * Returns the oldest modification time of any file in this subtree.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual time_t oldestFileMtime() Q_DECL_OVERRIDE;

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
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual bool isMountPoint() const  Q_DECL_OVERRIDE
	    { return _isMountPoint; }

	/**
	 * Sets the mount point state, i.e. whether or not this is a mount
	 * point.
	 *
	 * Reimplemented - inherited from FileInfo.
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
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual bool isFinished() Q_DECL_OVERRIDE;

	/**
	 * Returns true if this subtree is busy, i.e. it is not finished
	 * reading yet.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual bool isBusy() Q_DECL_OVERRIDE;

	/**
	 * Returns the number of pending read jobs in this subtree. When this
	 * number reaches zero, the entire subtree is done.
	 *
	 * Reimplemented - inherited from FileInfo.
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
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual void setFirstChild( FileInfo * newfirstChild ) Q_DECL_OVERRIDE
	    { _firstChild = newfirstChild; }

	/**
	 * Insert a child into the children list.
	 *
	 * The order of children in this list is absolutely undefined;
	 * don't rely on any implementation-specific order.
	 **/
	virtual void insertChild( FileInfo * newChild ) Q_DECL_OVERRIDE;

	/**
	 * Add a child to the attic. This is very much like insertChild(), but
	 * it inserts the child into the appropriate attic instead (and sets
	 * its 'ignored' flag: The dot entry's attic if there is a dot entry
	 * and the new child is not a directory, and the directory's attic
	 * otherwise.
	 **/
	virtual void addToAttic( FileInfo * newChild );

	/**
	 * Move a child to the attic, i.e. remove it from the normal children
	 * and move it to the attic instead.
	 **/
	virtual void moveToAttic( FileInfo * newChild );

	/**
	 * Get the "Dot Entry" for this node if there is one (or 0 otherwise):
	 * This is a pseudo entry that directory nodes use to store
	 * non-directory children separately from directories. This way the end
	 * user can easily tell which summary fields belong to the directory
	 * itself and which are the accumulated values of the entire subtree.
	 **/
	virtual DotEntry * dotEntry() const Q_DECL_OVERRIDE
	    { return _dotEntry; }

	/**
	 * Return the dot entry for this node. If it doesn't have one yet,
	 * create it first.
	 **/
	virtual DotEntry * ensureDotEntry();

	/**
	 * Delete the dot entry if it is empty, i.e. it does not have any
	 * children or its attic (if it has one) is also empty. The dot entry's
	 * attic is implicitly deleted along with it.
	 **/
	virtual void deleteEmptyDotEntry();

	/**
	 * Return the "Attic" entry for this node if there is one (or 0
	 * otherwise): This is a pseudo entry that directory nodes use to store
	 * ignored files and directories separately from the normal tree
	 * hierarchy.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual Attic * attic() const Q_DECL_OVERRIDE
	    { return _attic; }

	/**
	 * Return the attic for this node. If it doesn't have one yet, create
	 * it first.
	 **/
	virtual Attic * ensureAttic();

	/**
	 * Delete the attic if it is empty.
	 **/
	virtual void deleteEmptyAttic();

	/**
	 * Return 'true' if there is an attic and it has any children.
	 **/
	bool hasAtticChildren() const;

	/**
	 * Notification that a child has been added somewhere in the subtree.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual void childAdded( FileInfo * newChild ) Q_DECL_OVERRIDE;

	/**
	 * Remove a child from the children list.
	 *
	 * IMPORTANT: This MUST be called just prior to deleting an object of
	 * this class. Regrettably, this cannot simply be moved to the
	 * destructor: Important parts of the object might already be destroyed
	 * (e.g., the virtual table - no more virtual methods).
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual void unlinkChild( FileInfo * deletedChild ) Q_DECL_OVERRIDE;

	/**
	 * Notification that a child is about to be deleted somewhere in the
	 * subtree.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual void deletingChild( FileInfo * deletedChild ) Q_DECL_OVERRIDE;

	/**
	 * Notification of a new directory read job somewhere in the subtree.
	 **/
	void readJobAdded();

	/**
	 * Notification of a finished directory read job for 'dir'.
	 * This is cascaded upward in the tree.
	 **/
	void readJobFinished( DirInfo * dir );

	/**
	 * Notification of an aborted directory read job for 'dir'.
	 **/
	void readJobAborted( DirInfo * dir );

	/**
	 * Finalize this directory level after reading it is completed. This
	 * does _not_ mean that reading all subdirectories is completed as
	 * well!
	 *
	 * Clean up unneeded dot entries.
	 **/
	virtual void finalizeLocal();

	/**
	 * Recursively finalize all directories from here on -
	 * call finalizeLocal() recursively.
	 **/
	virtual void finalizeAll();

	/**
	 * Get the current state of the directory reading process:
	 *
	 *    DirQueued		    waiting in the directory read queue
	 *    DirReading	    reading in progress
	 *    DirFinished	    reading finished and OK
	 *    DirAborted	    reading aborted upon user request
	 *    DirError		    error while reading
	 *    DirPermissionDenied   insufficient permissions
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual DirReadState readState() const Q_DECL_OVERRIDE;

	/**
	 * Check if readState() is anything that indicates an error reading the
	 * directory. This returns 'true' for DirError or DirPermissionDenied,
	 * 'false' otherwise.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual bool readError() const Q_DECL_OVERRIDE;

	/**
	 * Return a prefix for the total size (and similar accumulated fields)
	 * of this item: ">" if there might be more, i.e. if a subdirectory
	 * could not be read or if reading was aborted, an empty string
	 * otherwise.
	 *
	 * Notice that this implementation also returns an empty string as long
	 * as this subtree is busy, i.e. reading is not finished: The ">"
	 * prefix should be something special to catch the user's attention,
	 * not something omnipresent that is commonly ignored.
	 *
	 * Reimplemented from FileInfo.
	 **/
	virtual QString sizePrefix() const Q_DECL_OVERRIDE;

	/**
	 * Set the state of the directory reading process.
	 * See readState() for details.
	 **/
	void setReadState( DirReadState newReadState );

	/**
	 * Return a list of (direct) children sorted by 'sortCol' and
	 * 'sortOrder' (Qt::AscendingOrder or Qt::DescendingOrder).  If
	 * 'includeAttic' is 'true', the attic (if there is one) is added to
	 * the list.
	 *
	 * This might return cached information if all parameters are the same
	 * as for the last call to this function, and there were no children
	 * added or removed in the meantime.
	 **/
	const FileInfoList & sortedChildren( DataColumn	   sortCol,
					     Qt::SortOrder sortOrder,
					     bool	   includeAttic = false );

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
	virtual void reset();

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
	 * Returns true if this is a DirInfo object.
	 *
	 * Don't confuse this with isDir() which tells whether or not this is a
	 * disk directory! Both should return the same, but you'll never know -
	 * better be safe than sorry!
	 *
	 * Notice that DotEntry inherits DirInfo, so a DotEntry is also
	 * implicitly a DirInfo.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual bool isDirInfo() const Q_DECL_OVERRIDE
	    { return true; }

	/**
	 * Count the direct children unconditionally and update
	 * _directChildrenCount.
	 **/
	int countDirectChildren();

	/**
	 * Check the 'ignored' state of this item and set the '_isIgnored' flag
	 * accordingly.
	 **/
	virtual void checkIgnored();

	/**
	 * Set any empty subdir children to ignored. This affects only direct
	 * children.
	 **/
	virtual void ignoreEmptySubDirs();

	/**
	 * Take all children from 'oldParent' and move them to this DirInfo.
	 **/
	virtual void takeAllChildren( DirInfo * oldParent );

	/**
	 * Recursively recalculate the summary fields when they are dirty.
	 *
	 * This is a _very_ expensive operation since the entire subtree may
	 * recursively be traversed.
	 **/
	void recalc();


    protected:

	/**
	 * Clean up unneeded / undesired dot entries:
	 * Delete dot entries that don't have any children,
	 * reparent dot entry children to the "real" (parent) directory if
	 * there are not subdirectory siblings at the level of the dot entry.
	 **/
	virtual void cleanupDotEntries();

	/**
	 * Clean up unneeded attics: Delete attic entries that don't have any
	 * children.
	 **/
	virtual void cleanupAttics();


	//
	// Data members
	//

	bool		_isMountPoint:1;	// Flag: is this a mount point?
	bool		_isExcluded:1;		// Flag: was this directory excluded?
	bool		_summaryDirty:1;	// dirty flag for the cached values
	bool		_deletingAll:1;		// Deleting complete children tree?
	bool		_locked:1;		// App lock
	bool		_touched:1;		// App 'touch' flag
	int		_pendingReadJobs;	// number of open directories in this subtree

	// Children management

	FileInfo *	_firstChild;		// pointer to the first child
	DotEntry *	_dotEntry;		// pseudo entry to hold non-dir children
	Attic	 *	_attic;			// pseudo entry to hold ignored children

	// Some cached values

	FileSize	_totalSize;
	FileSize	_totalAllocatedSize;
	FileSize	_totalBlocks;
	int		_totalItems;
	int		_totalSubDirs;
	int		_totalFiles;
	int		_totalIgnoredItems;
	int		_totalUnignoredItems;
	int		_directChildrenCount;
	int		_errSubDirCount;
	time_t		_latestMtime;
	time_t		_oldestFileMtime;

	FileInfoList *	_sortedChildren;
	DataColumn	_lastSortCol;
	Qt::SortOrder	_lastSortOrder;
	bool		_lastIncludeAttic;

	DirReadState	_readState;


    private:

	void init();

    };	// class DirInfo

}	// namespace QDirStat


#endif // ifndef DirInfo_h

