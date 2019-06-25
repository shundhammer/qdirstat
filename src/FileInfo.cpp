/*
 *   File name: FileInfo.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>	// getpwuid()
#include <grp.h>	// getgrgid()
#include <unistd.h>

#include <QDateTime>

#include "FileInfo.h"
#include "DirInfo.h"
#include "DirTree.h"
#include "PkgInfo.h"
#include "Logger.h"
#include "Exception.h"

// Some file systems (NTFS seems to be among them) may handle block fragments
// well. Don't report files as "sparse" files if the block size is only a few
// bytes less than the byte size - it may be due to intelligent fragment
// handling.

#define FRAGMENT_SIZE	2048

using namespace QDirStat;


FileInfo::FileInfo( DirTree    * tree,
		    DirInfo    * parent,
		    const char * name )
    : _parent( parent )
    , _next( 0 )
    , _tree( tree )
{
    _isLocalFile  = true;
    _isSparseFile = false;
    _isIgnored    = false;
    _name	  = name ? name : "";
    _device	  = 0;
    _mode	  = 0;
    _links	  = 0;
    _uid	  = 0;
    _gid	  = 0;
    _size	  = 0;
    _blocks	  = 0;
    _mtime	  = 0;
    _magic	  = FileInfoMagic;
}


FileInfo::FileInfo( const QString & filenameWithoutPath,
		    struct stat	  * statInfo,
		    DirTree	  * tree,
		    DirInfo	  * parent )
    : _parent( parent )
    , _next( 0 )
    , _tree( tree )
{
    CHECK_PTR( statInfo );

    _isLocalFile = true;
    _isIgnored    = false;
    _name	 = filenameWithoutPath;

    _device	 = statInfo->st_dev;
    _mode	 = statInfo->st_mode;
    _links	 = statInfo->st_nlink;
    _uid	 = statInfo->st_uid;
    _gid	 = statInfo->st_gid;
    _mtime	 = statInfo->st_mtime;
    _magic	 = FileInfoMagic;

    if ( isSpecial() )
    {
	_size		= 0;
	_blocks		= 0;
	_isSparseFile	= false;
    }
    else
    {
	_size		= statInfo->st_size;
	_blocks		= statInfo->st_blocks;
	_isSparseFile	= isFile()
	    && _blocks >= 0				// if file system can report blocks
	    && allocatedSize() + FRAGMENT_SIZE < _size; // allow for intelligent fragment handling

	if ( _isSparseFile )
	{
	    logDebug() << "Found sparse file: " << this
		       << "    Byte size: " << formatSize( byteSize() )
		       << "  Allocated: " << formatSize( allocatedSize() )
		       << " (" << (int) _blocks << " blocks)"
		       << endl;
	}

#if 0
	if ( isFile() && _links > 1 )
	{
	    logDebug() << _links << " hard links: " << this << endl;
	}
#endif
    }
}



FileInfo::FileInfo( DirTree *	    tree,
		    DirInfo *	    parent,
		    const QString & filenameWithoutPath,
		    mode_t	    mode,
		    FileSize	    size,
		    time_t	    mtime,
		    FileSize	    blocks,
		    nlink_t	    links )
    : _parent( parent )
    , _next( 0 )
    , _tree( tree )
{
    _name	 = filenameWithoutPath;
    _isLocalFile = true;
    _isIgnored   = false;
    _device	 = 0;
    _mode	 = mode;
    _size	 = size;
    _mtime	 = mtime;
    _links	 = links;
    _uid	 = 0;
    _gid	 = 0;
    _magic	 = FileInfoMagic;

    if ( blocks < 0 )
    {
	_isSparseFile	= false;
	_blocks		= _size / blockSize();

	if ( ( _size % blockSize() ) > 0 )
	    _blocks++;
    }
    else
    {
	_isSparseFile	= true;
	_blocks		= blocks;
    }

    // logDebug() << "Created FileInfo " << this << endl;
}


FileInfo::~FileInfo()
{
    _magic = 0;

    /**
     * The destructor should also take care about unlinking this object from
     * its parent's children list, but regrettably that just doesn't work: At
     * this point (within the destructor) parts of the object are already
     * destroyed, e.g., the virtual table - virtual methods don't work any
     * more. Thus, somebody from outside must call deletingChild() just prior
     * to the actual "delete".
     *
     * This sucks, but it's the C++ standard.
     **/
}


bool FileInfo::checkMagicNumber() const
{
    return _magic == FileInfoMagic;
}


FileSize FileInfo::allocatedSize() const
{
    return blocks() * blockSize();
}


FileSize FileInfo::size() const
{
    FileSize sz = _isSparseFile ? allocatedSize() : _size;

    if ( _links > 1 && ! isDir() )
	sz /= _links;

    return sz;
}


