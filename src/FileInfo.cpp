/*
 *   File name: FileInfo.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <QDateTime>

#include "FileInfo.h"
#include "DirInfo.h"
#include "DirSaver.h"
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
    _name	  = name ? name : "";
    _device	  = 0;
    _mode	  = 0;
    _links	  = 0;
    _size	  = 0;
    _blocks	  = 0;
    _mtime	  = 0;
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
    _name	 = filenameWithoutPath;

    _device	 = statInfo->st_dev;
    _mode	 = statInfo->st_mode;
    _links	 = statInfo->st_nlink;
    _mtime	 = statInfo->st_mtime;

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
	    && _blocks > 0				// if file system can't report blocks
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
    _mode	 = mode;
    _size	 = size;
    _mtime	 = mtime;
    _links	 = links;

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
    // NOP

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


QString FileInfo::debugUrl() const
{
    return url() + ( isDotEntry() ? ( QString( "/" ) + dotEntryName() ) : "" );
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


    if ( _parent )
	return _parent->treeLevel() + 1;
    else
	return 0;
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
    if ( ! url.startsWith( _name ) )
	return 0;
    else					// URL starts with this node's name
    {
	url.remove( 0, _name.length() );	// Remove leading name of this node

	if ( url.length() == 0 )		// Nothing left?
	    return this;			// Hey! That's us!

	if ( url.startsWith( "/" ) )		// If the next thing a path delimiter,
	    url.remove( 0, 1 );			// remove that leading delimiter.
	else					// No path delimiter at the beginning
	{
	    if ( _name.right(1) != "/" &&	// and this is not the root directory
		 ! isDotEntry() )		// or a dot entry:
		return 0;			// This can't be any of our children.
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
	     url.contains( "/" ) < 0 )	   // No (more) "/" in this URL
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

	sizeString.sprintf( "%.2f ", size );
	sizeString += units.at( unitIndex );
    }

    return sizeString;
}

DirInfo * FileInfo::toDirInfo()
{
    DirInfo * dirInfo = dynamic_cast<DirInfo *>( this );
    CHECK_DYNAMIC_CAST( dirInfo, "DirInfo *" );

    return dirInfo;
}


QString QDirStat::formatTime( time_t rawTime )
{
    QDateTime time = QDateTime::fromTime_t( rawTime );
    return time.toString( Qt::DefaultLocaleShortDate );
}


QString QDirStat::baseName( const QString & fileName )
{
    QStringList segments = fileName.split( '/', QString::SkipEmptyParts );
    return segments.isEmpty() ? "" : segments.last();
}

