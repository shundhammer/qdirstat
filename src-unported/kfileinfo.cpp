/*
 *   File name:	kfileinfo.cpp
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <klocale.h>

#include "kfileinfo.h"
#include "kdirinfo.h"
#include "kdirsaver.h"

// Some file systems (NTFS seems to be among them) may handle block fragments well.
// Don't report files as "sparse" files if the block size is only a few bytes
// less than the byte size - it may be due to intelligent fragment handling.

#define FRAGMENT_SIZE	2048


using namespace KDirStat;


KFileInfo::KFileInfo( KDirTree   *	tree,
		      KDirInfo   *	parent,
		      const char *	name )
    : _parent( parent )
    , _next( 0 )
    , _tree( tree )
{
    _isLocalFile	= true;
    _isSparseFile	= false;
    _name	 	= name ? name : "";
    _device	 	= 0;
    _mode	 	= 0;
    _links	 	= 0;
    _size	 	= 0;
    _blocks	 	= 0;
    _mtime	 	= 0;
}


KFileInfo::KFileInfo( const QString &	filenameWithoutPath,
		      struct stat *	statInfo,
		      KDirTree    *	tree,
		      KDirInfo	  *	parent )
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
	_size	 	= 0;
	_blocks	 	= 0;
	_isSparseFile	= false;
    }
    else
    {
	_size	 	= statInfo->st_size;
	_blocks	 	= statInfo->st_blocks;
	_isSparseFile	= isFile()
	    && _blocks > 0				// if file system can't report blocks
	    && allocatedSize() + FRAGMENT_SIZE < _size;	// allow for intelligent fragment handling

	if ( _isSparseFile )
	{
	    kdDebug() << "Found sparse file: " << this
		      << "    Byte size: " << formatSize( byteSize() )
		      << "  Allocated: " << formatSize( allocatedSize() )
		      << " (" << (int) _blocks << " blocks)"
		      << endl;
	}

#if 0
	if ( isFile() && _links > 1 )
	{
	    kdDebug() << _links << " hard links: " << this << endl;
	}
#endif
    }

#if 0
#warning Debug mode: Huge sizes
    _size <<= 10;
#endif
}


KFileInfo::KFileInfo(  const KFileItem	* fileItem,
		       KDirTree    	* tree,
		       KDirInfo		* parent )
    : _parent( parent )
    , _next( 0 )
    , _tree( tree )
{
    CHECK_PTR( fileItem );

    _isLocalFile = fileItem->isLocalFile();
    _name	 = parent ? fileItem->name() : fileItem->url().url();
    _device	 = 0;
    _mode	 = fileItem->mode();
    _links	 = 1;


    if ( isSpecial() )
    {
	_size	 	= 0;
	_blocks	 	= 0;
	_isSparseFile 	= false;
    }
    else
    {
	_size	 = fileItem->size();

	// Since KFileItem does not return any information about allocated disk
	// blocks, calculate that information artificially so callers don't
	// need to bother with special cases depending on how this object was
	// constructed.

	_blocks	 = _size / blockSize();

	if ( ( _size % blockSize() ) > 0 )
	    _blocks++;

	// There is no way to find out via KFileInfo if this is a sparse file.
	_isSparseFile = false;
    }

    _mtime	 = fileItem->time( KIO::UDS_MODIFICATION_TIME );
}


KFileInfo::KFileInfo( KDirTree * 	tree,
		      KDirInfo * 	parent,
		      const QString &	filenameWithoutPath,
		      mode_t	   	mode,
		      KFileSize	   	size,
		      time_t	   	mtime,
		      KFileSize	   	blocks,
		      nlink_t	   	links )
    : _parent( parent )
    , _next( 0 )
    , _tree( tree )
{
    _name		= filenameWithoutPath;
    _isLocalFile	= true;
    _mode		= mode;
    _size	 	= size;
    _mtime	 	= mtime;
    _links	 	= links;

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

    // kdDebug() << "Created KFileInfo " << this << endl;
}


KFileInfo::~KFileInfo()
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


KFileSize
KFileInfo::allocatedSize() const
{
    return blocks() * blockSize();
}


KFileSize
KFileInfo::size() const
{
    KFileSize sz = _isSparseFile ? allocatedSize() : _size;

    if ( _links > 1 )
	sz /= _links;

    return sz;
}


QString
KFileInfo::url() const
{
    if ( _parent )
    {
	QString parentUrl = _parent->url();

	if ( isDotEntry() )	// don't append "/." for dot entries
	    return parentUrl;

	if ( parentUrl == "/" ) // avoid duplicating slashes
	    return parentUrl + _name;
	else
	    return parentUrl + "/" + _name;
    }
    else
	return _name;
}


QString
KFileInfo::debugUrl() const
{
    return url() + ( isDotEntry() ? "/<Files>" : "" );
}


QString
KFileInfo::urlPart( int targetLevel ) const
{
    int level = treeLevel();	// Cache this - it's expensive!

    if ( level < targetLevel )
    {
	kdError() << k_funcinfo << "URL level " << targetLevel
		  << " requested, this is level " << level << endl;
	return "";
    }

    const KFileInfo *item = this;

    while ( level > targetLevel )
    {
	level--;
	item = item->parent();
    }

    return item->name();
}


int
KFileInfo::treeLevel() const
{
    int		level	= 0;
    KFileInfo *	parent	= _parent;

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


bool
KFileInfo::hasChildren() const
{
    return firstChild() || dotEntry();
}


bool
KFileInfo::isInSubtree( const KFileInfo *subtree ) const
{
    const KFileInfo * ancestor = this;

    while ( ancestor )
    {
	if ( ancestor == subtree )
	    return true;

	ancestor = ancestor->parent();
    }

    return false;
}


KFileInfo *
KFileInfo::locate( QString url, bool findDotEntries )
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

	KFileInfo *child = firstChild();

	while ( child )
	{
	    KFileInfo *foundChild = child->locate( url, findDotEntries );

	    if ( foundChild )
		return foundChild;
	    else
		child = child->next();
	}


	// Special case: The dot entry is requested.

	if ( findDotEntries && dotEntry() && url == "<Files>" )
	    return dotEntry();

	// Search the dot entry if there is one - but only if there is no more
	// path delimiter left in the URL. The dot entry contains files only,
	// and their names may not contain the path delimiter, nor can they
	// have children. This check is not strictly necessary, but it may
	// speed up things a bit if we don't search the non-directory children
	// if the rest of the URL consists of several pathname components.

	if ( dotEntry() &&
	     url.find ( "/" ) < 0 )	// No (more) "/" in this URL
	{
	    return dotEntry()->locate( url, findDotEntries );
	}
    }

    return 0;
}






KURL
KDirStat::fixedUrl( const QString & dirtyUrl )
{
    KURL url = dirtyUrl;

    if ( ! url.isValid() )		// Maybe it's just a path spec?
    {
	url = KURL();			// Start over with an empty, but valid URL
	url.setPath( dirtyUrl );	// and use just the path part.
    }
    else
    {
	url.cleanPath();	// Resolve relative paths, get rid of multiple slashes.
    }


    // Strip off the rightmost slash - some kioslaves (e.g. 'tar') can't handle that.

    QString path = url.path();

    if ( path.length() > 1 && path.right(1) == "/" )
    {
	path = path.left( path.length()-1 );
	url.setPath( path );
    }

    if ( url.isLocalFile() )
    {
	// Make a relative path an absolute path

	KDirSaver dir( url.path() );
	url.setPath( dir.currentDirPath() );
    }

    return url;
}






QString
KDirStat::formatSize( KFileSize lSize )
{
   QString	sizeString;
   double	size;
   QString	unit;

   if ( lSize < 1024 )
   {
      sizeString.setNum( (long) lSize );

      unit = i18n( "Bytes" );
   }
   else
   {
      size = lSize / 1024.0;		// kB

      if ( size < 1024.0 )
      {
	 sizeString.sprintf( "%.1f", size );
	 unit = i18n( "kB" );
      }
      else
      {
	 size /= 1024.0;		// MB

	 if ( size < 1024.0 )
	 {
	    sizeString.sprintf( "%.1f", size );
	    unit = i18n ( "MB" );
	 }
	 else
	 {
	    size /= 1024.0;		// GB - we won't go any further...

	    sizeString.sprintf( "%.2f", size );
	    unit = i18n ( "GB" );
	 }
      }
   }

   if ( ! unit.isEmpty() )
   {
      sizeString += " " + unit;
   }

   return sizeString;
}



// EOF