QString FileInfo::url() const
{
    if ( _parent )
    {
	QString parentUrl = _parent->url();

	if ( isDotEntry() )	// don't append "/." for dot entries
	    return parentUrl;

	if ( ! parentUrl.endsWith( "/" ) && ! _name.startsWith( "/" ) )
	    parentUrl += "/";

	return parentUrl + _name;
    }
    else
	return _name;
}


QString FileInfo::path() const
{
    if ( isPkgInfo() )
        return "";

    if ( _parent )
    {
	QString parentPath = _parent->isPkgInfo() ? "/" : _parent->path();

	if ( isDotEntry() )	// don't append "/." for dot entries
	    return parentPath;

	if ( ! parentPath.endsWith( "/" ) && ! _name.startsWith( "/" ) )
	    parentPath += "/";

	return parentPath + _name;
    }
    else
	return _name;
}


QString FileInfo::debugUrl() const
{
    if ( _tree && this == _tree->root() )
	return "<root>";

    if ( isDotEntry() )
	return url() + "/" + dotEntryName();

    return url() ;
}


int FileInfo::treeLevel() const
{
    int		level	= 0;
    FileInfo *	parent	= _parent;

    while ( parent )
    {
	level++;
	parent = parent->parent();
    }

    return level;
}


bool FileInfo::hasChildren() const
{
    return firstChild() || dotEntry();
}


bool FileInfo::isInSubtree( const FileInfo *subtree ) const
{
    const FileInfo * ancestor = this;

    while ( ancestor )
    {
	if ( ancestor == subtree )
	    return true;

	ancestor = ancestor->parent();
    }

    return false;
}


FileInfo * FileInfo::locate( QString url, bool findDotEntries )
{
    if ( ! _tree )
        return 0;

    if ( ! url.startsWith( _name ) && this != _tree->root() )
	return 0;
    else					// URL starts with this node's name
    {
	if ( this != _tree->root() )		// The root item is invisible
	{
	    url.remove( 0, _name.length() );	// Remove leading name of this node

	    if ( url.length() == 0 )		// Nothing left?
		return this;			// Hey! That's us!

	    if ( url.startsWith( "/" ) )	// If the next thing a path delimiter,
		url.remove( 0, 1 );		// remove that leading delimiter.
	    else				// No path delimiter at the beginning
	    {
		if ( _name.right(1) != "/" &&	// and this is not the root directory
		     ! isDotEntry() )		// or a dot entry:
		    return 0;			// This can't be any of our children.
	    }
	}


	// Search all children

	FileInfo *child = firstChild();

	while ( child )
	{
	    FileInfo *foundChild = child->locate( url, findDotEntries );

	    if ( foundChild )
		return foundChild;
	    else
		child = child->next();
	}


	// Special case: The dot entry is requested.

	if ( findDotEntries && dotEntry() && url == dotEntryName() )
	    return dotEntry();

	// Search the dot entry if there is one - but only if there is no more
	// path delimiter left in the URL. The dot entry contains files only,
	// and their names may not contain the path delimiter, nor can they
	// have children. This check is not strictly necessary, but it may
	// speed up things a bit if we don't search the non-directory children
	// if the rest of the URL consists of several pathname components.

	if ( dotEntry() &&
	     ! url.contains( "/" ) )	   // No (more) "/" in this URL
	{
	    return dotEntry()->locate( url, findDotEntries );
	}
    }

    return 0;
}


float FileInfo::subtreePercent()
{
    if ( ! parent()			 ||	// only if there is a parent as calculation base
	 parent()->pendingReadJobs() > 0 ||	// not before subtree is finished reading
	 parent()->totalSize() == 0	 ||	// avoid division by zero
	 isExcluded() )				// not if this is an excluded object (dir)
    {
	return -1.0;
    }

    return ( 100.0 * totalSize() ) / (float) parent()->totalSize();
}


QString FileInfo::dotEntryName()
{
    return QObject::tr( "<Files>" );
}


bool FileInfo::isCached() const
{
    if ( isDirInfo() && ! isDotEntry() )
        return readState() == DirCached;
    else
        return _parent && _parent->readState() == DirCached;
}


bool FileInfo::hasUid() const
{
    return ! isPkgInfo() && ! isCached();
}


bool FileInfo::hasGid() const
{
    return ! isPkgInfo() && ! isCached();
}


QString FileInfo::userName() const
{
    if ( ! hasUid() )
	return QString();

    struct passwd * pw = getpwuid( uid() );

    if ( pw )
	return pw->pw_name;
    else
	return QString::number( uid() );
}


QString FileInfo::groupName() const
{
    if ( ! hasGid() )
	return QString();

    struct group * grp = getgrgid( gid() );

    if ( grp )
	return grp->gr_name;
    else
	return QString::number( gid() );
}


QString FileInfo::symbolicPermissions() const
{
    return symbolicMode( _mode,
			 true ); // omitTypeForRegularFiles
}


