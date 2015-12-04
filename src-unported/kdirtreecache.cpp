/*
 *   File name:	kdirtreecache.cpp
 *   Summary:	KDirStat cache reader / writer
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2008-12-18
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <kdebug.h>
#include "kdirtreecache.h"
#include "kdirtree.h"
#include "kexcluderules.h"

#define KB 1024
#define MB (1024*1024)
#define GB (1024*1024*1024)


using namespace KDirStat;


KCacheWriter::KCacheWriter( const QString & fileName, KDirTree *tree )
{
    _ok = writeCache( fileName, tree );
}


KCacheWriter::~KCacheWriter()
{
    // NOP
}


bool
KCacheWriter::writeCache( const QString & fileName, KDirTree *tree )
{
    if ( ! tree || ! tree->root() )
	return false;

    gzFile cache = gzopen( (const char *) fileName, "w" );

    if ( cache == 0 )
    {
	kdError() << "Can't open " << fileName << ": " << strerror( errno ) << endl;
	return false;
    }

    gzprintf( cache, "[kdirstat %s cache file]\n", VERSION );
    gzprintf( cache,
	     "# Do not edit!\n"
	     "#\n"
	     "# Type\tpath\t\tsize\tmtime\t\t<optional fields>\n"
	     "\n" );

    writeTree( cache, tree->root() );
    gzclose( cache );

    return true;
}


void
KCacheWriter::writeTree( gzFile cache, KFileInfo * item )
{
    if ( ! item )
	return;

    //
    // Write entry for this item
    //

    if ( ! item->isDotEntry() )
	writeItem( cache, item );

    //
    // Write file children
    //

    if ( item->dotEntry() )
	writeTree( cache, item->dotEntry() );

    //
    // Recurse through subdirectories
    //

    KFileInfo * child = item->firstChild();

    while ( child )
    {
	writeTree( cache, child );
	child = child->next();
    }
}


void
KCacheWriter::writeItem( gzFile cache, KFileInfo * item )
{
    if ( ! item )
	return;

    // Write file type

    const char * file_type = "";
    if      ( item->isFile()		)	file_type = "F";
    else if ( item->isDir()		)	file_type = "D";
    else if ( item->isSymLink()		)	file_type = "L";
    else if ( item->isBlockDevice()	)	file_type = "BlockDev";
    else if ( item->isCharDevice()	)	file_type = "CharDev";
    else if ( item->isFifo()		)	file_type = "FIFO";
    else if ( item->isSocket()		)	file_type = "Socket";

    gzprintf( cache, "%s", file_type );

    // Write name

    if ( item->isDirInfo() && ! item->isDotEntry() )
    {
	// Use absolute path

	gzprintf( cache, " %s", (const char *) KURL::encode_string( item->url() ).latin1() );
    }
    else
    {
	// Use relative path

	gzprintf( cache, "\t%s", (const char *) KURL::encode_string( item->name() ).latin1() );
    }


    // Write size

    gzprintf( cache, "\t%s", (const char *) formatSize( item->size() ) );


    // Write mtime

    gzprintf( cache, "\t0x%lx", (unsigned long) item->mtime() );

    // Optional fields

    if ( item->isSparseFile() )
	gzprintf( cache, "\tblocks: %lld", item->blocks() );

    if ( item->isFile() && item->links() > 1 )
	gzprintf( cache, "\tlinks: %u", (unsigned) item->links() );

    gzputc( cache, '\n' );
}


QString
KCacheWriter::formatSize( KFileSize size )
{
    QString str;

    if ( size >= GB && size % GB == 0 )
    {
	str.sprintf( "%lldG", size / GB );
	return str;
    }

    if ( size >= MB && size % MB == 0 )
    {
	str.sprintf( "%lldM", size / MB );
	return str;
    }

    if ( size >= KB && size % KB == 0 )
    {
	str.sprintf( "%lldK", size / KB );
	return str;
    }

    str.sprintf( "%lld", size );
    return str;
}







KCacheReader::KCacheReader( const QString &	fileName,
			    KDirTree *		tree,
			    KDirInfo *		parent )
    : QObject()
{
    _fileName		= fileName;
    _buffer[0] 		= 0;
    _line		= _buffer;
    _lineNo		= 0;
    _ok			= true;
    _tree		= tree;
    _toplevel		= parent;
    _lastDir		= 0;
    _lastExcludedDir	= 0;

    _cache = gzopen( (const char *) fileName, "r" );

    if ( _cache == 0 )
    {
	kdError() << "Can't open " << fileName << ": " << strerror( errno ) << endl;
	_ok = false;
	emit error();
	return;
    }

    // kdDebug() << "Opening " << fileName << " OK" << endl;
    checkHeader();
}


KCacheReader::~KCacheReader()
{
    if ( _cache )
	gzclose( _cache );

    // kdDebug() << "Cache reading finished" << endl;

    if ( _toplevel )
	_toplevel->finalizeAll();

    emit finished();
}


void
KCacheReader::rewind()
{
    if ( _cache )
    {
	gzrewind( _cache );
	checkHeader();		// skip cache header
    }
}


bool
KCacheReader::read( int maxLines )
{
    while ( ! gzeof( _cache )
	    && _ok
	    && ( maxLines == 0 || --maxLines > 0 ) )
    {
	if ( readLine() )
	{
	    splitLine();
	    addItem();
	}
    }

    return _ok && ! gzeof( _cache );
}


void
KCacheReader::addItem()
{
    if ( fieldsCount() < 4 )
    {
	_ok = false;
	emit error();
	return;
    }

    int n = 0;
    char * type		= field( n++ );
    char * raw_path	= field( n++ );
    char * size_str	= field( n++ );
    char * mtime_str	= field( n++ );
    char * blocks_str	= 0;
    char * links_str	= 0;

    while ( fieldsCount() > n+1 )
    {
	char * keyword	= field( n++ );
	char * val_str	= field( n++ );

	if ( strcasecmp( keyword, "blocks:" ) == 0 ) blocks_str = val_str;
	if ( strcasecmp( keyword, "links:"  ) == 0 ) links_str  = val_str;
    }


    // Type

    mode_t mode = S_IFREG;

    if      ( strcasecmp( type, "F"        ) == 0 )	mode = S_IFREG;
    else if ( strcasecmp( type, "D"        ) == 0 )	mode = S_IFDIR;
    else if ( strcasecmp( type, "L"        ) == 0 )	mode = S_IFLNK;
    else if ( strcasecmp( type, "BlockDev" ) == 0 )	mode = S_IFBLK;
    else if ( strcasecmp( type, "CharDev"  ) == 0 )	mode = S_IFCHR;
    else if ( strcasecmp( type, "FIFO"     ) == 0 )	mode = S_IFIFO;
    else if ( strcasecmp( type, "Socket"   ) == 0 )	mode = S_IFSOCK;


    // Path

    if ( *raw_path == '/' )
	_lastDir = 0;



    // Size

    char * end = 0;
    KFileSize size = strtoll( size_str, &end, 10 );

    if ( end )
    {
	switch ( *end )
	{
	    case 'K':	size *= KB; break;
	    case 'M':	size *= MB; break;
	    case 'G':	size *= GB; break;
	    default: break;
	}
    }


    // MTime

    time_t mtime = strtol( mtime_str, 0, 0 );


    // Blocks

    KFileSize blocks = blocks_str ? strtoll( blocks_str, 0, 10 ) : -1;


    // Links

    int links = links_str ? atoi( links_str ) : 1;


    //
    // Create a new item
    //

    char * raw_name = raw_path;

    if ( *raw_path == '/' && _tree->root() )
    {
	// Split raw_path in path + name

	raw_name = strrchr( raw_path, '/' );

	if ( raw_name )
	    *raw_name++ = 0;	// Overwrite the last '/' with 0 byte - split string there
	else			// No '/' found
	    raw_name = raw_path;
    }

    QString path = KURL::decode_string( QString::fromLatin1( raw_path ) );
    QString name = KURL::decode_string( QString::fromLatin1( raw_name ) );

    if ( _lastExcludedDir )
    {
	if ( path.startsWith( _lastExcludedDirUrl ) )
	{
	    // kdDebug() << "Excluding " << path << "/" << name << endl;
	    return;
	}
    }

    // Find parent in tree
    
    KDirInfo * parent = _lastDir;

    if ( ! parent && _tree->root() )
    {
	// Try the easy way first - the starting point of this cache

	if ( _toplevel )
	    parent = dynamic_cast<KDirInfo *> ( _toplevel->locate( path ) );


	// Fallback: Search the entire tree

	if ( ! parent )
	    parent = dynamic_cast<KDirInfo *> ( _tree->locate( path ) );


	if ( ! parent )	// Still nothing?
	{
#if 0
	    kdError() << _fileName << ":" << _lineNo << ": "
		      << "Could not locate parent " << path << endl;
#endif

	    return;	// Ignore this cache line completely
	}
    }

    if ( strcasecmp( type, "D" ) == 0 )
    {
	// kdDebug() << "Creating KDirInfo  for " << name << endl;
	KDirInfo * dir = new KDirInfo( _tree, parent, name,
				       mode, size, mtime );
	dir->setReadState( KDirCached );
	_lastDir = dir;

	if ( parent )
	    parent->insertChild( dir );

	if ( ! _tree->root() )
	{
	    _tree->setRoot( dir );
	    _toplevel = dir;
	}

	if ( ! _toplevel )
	    _toplevel = dir;

	_tree->childAddedNotify( dir );

	if ( dir != _toplevel )
	{
	    if ( KExcludeRules::excludeRules()->match( dir->url() ) )
	    {
		// kdDebug() << "Excluding " << name << endl;
		dir->setExcluded();
		dir->setReadState( KDirOnRequestOnly );
		_tree->sendFinalizeLocal( dir );
		dir->finalizeLocal();

		_lastExcludedDir   	= dir;
		_lastExcludedDirUrl	= _lastExcludedDir->url();
		_lastDir		= 0;
	    }
	}
    }
    else
    {
	if ( parent )
	{
	    // kdDebug() << "Creating KFileInfo for " << parent->debugUrl() << "/" << name << endl;

	    KFileInfo * item = new KFileInfo( _tree, parent, name,
					      mode, size, mtime,
					      blocks, links );
	    parent->insertChild( item );
	    _tree->childAddedNotify( item );
	}
	else
	{
	    kdError() << _fileName << ":" << _lineNo << ": "
		      << "No parent for item " << name << endl;
	}
    }
}


bool
KCacheReader::eof()
{
    if ( ! _ok || ! _cache )
	return true;

    return gzeof( _cache );
}


QString
KCacheReader::firstDir()
{
    while ( ! gzeof( _cache ) && _ok )
    {
	if ( ! readLine() )
	    return "";

	splitLine();

	if ( fieldsCount() < 2 )
	    return "";

	int n = 0;
	char * type = field( n++ );
	char * path = field( n++ );

	if ( strcasecmp( type, "D" ) == 0 )
	    return QString( path );
    }

    return "";
}


bool
KCacheReader::checkHeader()
{
    if ( ! _ok || ! readLine() )
	return false;

    // kdDebug() << "Checking cache file header" << endl;
    QString line( _line );
    splitLine();

    // Check for    [kdirstat <version> cache file]

    if ( fieldsCount() != 4 )	_ok = false;

    if ( _ok )
    {
	if ( strcmp( field( 0 ), "[kdirstat" ) != 0 ||
	     strcmp( field( 2 ), "cache"     ) != 0 ||
	     strcmp( field( 3 ), "file]"     ) != 0 )
	{
	    _ok = false;
	    kdError() << _fileName << ":" << _lineNo
		      << ": Unknown file format" << endl;
	}
    }

    if ( _ok )
    {
	QString version = field( 1 );

	// currently not checking version number
	// for future use

	if ( ! _ok )
	    kdError() << _fileName << ":" << _lineNo
		      << ": Incompatible cache file version" << endl;
    }

    // kdDebug() << "Cache file header check OK: " << _ok << endl;

    if ( ! _ok )
	emit error();

    return _ok;
}


bool
KCacheReader::readLine()
{
    if ( ! _ok || ! _cache )
	return false;

    _fieldsCount = 0;

    do
    {
	_lineNo++;

	if ( ! gzgets( _cache, _buffer, MAX_CACHE_LINE_LEN-1 ) )
	{
	    _buffer[0]	= 0;
	    _line	= _buffer;

	    if ( ! gzeof( _cache ) )
	    {
		_ok = false;
		kdError() << _fileName << ":" << _lineNo << ": Read error" << endl;
		emit error();
	    }

	    return false;
	}

	_line = skipWhiteSpace( _buffer );
	killTrailingWhiteSpace( _line );

	// kdDebug() << "line[ " << _lineNo << "]: \"" << _line<< "\"" << endl;

    } while ( ! gzeof( _cache ) &&
	      ( *_line == 0   ||	// empty line
		*_line == '#'     ) );	// comment line

    return true;
}


void
KCacheReader::splitLine()
{
    _fieldsCount = 0;

    if ( ! _ok || ! _line )
	return;

    if ( *_line == '#' )	// skip comment lines
	*_line = 0;

    char * current = _line;
    char * end     = _line + strlen( _line );

    while ( current
	    && current < end
	    && *current
	    && _fieldsCount < MAX_FIELDS_PER_LINE-1 )
    {
	_fields[ _fieldsCount++ ] = current;
	current = findNextWhiteSpace( current );

	if ( current && current < end )
	{
	    *current++ = 0;
	    current = skipWhiteSpace( current );
	}
    }
}


char *
KCacheReader::field( int no )
{
    if ( no >= 0 && no < _fieldsCount )
	return _fields[ no ];
    else
	return 0;
}


char *
KCacheReader::skipWhiteSpace( char * cptr )
{
    if ( cptr == 0 )
	return 0;

    while ( *cptr != 0 && isspace( *cptr ) )
	cptr++;

    return cptr;
}


char *
KCacheReader::findNextWhiteSpace( char * cptr )
{
    if ( cptr == 0 )
	return 0;

    while ( *cptr != 0 && ! isspace( *cptr ) )
	cptr++;

    return *cptr == 0 ? 0 : cptr;
}


void
KCacheReader::killTrailingWhiteSpace( char * cptr )
{
    char * start = cptr;

    if ( cptr == 0 )
	return;

    cptr = start + strlen( start ) -1;

    while ( cptr >= start && isspace( *cptr ) )
	*cptr-- = 0;
}



// EOF
