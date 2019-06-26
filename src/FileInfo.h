/*
 *   File name: FileInfo.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileInfo_h
#define FileInfo_h


#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include <QTextStream>
#include <QList>

#include "Logger.h"


namespace QDirStat
{
    typedef long long FileSize;

#define FileSizeMax   LLONG_MAX
// 0x7FFFFFFFFFFFFFFFLL == 9223372036854775807LL

#define FileInfoMagic 4242

    // Forward declarations
    class DirInfo;
    class DotEntry;
    class Attic;
    class PkgInfo;
    class DirTree;


    /**
     * Status of a directory read job.
     **/
    enum DirReadState
    {
	DirQueued,		// Waiting in the directory read queue
	DirReading,		// Reading in progress
	DirFinished,		// Reading finished and OK
	DirOnRequestOnly,	// Will be read upon explicit request only (mount points)
	DirCached,		// Content was read from a cache
	DirAborted,		// Reading aborted upon user request
	DirError		// Error while reading
    };


    /**
     * The most basic building block of a DirTree:
     *
     * Information about one single directory entry. This is the type of info
     * typically obtained by stat() / lstat() or similar calls.
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
    class FileInfo
    {
    public:
	/**
	 * Default constructor.
	 **/
	FileInfo( DirTree    * tree,
		  DirInfo    * parent = 0,
		  const char * name   = 0 );

	/**
	 * Constructor from a stat buffer (i.e. based on an lstat() call).
	 **/
	FileInfo( const QString & filenameWithoutPath,
		  struct stat	* statInfo,
		  DirTree	* tree,
		  DirInfo	* parent = 0 );

	/**
	 * Constructor from the bare necessary fields
	 * for use from a cache file reader
	 *
	 * If 'blocks' is -1, it will be calculated from 'size'.
	 **/
	FileInfo( DirTree *	  tree,
		  DirInfo *	  parent,
		  const QString & filenameWithoutPath,
		  mode_t	  mode,
		  FileSize	  size,
		  time_t	  mtime,
		  FileSize	  blocks = -1,
		  nlink_t	  links	 = 1 );

	/**
	 * Destructor.
	 *
	 * Don't forget to call FileInfo::unlinkChild() when deleting
	 * objects of this class!
	 **/
	virtual ~FileInfo();

	/**
	 * Check with the magic number if this object is valid.
	 * Return 'true' if it is valid, 'false' if invalid.
	 *
	 * Notice that this is intentionally not a virtual function to avoid
	 * a segfault via the vptr if it is not valid.
	 **/
	bool checkMagicNumber() const;

	/**
	 * Returns whether or not this is a local file (protocol "file:").
	 * It might as well be a remote file ("ftp:", "smb:" etc.).
	 **/
	bool isLocalFile() const { return _isLocalFile; }

	/**
	 * Returns the file or directory name without path, i.e. only the last
	 * path name component (i.e. "printcap" rather than "/etc/printcap").
	 *
	 * If a directory scan doesn't begin at the root directory and this is
	 * the top entry of this directory scan, it will also contain the base
	 * path, i.e. "/usr/share/man" rather than just "man" if a scan was
	 * requested for "/usr/share/man". Notice, however, that the entry for
	 * "/usr/share/man/man1" will only return "man1" in this example.
	 **/
	QString name() const { return _name; }

        /**
         * Returns the base name of this object, i.e. the last path component,
         * even if this is a toplevel item.
         **/
        QString baseName() const;

	/**
	 * Returns the full URL of this object with full path.
	 *
	 * This is a (somewhat) expensive operation since it will recurse up
	 * to the top of the tree.
	 **/
	virtual QString url() const;

        /**
         * Returns the full path of this object. Unlike url(), this never has a
         * protocol prefix or a part that identifies the package this belongs
         * to. This is the path that can be used to find this object in the
         * filesystem.
         *
         * url()  might return  "Pkg:/chromium-browser/usr/lib/chromium/foo.z"
         * path() returns just  "/usr/lib/chromium/foo.z"
         *
         * Like url(), this is somewhat expensive since it recurses up the
         * tree, but it stops when a PkgInfo node is found there.
         **/
        virtual QString path() const;

	/**
	 * Very much like FileInfo::url(), but with "/<Files>" appended if this
	 * is a dot entry. Useful for debugging.
         *
         * Notice: You can simply use the QTextStream operator<< to output
	 * exactly this:
	 *
	 * logDebug() << "Found fileInfo " << info << endl;
	 **/
	virtual QString debugUrl() const;

	/**
	 * Returns the major and minor device numbers of the device this file
	 * resides on or 0 if this is a remote file.
	 **/
	dev_t device() const { return _device; }

	/**
	 * The file permissions and object type as returned by lstat().
	 * You might want to use the repective convenience methods instead:
	 * isDir(), isFile(), ...
         *
         * See also symbolicPermissions(), octalPermissions()
	 **/
	mode_t mode() const { return _mode;   }

	/**
	 * The number of hard links to this file. Relevant for size summaries
	 * to avoid counting one file several times.
	 **/
	nlink_t links() const { return _links;	}

	/**
	 * User ID of the owner.
	 *
	 * Notice that this might be undefined if this tree branch was read
	 * from a cache file. Check that with hasUid().
	 **/
	uid_t uid() const { return _uid; }

	/**
	 * Return the user name of the owner.
	 *
	 * If this tree branch was read from a cache file, this returns an
	 * empty string.
	 **/
	QString userName() const;

        /**
         * Return 'true' if this FileInfo has a UID (user ID).
         *
         * It might not have that information e.g. if it was read from a cache
         * file.
         **/
        bool hasUid() const;

	/**
	 * Group ID of the owner.
	 *
	 * Notice that this might be undefined if this tree branch was read
	 * from a cache file. Check that with hasGid().
	 **/
	gid_t gid() const { return _gid; }

	/**
	 * Return the group name of the owner.
	 *
	 * If this tree branch was read from a cache file, this returns an
	 * empty string.
	 **/
	QString groupName() const;

        /**
         * Return 'true' if this FileInfo has a GID (group ID).
         *
         * It might not have that information e.g. if it was read from a cache
         * file.
         **/
        bool hasGid() const;

        /**
         * File permissions formatted like in "ls -l", i.e. "-rwxrwxrwx",
         * "drwxrwxrwx"
         **/
        QString symbolicPermissions() const;

        /**
         * File permissions formatted as octal number (like used by the "chmod"
         * command, i.e. "0644"
         **/
        QString octalPermissions() const;

	/**
	 * The file size in bytes. This does not take unused space in the last
	 * disk block (cluster) into account, yet it is the only size all kinds
	 * of info functions can obtain. This is also what most file system
	 * utilities (like "ls -l") display.
	 **/
	FileSize byteSize() const { return _size;   }

	/**
	 * The number of bytes actually allocated on the file system. Usually
	 * this will be more than byteSize() since the last few bytes of a file
	 * usually consume an additional cluster on the file system.
	 *
	 * In the case of sparse files, however, this might as well be
	 * considerably less than byteSize() - this means that this file has
	 * "holes", i.e. large portions filled with zeros. This is typical for
	 * large core dumps for example. The only way to create such a file is
	 * to lseek() far ahead of the previous file size and then writing
	 * data. Most file system utilities will however disregard the fact
	 * that files are sparse files and simply allocate the holes as well,
	 * thus greatly increasing the disk space consumption of such a
	 * file. Only some few file system utilities like "cp", "rsync", "tar"
	 * have options to handle this more graciously - but usually only when
	 * specifically requested. See the respective man pages.
	 **/
	FileSize allocatedSize() const;

	/**
	 * The file size, taking into account multiple links for plain files or
	 * the true allocated size for sparse files. For plain files with
	 * multiple links this will be size/no_links, for sparse files it is
	 * the number of bytes actually allocated.
	 **/
	FileSize size() const;

	/**
	 * The file size in 512 byte blocks.
	 **/
	FileSize blocks() const { return _blocks; }

	/**
	 * The size of one single block that blocks() returns.
	 * Notice: This is _not_ the blocksize that lstat() returns!
	 **/
	FileSize blockSize() const { return 512L;    }

	/**
	 * The modification time of the file (not the inode).
	 **/
	time_t mtime() const { return _mtime; }

	/**
	 * Returns the total size in bytes of this subtree.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual FileSize totalSize() { return size(); }

	/**
	 * Returns the total size in blocks of this subtree.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual FileSize totalBlocks() { return _blocks; }

	/**
	 * Returns the total number of children in this subtree, excluding this
	 * item.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual int totalItems() { return 0; }

	/**
	 * Returns the total number of subdirectories in this subtree,
	 * excluding this item. Dot entries and "." or ".." are not counted.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual int totalSubDirs() { return 0; }

	/**
	 * Returns the total number of plain file children in this subtree,
	 * excluding this item.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual int totalFiles() { return 0; }

	/**
	 * Returns the total number of direct children of this item.
         *
	 * Derived classes that have children should overwrite this.
	 **/
	virtual int directChildrenCount() { return 0; }

	/**
	 * Returns the latest modification time of this subtree.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual time_t latestMtime() { return _mtime; }

	/**
	 * Return the percentage of this subtree in regard to its parent
	 * (0.0..100.0). Return a negative value if for any reason this cannot
	 * be calculated or it would not make any sense.
	 *
	 * Derived classes are free to overwrite this, but this default
	 * implementation should work well enough.
	 **/
	virtual float subtreePercent();

	/**
	 * Returns 'true' if this had been excluded while reading.
	 * Derived classes may want to overwrite this.
	 **/
	virtual bool isExcluded() const { return false; }

	/**
	 * Set the 'excluded' status.
	 *
	 * This default implementation silently ignores the value passed and
	 * does nothing. Derived classes may want to overwrite this.
	 **/
	virtual void setExcluded( bool excl ) { Q_UNUSED( excl); return; }

	/**
	 * Returns whether or not this is a mount point.
	 * Derived classes may want to overwrite this.
	 **/
	virtual bool isMountPoint() const  { return false; }

	/**
	 * Sets the mount point state, i.e. whether or not this is a mount
	 * point.
	 *
	 * This default implementation silently ignores the value passed and
	 * does nothing. Derived classes may want to overwrite this.
	 **/
	virtual void setMountPoint( bool isMountPoint = true )
	    { Q_UNUSED( isMountPoint ); return; }

	/**
	 * Returns true if this subtree is finished reading.
	 *
	 * This default implementation always returns 'true';
	 * derived classes should overwrite this.
	 **/
	virtual bool isFinished() { return true; }

	/**
	 * Returns true if this subtree is busy, i.e. it is not finished
	 * reading yet.
	 *
	 * This default implementation always returns 'false';
	 * derived classes should overwrite this.
	 **/
	virtual bool isBusy() { return false; }

	/**
	 * Returns the number of pending read jobs in this subtree. When this
	 * number reaches zero, the entire subtree is done.
	 * Derived classes that have children should overwrite this.
	 **/
	virtual int pendingReadJobs() { return 0;  }


	//
	// Tree management
	//

	/**
	 * Returns a pointer to the DirTree this entry belongs to.
	 **/
	DirTree * tree() const { return _tree; }

	/**
	 * Returns a pointer to this entry's parent entry or 0 if there is
	 * none.
	 **/
	DirInfo * parent() const { return _parent; }

	/**
	 * Set the "parent" pointer.
	 **/
	void setParent( DirInfo *newParent ) { _parent = newParent; }

	/**
	 * Returns a pointer to the next entry on the same level
	 * or 0 if there is none.
	 **/
	FileInfo * next() const { return _next;	  }

	/**
	 * Set the "next" pointer.
	 **/
	void  setNext( FileInfo *newNext ) { _next = newNext; }

	/**
	 * Returns the first child of this item or 0 if there is none.
	 * Use the child's next() method to get the next child.
	 *
	 * This default implementation always returns 0.
	 **/
	virtual FileInfo * firstChild() const { return 0; }

	/**
	 * Set this entry's first child.
	 * Use this method only if you know exactly what you are doing.
	 *
	 * This default implementation does nothing.
	 * Derived classes might want to overwrite this.
	 **/
	virtual void setFirstChild( FileInfo *newFirstChild )
	    { Q_UNUSED( newFirstChild ); }

	/**
	 * Returns true if this entry has any children.
	 **/
	virtual bool hasChildren() const;

	/**
	 * Returns true if this entry is in subtree 'subtree', i.e. if this is
	 * a child or grandchild etc. of 'subtree'.
	 **/
	bool isInSubtree( const FileInfo *subtree ) const;

	/**
	 * Locate a child somewhere in this subtree whose URL (i.e. complete
	 * path) matches the URL passed. Returns 0 if there is no such child.
	 *
	 * Notice: This is a very expensive operation since the entire subtree
	 * is searched recursively.
	 *
	 * Derived classes might or might not wish to overwrite this method;
	 * it's only advisable to do so if a derived class comes up with a
	 * different method than brute-force searching all children.
	 *
	 * 'findDotEntries' specifies if locating "dot entries" (".../<Files>")
	 * is desired.
	 **/
	virtual FileInfo * locate( QString url, bool findDotEntries = false );

	/**
	 * Insert a child into the children list.
	 *
	 * The order of children in this list is absolutely undefined;
	 * don't rely on any implementation-specific order.
	 *
	 * This default implementation does nothing.
	 **/
	virtual void insertChild( FileInfo *newChild ) { Q_UNUSED( newChild ); }

	/**
	 * Return the "Dot Entry" for this node if there is one (or 0
	 * otherwise): This is a pseudo entry that directory nodes use to store
	 * non-directory children separately from directories. This way the end
	 * user can easily tell which summary fields belong to the directory
	 * itself and which are the accumulated values of the entire subtree.
	 *
	 * This default implementation always returns 0.
	 **/
	virtual DotEntry * dotEntry() const { return 0; }

	/**
	 * Set a "Dot Entry". This makes sense for directories only.
	 *
	 * This default implementation does nothing.
	 **/
	virtual void setDotEntry( FileInfo *newDotEntry )
	    { Q_UNUSED( newDotEntry ); }

        /**
         * Return 'true' if this is a pseudo directory: A "dot entry" or an
         * "attic".
         **/
        virtual bool isPseudoDir() const
            { return isDotEntry() || isAttic(); }

	/**
	 * Returns true if this is a "Dot Entry".
	 * See dotEntry() for details.
	 *
	 * This default implementation always returns false.
	 **/
	virtual bool isDotEntry() const { return false; }

	/**
	 * Return the "Attic" entry for this node if there is one (or 0
	 * otherwise): This is a pseudo entry that directory nodes use to store
	 * ignored files and directories separately from the normal tree
	 * hierarchy.
	 *
	 * This default implementation always returns 0.
	 **/
	virtual Attic * attic() const { return 0; }

	/**
	 * Check if this is an attic entry where ignored files and directories
	 * are stored.
	 *
	 * This default implementation always returns false.
	 **/
	virtual bool isAttic() const { return false; }

	/**
	 * (Translated) user-visible string for a "Dot Entry" ("<Files>").
	 **/
	static QString dotEntryName();

	/**
	 * (Translated) user-visible string for the "Attic" ("<Ignored>").
	 **/
        static QString atticName();

	/**
	 * Returns the tree level (depth) of this item.
	 * The topmost level is 0.
	 *
	 * This is a (somewhat) expensive operation since it will recurse up
	 * to the top of the tree.
	 **/
	int treeLevel() const;

	/**
	 * Notification that a child has been added somewhere in the subtree.
	 *
	 * This default implementation does nothing.
	 **/
	virtual void childAdded( FileInfo *newChild ) { Q_UNUSED( newChild ); }

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
	virtual void unlinkChild( FileInfo *deletedChild ) { Q_UNUSED( deletedChild ); }

	/**
	 * Notification that a child is about to be deleted somewhere in the
	 * subtree.
	 **/
	virtual void deletingChild( FileInfo *deletedChild ) { Q_UNUSED( deletedChild ); }

	/**
	 * Get the current state of the directory reading process:
	 *
	 * This default implementation always returns DirFinished.
	 * Derived classes should overwrite this.
	 **/
	virtual DirReadState readState() const { return DirFinished; }

	/**
	 * Returns true if this is a DirInfo object.
	 *
	 * Don't confuse this with isDir() which tells whether or not this is a
	 * disk directory! Both should return the same, but you'll never know -
	 * better be safe than sorry!
	 *
	 * This default implementation always returns 'false'. Derived classes
	 * (in particular, those derived from DirInfo) should overwrite this.
	 **/
	virtual bool isDirInfo() const { return false; }

        /**
         * Returns true if this is a PkgInfo object.
         *
	 * This default implementation always returns 'false'. Derived classes
	 * (in particular, those derived from PkgInfo) should overwrite this.
         **/
        virtual bool isPkgInfo() const { return false; }

	/**
	 * Try to convert this to a DirInfo pointer. This returns null if this
	 * is not a DirInfo.
	 **/
	DirInfo * toDirInfo();

	/**
	 * Try to convert this to a DirInfo pointer. This returns null if this
	 * is not a DirInfo.
	 **/
	DotEntry * toDotEntry();

	/**
	 * Try to convert this to a DirInfo pointer. This returns null if this
	 * is not a DirInfo.
	 **/
	Attic * toAttic();

	/**
	 * Try to convert this to a PkgInfo pointer. This returns null if this
	 * is not a DirInfo.
	 **/
        PkgInfo * toPkgInfo();

	/**
	 * Returns true if this is a sparse file, i.e. if this file has
	 * actually fewer disk blocks allocated than its byte size would call
	 * for.
	 *
	 * This is a cheap operation since it relies on a cached flag that is
	 * calculated in the constructor rather than doing repeated
	 * calculations and comparisons.
	 *
	 * Please not that size() already takes this into account.
	 **/
	bool isSparseFile() const { return _isSparseFile; }

        /**
         * Returns true if this FileInfo was read from a cache file.
         **/
        bool isCached() const;

        /**
         * Returns true if this FileInfo was ignored by some rule (e.g. in the
         * "unpackaged files" view).
         **/
        bool isIgnored() const { return _isIgnored; }

        /**
         * Set the "ignored" flag. Notice that this only sets the flag; it does
         * not reparent the FileInfo or anything like that.
         **/
        void setIgnored( bool ignored ) { _isIgnored = ignored; }

        /**
         * Return the nearest PkgInfo parent or 0 if there is none.
         **/
        PkgInfo * pkgInfoParent() const;


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

	short		_magic;			// magic number to detect if this object is valid
	QString		_name;			// the file name (without path!)
	bool		_isLocalFile  :1;	// flag: local or remote file?
	bool		_isSparseFile :1;	// (cache) flag: sparse file (file with "holes")?
        bool            _isIgnored    :1;       // flag: ignored by rule?
	dev_t		_device;		// device this object resides on
	mode_t		_mode;			// file permissions + object type
	nlink_t		_links;			// number of links
	uid_t		_uid;			// User ID of owner
	gid_t		_gid;			// Group ID of owner
	FileSize	_size;			// size in bytes
	FileSize	_blocks;		// 512 bytes blocks
	time_t		_mtime;			// modification time

	DirInfo	 *	_parent;		// pointer to the parent entry
	FileInfo *	_next;			// pointer to the next entry
	DirTree	 *	_tree;			// pointer to the parent tree

    };	// class FileInfo



    typedef QList<FileInfo *> FileInfoList;



    //----------------------------------------------------------------------
    //			       Static Functions
    //----------------------------------------------------------------------


    /**
     * Format a file / subtree size human readable, i.e. in "GB" / "MB"
     * etc. rather than huge numbers of digits.
     *
     * Note: For logDebug() etc., operator<< is overwritten to do exactly that:
     *
     *	   logDebug() << "Size: " << x->totalSize() << endl;
     **/
    QString formatSize ( FileSize lSize );

    /**
     * Format a timestamp (like the latestMTime()) human-readable.
     **/
    QString formatTime( time_t rawTime );

    /**
     * Return the last pathname component of a file name.
     *
     * Examples:
     *
     *	   "/home/bob/foo.txt"	-> "foo.txt"
     *	   "foo.txt"		-> "foo.txt"
     *	   "/usr/bin"		-> "bin"
     *	   "/usr/bin/"		-> "bin"
     *
     * Notice that FileInfo also has a member function baseName().
     **/
    QString baseName( const QString & fileName );

    /**
     * Format the mode (the permissions bits) returned from the stat() system
     * call in the commonly used formats, both symbolic and octal, e.g.
     *     drwxr-xr-x  0755
     **/
    QString formatPermissions( mode_t mode );

    /**
     * Format a number in octal with a leading zero.
     **/
    QString formatOctal( int number );

    /**
     * Return the mode (the permission bits) returned from stat() like the
     * "ls -l" shell command does, e.g.
     *
     *     drwxr-xr-x
     *
     * 'omitTypeForRegularFiles' specifies if the leading "-" should be omitted.
     **/
    QString symbolicMode( mode_t perm, bool omitTypeForRegularFiles = false );

    /**
     * Format the filesystem object type from a mode, e.g. "Directory",
     * "Symbolic Link", "Block Device", "File".
     **/
    QString formatFileSystemObjectType( mode_t mode );


    /**
     * Print the debugUrl() of a FileInfo in a debug stream.
     **/
    inline QTextStream & operator<< ( QTextStream & stream, const FileInfo * info )
    {
	if ( info )
	{
	    if ( info->checkMagicNumber() )
		stream << info->debugUrl();
	    else
		stream << "<INVALID FileInfo *>";
	}
	else
	    stream << "<NULL FileInfo *>";

	return stream;
    }


    /**
     * Human-readable output of a file size in a debug stream.
     **/
    inline QTextStream & operator<< ( QTextStream & stream, FileSize lSize )
    {
	stream << formatSize( lSize );

	return stream;
    }

}	// namespace QDirStat


#endif // ifndef FileInfo_h