QString FileInfo::octalPermissions() const
{
    return formatOctal( ALLPERMS & _mode );
}


QString FileInfo::baseName() const
{
    return QDirStat::baseName( _name );
}


//---------------------------------------------------------------------------


QString QDirStat::formatSize( FileSize lSize )
{
    QString sizeString;
    int	    unitIndex = 0;

    static QStringList units;

    if ( units.isEmpty() )
    {
	units << QObject::tr( "Bytes" )
	      << QObject::tr( "kB" )
	      << QObject::tr( "MB" )
	      << QObject::tr( "GB" )
	      << QObject::tr( "TB" )
	      << QObject::tr( "PB" )
	      << QObject::tr( "EB" )
	      << QObject::tr( "ZB" )
	      << QObject::tr( "YB" );
    }

    if ( lSize < 1024 )
    {
	sizeString.sprintf( "%lld ", lSize );
	sizeString += units.at( unitIndex );
    }
    else
    {
	double size = lSize;

	while ( size >= 1024.0 && unitIndex < units.size() - 1 )
	{
	    size /= 1024.0;
	    ++unitIndex;
	}

	sizeString.sprintf( "%.1f ", size );
	sizeString += units.at( unitIndex );
    }

    return sizeString;
}


DirInfo * FileInfo::toDirInfo()
{
    DirInfo * dirInfo = dynamic_cast<DirInfo *>( this );

    return dirInfo;
}


PkgInfo * FileInfo::toPkgInfo()
{
    PkgInfo * pkgInfo = dynamic_cast<PkgInfo *>( this );

    return pkgInfo;
}


PkgInfo * FileInfo::pkgInfoParent() const
{
    FileInfo * pkg = _parent;

    while ( pkg )
    {
        if ( pkg->isPkgInfo() )
            return pkg->toPkgInfo();

        pkg = pkg->parent();
    }

    return 0;
}


QString QDirStat::formatTime( time_t rawTime )
{
    if ( rawTime == (time_t) 0 )
	return "";

    QDateTime time = QDateTime::fromTime_t( rawTime );
    return time.toString( Qt::DefaultLocaleShortDate );
}


// See also FileInfo::baseName()

QString QDirStat::baseName( const QString & fileName )
{
    QStringList segments = fileName.split( '/', QString::SkipEmptyParts );
    return segments.isEmpty() ? "" : segments.last();
}


QString QDirStat::formatPermissions( mode_t mode )
{
    return symbolicMode( mode, true ) + "  " + formatOctal( ALLPERMS & mode );
}


QString QDirStat::formatOctal( int number )
{
    return QString( "0" ) + QString::number( number, 8 );
}


QString QDirStat::symbolicMode( mode_t mode, bool omitTypeForRegularFiles )
{
    QString result;

    // Type

    if	    ( S_ISDIR ( mode ) )	  result = "d";
    else if ( S_ISCHR ( mode ) )	  result = "c";
    else if ( S_ISBLK ( mode ) )	  result = "b";
    else if ( S_ISFIFO( mode ) )	  result = "p";
    else if ( S_ISLNK ( mode ) )	  result = "l";
    else if ( S_ISSOCK( mode ) )	  result = "s";
    else if ( ! omitTypeForRegularFiles ) result = "-";

    // User

    result += ( mode & S_IRUSR ) ? "r" : "-";
    result += ( mode & S_IWUSR ) ? "w" : "-";

    if ( mode & S_ISUID )
	result += "s";
    else
	result += ( mode & S_IXUSR ) ? "x" : "-";

    // Group

    result += ( mode & S_IRGRP ) ? "r" : "-";
    result += ( mode & S_IWGRP ) ? "w" : "-";

    if ( mode & S_ISGID )
	result += "s";
    else
	result += ( mode & S_IXGRP ) ? "x" : "-";

    // Other

    result += ( mode & S_IROTH ) ? "r" : "-";
    result += ( mode & S_IWOTH ) ? "w" : "-";

    if ( mode & S_ISVTX )
	result += "t";
    else
	result += ( mode & S_IXOTH ) ? "x" : "-";

    return result;
}


QString QDirStat::formatFileSystemObjectType( mode_t mode )
{
    if	    ( S_ISDIR ( mode ) ) return QObject::tr( "Directory"	);
    else if ( S_ISCHR ( mode ) ) return QObject::tr( "Character Device" );
    else if ( S_ISBLK ( mode ) ) return QObject::tr( "Block Device"	);
    else if ( S_ISFIFO( mode ) ) return QObject::tr( "Named Pipe"	);
    else if ( S_ISLNK ( mode ) ) return QObject::tr( "Symbolic Link"	);
    else if ( S_ISSOCK( mode ) ) return QObject::tr( "Socket"		);
    else			 return QObject::tr( "File"		);
}
