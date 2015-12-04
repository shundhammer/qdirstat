/*
 *   File name: kfileinfo.h
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifndef KFileInfo_h
#define KFileInfo_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <sys/types.h>
#include <limits.h>
#include <kdebug.h>
#include <kfileitem.h>

#ifndef NOT_USED
#    define NOT_USED(PARAM)	( (void) (PARAM) )
#endif

// Open a new name space since KDE's name space is pretty much cluttered
// already - all names that would even remotely match are already used up,
// yet the resprective classes don't quite fit the purposes required here.

namespace KDirStat
{
    // With today's hard disks, the 2 GB we could sum up with 'long' (or 4 GB
    // with 'unsigned long') are definitely not enough. So we have to go for
    // something larger:
    typedef long long	KFileSize;

    // Taken from Linux <limits.h> (the Alpha definition - 64 Bit long!).
    // This is how much bytes this program can handle.
#define KFileSizeMax 9223372036854775807LL

    // Forward declarations
    class KDirInfo;
    class KDirTree;


    /**
     * Status of a directory read job.
     **/
    typedef enum
    {
	KDirQueued,		// Waiting in the directory read queue
	KDirReading,		// Reading in progress
	KDirFinished,		// Reading finished and OK
	KDirOnRequestOnly,	// Will be read upon explicit request only (mount points)
	KDirCached,		// Content was read from a cache
	KDirAborted,		// Reading aborted upon user request
	KDirError		// Error while reading
    } KDirReadState;


    
    /**
     * The most basic building block of a @ref KDirTree:
     *
     * Information about one single directory entry. This is the type of info
     * typically obtained by stat() / lstat() or similar calls.	 Most of this
     * can also be obtained by @ref KIO::KDirListJob, but not all: The device
     * this file resides on is something none of KIO's many classes will tell
     * (since of course this only makes sense for local files) - yet this had
     * been _the_ single most requested feature of KDirStat <1.0: Stay on one
     * filesystem. To facilitate this, information about the device is
     * required, thus we'll do lstat() sys calls ourselves for local
     * files. This is what the classes in this file are all about.
     *
     * This class is tuned for size rather than speed: A typical Linux system
     * easily has 150,000+ file system objects, and at least one entry of this
     * sort is required for each of them.
     *
     * This class provides stubs for children management, yet those stubs all
     * are default implementations that don't really deal with children.
     * Derived classes need to take care of that.
     *
     * @short Basic file information (like obtained by the lstat() sys call)
     **/
    class KFileInfo
    {
    public:
	/**
	 * Default constructor.
	 **/
	KFileInfo( KDirTree   * tree,
		   KDirInfo   * parent = 0,
		   const char * name   = 0 );

	/**
	 * Constructor from a stat buffer (i.e. based on an lstat() call).
	 **/
	KFileInfo( const QString &	filenameWithoutPath,
		   struct stat *	statInfo,
		   KDirTree    *	tree,
		   KDirInfo    *	parent = 0 );

	/**
	 * Constructor from a KFileItem, i.e. from a @ref KIO::StatJob
	 **/
	KFileInfo( const KFileItem *	fileItem,
		   KDirTree *		tree,
		   KDirInfo *		parent = 0 );

	/**
	 * Constructor from the bare neccessary fields
	 * for use from a cache file reader
	 *
	 * If 'blocks' is -1, it will be calculated from 'size'.
	 **/
	KFileInfo( KDirTree *		tree,
		   KDirInfo *		parent,
		   const QString &	filenameWithoutPath,
		   mode_t		mode,
		   KFileSize		size,
		   time_t		mtime,
		   KFileSize		blocks = -1,
		   nlink_t		links  = 1 );

	/**
	 * Destructor.
	 *
	 * Don't forget to call @ref KFileInfo::unlinkChild() when deleting
	 * objects of this class!
	 **/
	virtual ~KFileInfo();

	/**
	 * Returns whether or not this is a local file (protocol "file:").
	 * It might as well be a remote file ("ftp:", "smb:" etc.).
	 **/
	bool			isLocalFile()	const { return _isLocalFile; }

	/**
	 * Returns the file or directory name without path, i.e. only the last
	 * path name component (i.e. "printcap" rather than "/etc/printcap").
	 *
	 * If a directory scan doesn't begin at the root directory and this is
	 * the top entry of this directory scan it will also contain the base
	 * path and maybe the protocol (for remote files),
	 * i.e. "/usr/share/man" rather than just "man" if a scan was requested
	 * for "/usr/share/man". Notice, however, that the entry for
	 * "/usr/share/man/man1" will only return "man1" in this example.
	 **/
	QString			name()		const { return _name; }

	/**
	 * Returns the full URL of this object with full path and protocol
	 * (unless the protocol is "file:").
	 *
	 * This is a (somewhat) expensive operation since it will recurse up
	 * to the top of the tree.
	 **/
	QString			url()			const;

	/**
	 * Very much like @ref KFileInfo::url(), but with "/<Files>" appended
	 * if this is a dot entry. Useful for debugging.
	 * Notice: You can simply use the @ref kdbgstream operator<< to
	 * output exactly this:
	 *
	 * kdDebug() << "Found fileInfo " << info << endl;
	 **/
	QString			debugUrl()		const;

	/**
	 * Returns part no. "level" of this object's URL, i.e. traverses up the
	 * tree until this tree level is reached and returns this predecessor's
	 * @ref name() . This is useful for tree searches in symmetrical trees
	 * to find an item's counterpart in the other tree.
	 **/
	QString			urlPart( int level )	const;

	/**
	 * Returns the major and minor device numbers of the device this file
	 * resides on or 0 if this is a remote file.
	 **/
	dev_t			device()	const { return _device; }

	/**
	 * The file permissions and object type as returned by lstat().
	 * You might want to use the repective convenience methods instead:
	 * @ref isDir(), @ref isFile(), ...
	 **/
	mode_t			mode()		const { return _mode;	}

	/**
	 * The number of hard links to this file. Relevant for size summaries
	 * to avoid counting one file several times.
	 **/
	nlink_t			links()		const { return _links;	}

	/**
	 * The file size in bytes. This does not take unused space in the last
	 * disk block (cluster) into account, yet it is the only size all kinds
	 * of info functions can obtain. This is also what most file system
	 * utilities (like "ls -l") display.
	 **/
	KFileSize		byteSize()	const { return _size;	}

	/**
	 * The number of bytes actually allocated on the file system. Usually
	 * this will be more than @ref byteSize() since the last few bytes of a
	 * file usually consume an additional cluster on the file system.
	 *
	 * In the case of sparse files, however, this might as well be
	 * considerably less than @ref byteSize() - this means that this file
	 * has "holes", i.e. large portions filled with zeros. This is typical
	 * for large core dumps for example. The only way to create such a file
	 * is to lseek() far ahead of the previous file size and then writing
	 * data. Most file system utilities will however disregard the fact
	 * that files are sparse files and simply allocate the holes as well,
	 * thus greatly increasing the disk space consumption of such a
	 * file. Only some few file system utilities like "cp", "rsync", "tar"
	 * have options to handle this more graciously - but usually only when
	 * specifically requested. See the respective man pages.
	 **/
	KFileSize		allocatedSize() const;

	/**
	 * The file size, taking into account multiple links for plain files or
	 * the true allocated size for sparse files. For plain files with
	 * multiple links this will be size/no_links, for sparse files it is
	 * the number of bytes actually allocated.
	 **/
	KFileSize		size()		const;

	/**
	 * The file size in 512 byte blocks.
	 **/
	KFileSize		blocks()	const { return _blocks; }

	/**
	 * The size of one single block that @ref blocks() returns.
	 * Notice: This is _not_ the blocksize that lstat() returns!
	 **/
	KFileSize		blockSize()	const { return 512L;	}

	/**
	 * The modification time of the file (not the inode).
	 **/
	time_t			mtime()		const { return _mtime;	}

	/**
	 * Returns the total size in bytes of this subtree.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual KFileSize	totalSize()	{ return size();  }

	/**
	 * Returns the total size in blocks of this subtree.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual KFileSize	totalBlocks()	{ return _blocks; }

	/**
	 * Returns the total number of children in this subtree, excluding this item.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual int		totalItems()	{ return 0;	}

	/**
	 * Returns the total number of subdirectories in this subtree,
	 * excluding this item. Dot entries and "." or ".." are not counted.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual int		totalSubDirs()	{ return 0;	}

	/**
	 * Returns the total number of plain file children in this subtree,
	 * excluding this item.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual int		totalFiles()	{ return 0;	}

	/**
	 * Returns the latest modification time of this subtree.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual time_t		latestMtime()	{ return _mtime;  }

	/**
	 * Returns 'true' if this had been excluded while reading.
	 * Derived classes may want to overwrite this.
	 **/
	virtual bool		isExcluded() const { return false; }

	/**
	 * Set the 'excluded' status. 
	 *
	 * This default implementation silently ignores the value passed and
	 * does nothing. Derived classes may want to overwrite this.
	 **/
	virtual void		setExcluded( bool excl )
	    { ((void) excl); return; }

	/**
	 * Returns whether or not this is a mount point.
	 * Derived classes may want to overwrite this.
	 **/
	virtual bool		isMountPoint()	{ return false; }

	/**
	 * Sets the mount point state, i.e. whether or not this is a mount
	 * point.
	 *
	 * This default implementation silently ignores the value passed and
	 * does nothing. Derived classes may want to overwrite this.
	 **/
	virtual void		setMountPoint( bool isMountPoint = true )
	    { ((void) isMountPoint); return; }

	/**
	 * Returns true if this subtree is finished reading.
	 *
	 * This default implementation always returns 'true';
	 * derived classes should overwrite this.
	 **/
	virtual bool		isFinished()	{ return true; }

	/**
	 * Returns true if this subtree is busy, i.e. it is not finished
	 * reading yet.
	 *
	 * This default implementation always returns 'false';
	 * derived classes should overwrite this.
	 **/
	virtual bool		isBusy()	{ return false; }

	/**
	 * Returns the number of pending read jobs in this subtree. When this
	 * number reaches zero, the entire subtree is done.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual int		pendingReadJobs()	{ return 0;  }


	//
	// Tree management
	//

	/**
	 * Returns a pointer to the @ref KDirTree this entry belongs to.
	 **/
	KDirTree *	tree()			const { return _tree; }

	/**
	 * Returns a pointer to this entry's parent entry or 0 if there is
	 * none.
	 **/
	KDirInfo *	parent()		const { return _parent; }

	/**
	 * Set the "parent" pointer.
	 **/
	void		setParent( KDirInfo *newParent ) { _parent = newParent; }

	/**
	 * Returns a pointer to the next entry on the same level
	 * or 0 if there is none.
	 **/
	KFileInfo *	next()			const { return _next;	}

	/**
	 * Set the "next" pointer.
	 **/
	void		setNext( KFileInfo *newNext ) { _next = newNext; }

	/**
	 * Returns the first child of this item or 0 if there is none.
	 * Use the child's next() method to get the next child.
	 *
	 * This default implementation always returns 0.
	 **/
	virtual KFileInfo * firstChild()	const { return 0;	}

	/**
	 * Set this entry's first child.
	 * Use this method only if you know exactly what you are doing.
	 *
	 * This default implementation does nothing.
	 * Derived classes might want to overwrite this.
	 **/
	virtual void	setFirstChild( KFileInfo *newFirstChild )
	    { NOT_USED( newFirstChild ); }

	/**
	 * Returns true if this entry has any children.
	 **/
	virtual bool	hasChildren()		const;

	/**
	 * Returns true if this entry is in subtree 'subtree', i.e. if this is
	 * a child or grandchild etc. of 'subtree'.
	 **/
	bool isInSubtree( const KFileInfo *subtree ) const;

	/**
	 * Locate a child somewhere in this subtree whose URL (i.e. complete
	 * path) matches the URL passed. Returns 0 if there is no such child.
	 *
	 * Notice: This is a very expensive operation since the entire subtree
	 * is searched recursively.
	 *
	 * Derived classes might or might not wish to overwrite this method;
	 * it's only advisable to do so if a derived class comes up with a
	 * different method than brute-force search all children.
	 *
	 * 'findDotEntries' specifies if locating "dot entries" (".../<Files>")
	 * is desired.
	 **/
	virtual KFileInfo * locate( QString url, bool findDotEntries = false );

	/**
	 * Insert a child into the children list.
	 *
	 * The order of children in this list is absolutely undefined;
	 * don't rely on any implementation-specific order.
	 *
	 * This default implementation does nothing.
	 **/
	virtual void	insertChild( KFileInfo *newChild ) { NOT_USED( newChild ); }

	/**
	 * Return the "Dot Entry" for this node if there is one (or 0
	 * otherwise): This is a pseudo entry that directory nodes use to store
	 * non-directory children separately from directories. This way the end
	 * user can easily tell which summary fields belong to the directory
	 * itself and which are the accumulated values of the entire subtree.
	 *
	 * This default implementation always returns 0.
	 **/
	virtual KFileInfo *dotEntry()	const { return 0; }

	/**
	 * Set a "Dot Entry". This makes sense for directories only.
	 *
	 * This default implementation does nothing.
	 **/
	virtual void	setDotEntry( KFileInfo *newDotEntry ) { NOT_USED( newDotEntry ); }

	/**
	 * Returns true if this is a "Dot Entry".
	 * See @ref dotEntry() for details.
	 *
	 * This default implementation always returns false.
	 **/
	virtual bool	isDotEntry() const { return false; }

	/**
	 * Returns the tree level (depth) of this item.
	 * The topmost level is 0.
	 *
	 * This is a (somewhat) expensive operation since it will recurse up
	 * to the top of the tree.
	 **/
	int		treeLevel() const;

	/**
	 * Notification that a child has been added somewhere in the subtree.
	 *
	 * This default implementation does nothing.
	 **/
	virtual void	childAdded( KFileInfo *newChild ) { NOT_USED( newChild ); }

	/**
	 * Remove a child from the children list.
	 *
	 * IMPORTANT: This MUST be called just prior to deleting an object of
	 * this class. Regrettably, this cannot simply be moved to the
	 * destructor: Important parts of the object might already be destroyed
	 * (e.g., the virtual table - no more virtual methods).
	 *
	 * This default implementation does nothing.
	 * Derived classes that can handle children should overwrite this.
	 **/
	virtual void	unlinkChild( KFileInfo *deletedChild ) { NOT_USED( deletedChild ); }

	/**
	 * Notification that a child is about to be deleted somewhere in the
	 * subtree.
	 **/
	virtual void	deletingChild( KFileInfo *deletedChild ) { NOT_USED( deletedChild ); }

	/**
	 * Get the current state of the directory reading process:
	 *
	 * This default implementation always returns KDirFinished.
	 * Derived classes should overwrite this.
	 **/
	virtual KDirReadState readState() const { return KDirFinished; }

	/**
	 * Returns true if this is a @ref KDirInfo object.
	 *
	 * Don't confuse this with @ref isDir() which tells whether or not this
	 * is a disk directory! Both should return the same, but you'll never
	 * know - better be safe than sorry!
	 *
	 * This default implementation always returns 'false'. Derived classes
	 * (in particular, those derived from @ref KDirInfo) should overwrite this.
	 **/
	virtual bool isDirInfo() const { return false; }

	/**
	 * Returns true if this is a sparse file, i.e. if this file has
	 * actually fewer disk blocks allocated than its byte size would call
	 * for.
	 *
	 * This is a cheap operation since it relies on a cached flag that is
	 * calculated in the constructor rather than doing repeated
	 * calculations and comparisons.
	 *
	 * Please not that @ref size() already takes this into account.
	 **/
	bool isSparseFile() const { return _isSparseFile; }


	//
	// File type / mode convenience methods.
	// These are simply shortcuts to the respective macros from
	// <sys/stat.h>.
	//

	/**
	 * Returns true if this is a directory.
	 **/
	bool isDir()		const { return S_ISDIR( _mode ) ? true : false; }

	/**
	 * Returns true if this is a regular file.
	 **/
	bool isFile()		const { return S_ISREG( _mode ) ? true : false; }

	/**
	 * Returns true if this is a symbolic link.
	 **/
	bool isSymLink()	const { return S_ISLNK( _mode ) ? true : false; }


	/**
	 * Returns true if this is a (block or character) device.
	 **/
	bool isDevice()		const { return ( S_ISBLK ( _mode ) ||
						 S_ISCHR ( _mode )   ) ? true : false; }

	/**
	 * Returns true if this is a block device.
	 **/
	bool isBlockDevice()	const { return S_ISBLK ( _mode ) ? true : false; }

	/**
	 * Returns true if this is a block device.
	 **/
	bool isCharDevice()	const { return S_ISCHR ( _mode ) ? true : false; }

	/**
	 * Returns true if this is a FIFO.
	 **/
	bool isFifo()		const { return S_ISFIFO ( _mode ) ? true : false; }

	/**
	 * Returns true if this is a socket.
	 **/
	bool isSocket()		const { return S_ISSOCK ( _mode ) ? true : false; }

	/**
	 * Returns true if this is a "special" file, i.e. a (block or character)
	 * device, a FIFO (named pipe) or a socket.
	 **/
	bool isSpecial()	const { return ( S_ISBLK ( _mode ) ||
						 S_ISCHR ( _mode ) ||
						 S_ISFIFO( _mode ) ||
						 S_ISSOCK( _mode )   ) ? true : false; }

    protected:

	// Data members.
	//
	// Keep this short in order to use as little memory as possible -
	// there will be a _lot_ of entries of this kind!

	QString		_name;			// the file name (without path!)
	bool		_isLocalFile  :1;	// flag: local or remote file?
	bool		_isSparseFile :1;	// (cache) flag: sparse file (file with "holes")?
	dev_t		_device;		// device this object resides on
	mode_t		_mode;			// file permissions + object type
	nlink_t		_links;			// number of links
	KFileSize	_size;			// size in bytes
	KFileSize	_blocks;		// 512 bytes blocks
	time_t		_mtime;			// modification time

	KDirInfo *	_parent;		// pointer to the parent entry
	KFileInfo *	_next;			// pointer to the next entry
	KDirTree  *	_tree;			// pointer to the parent tree
	
    };	// class KFileInfo



    //----------------------------------------------------------------------
    //			       Static Functions
    //----------------------------------------------------------------------

    /**
     * Make a valid, fixed and cleaned URL from a (possibly dirty) URL or maybe
     * a path.
     **/
    KURL fixedUrl( const QString & dirtyUrl );


    /**
     * Format a file / subtree size human readable, i.e. in "GB" / "MB"
     * etc. rather than huge numbers of digits.
     *
     * Note: For kdDebug() etc., operator<< is overwritten to do exactly that:
     *
     *	   kdDebug() << "Size: " << x->totalSize() << endl;
     **/
    QString formatSize ( KFileSize lSize );


    /**
     * Print the debugUrl() of a @ref KFileInfo in a debug stream.
     **/
    inline kdbgstream & operator<< ( kdbgstream & stream, const KFileInfo * info )
    {
	if ( info )
	    stream << info->debugUrl();
	else
	    stream << "<NULL>";

	return stream;
    }


    /**
     * Human-readable output of a file size in a debug stream.
     **/
    inline kdbgstream & operator<< ( kdbgstream & stream, KFileSize lSize )
    {
	stream << formatSize( lSize );

	return stream;
    }

}	// namespace KDirStat


#endif // ifndef KFileInfo_h


// EOF
