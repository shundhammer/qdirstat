/*
 *   File name: DirTreeCache.cpp
 *   Summary:	QDirStat cache reader / writer
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <ctype.h>
#include <errno.h>
#include <QUrl>

#include "Logger.h"
#include "DirTreeCache.h"
#include "DirTree.h"
#include "ExcludeRules.h"

#define KB 1024LL
#define MB (1024LL*1024)
#define GB (1024LL*1024*1024)
#define TB (1024LL*1024*1024*1024)


using namespace QDirStat;


CacheWriter::CacheWriter( const QString & fileName, DirTree *tree )
{
    _ok = writeCache( fileName, tree );
}


CacheWriter::~CacheWriter()
{
    // NOP
}


bool CacheWriter::writeCache( const QString & fileName, DirTree *tree )
{
    if ( ! tree || ! tree->root() )
	return false;

    gzFile cache = gzopen( (const char *) fileName.toUtf8(), "w" );

    if ( cache == 0 )
    {
	logError() << "Can't open " << fileName << ": " << strerror( errno ) << endl;
	return false;
    }

    gzprintf( cache, "[qdirstat %s cache file]\n", CACHE_FORMAT_VERSION );
    gzprintf( cache,
	     "# Do not edit!\n"
	     "#\n"
	     "# Type\tpath\t\tsize\tmtime\t\t<optional fields>\n"
	     "\n" );

    writeTree( cache, tree->root()->firstChild() );
    gzclose( cache );

    return true;
}


void CacheWriter::writeTree( gzFile cache, FileInfo * item )
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

    FileInfo * child = item->firstChild();

    while ( child )
    {
	writeTree( cache, child );
	child = child->next();
    }
}


void CacheWriter::writeItem( gzFile cache, FileInfo * item )
{
    if ( ! item )
	return;

    // Write file type

    const char * file_type = "";
    if	    ( item->isFile()		)	file_type = "F";
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

	gzprintf( cache, " %s", QUrl( item->url() ).toEncoded().data() );
    }
    else
    {
	// Use relative path

	gzprintf( cache, "\t%s", QUrl( item->name() ).toEncoded().data() );
    }


    // Write size

    gzprintf( cache, "\t%s", formatSize( item->size() ).toUtf8().data() );


    // Write mtime

    gzprintf( cache, "\t0x%lx", (unsigned long) item->mtime() );

    // Optional fields

    if ( item->isSparseFile() )
	gzprintf( cache, "\tblocks: %lld", item->blocks() );

    if ( item->isFile() && item->links() > 1 )
	gzprintf( cache, "\tlinks: %u", (unsigned) item->links() );

    gzputc( cache, '\n' );
}


QString CacheWriter::formatSize( FileSize size )
{
    QString str;

    if ( size >= TB && size % TB == 0 )
    {
	str.sprintf( "%lldT", size / TB );
	return str;
    }

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







CacheReader::CacheReader( const QString & fileName,
			  DirTree *	  tree,
			  DirInfo *	  parent )
    : QObject()
{
    _fileName		= fileName;
    _buffer[0]		= 0;
    _line		= _buffer;
    _lineNo		= 0;
    _ok			= true;
    _tree		= tree;
    _toplevel		= parent;
    _lastDir		= 0;
    _lastExcludedDir	= 0;

    _cache = gzopen( fileName.toUtf8(), "r" );

    if ( _cache == 0 )
    {
	logError() << "Can't open " << fileName << ": " << strerror( errno ) << endl;
	_ok = false;
	emit error();
	return;
    }

    // logDebug() << "Opening " << fileName << " OK" << endl;
    checkHeader();
}


CacheReader::~CacheReader()
{
    if ( _cache )
	gzclose( _cache );

    // logDebug() << "Cache reading finished" << endl;

    if ( _toplevel )
	_toplevel->finalizeAll();

    emit finished();
}


void CacheReader::rewind()
{
    if ( _cache )
    {
	gzrewind( _cache );
	checkHeader();		// skip cache header
    }
}


bool CacheReader::read( int maxLines )
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


void CacheReader::addItem()
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
	if ( strcasecmp( keyword, "links:"  ) == 0 ) links_str	= val_str;
    }


    // Type

    mode_t mode = S_IFREG;

    if	    ( strcasecmp( type, "F"	   ) == 0 )	mode = S_IFREG;
    else if ( strcasecmp( type, "D"	   ) == 0 )	mode = S_IFDIR;
    else if ( strcasecmp( type, "L"	   ) == 0 )	mode = S_IFLNK;
    else if ( strcasecmp( type, "BlockDev" ) == 0 )	mode = S_IFBLK;
    else if ( strcasecmp( type, "CharDev"  ) == 0 )	mode = S_IFCHR;
    else if ( strcasecmp( type, "FIFO"	   ) == 0 )	mode = S_IFIFO;
    else if ( strcasecmp( type, "Socket"   ) == 0 )mode = S_IFSOCK;


    // Path

    if ( *raw_path == '/' )
	_lastDir = 0;



    // Size

    char * end = 0;
    FileSize size = strtoll( size_str, &end, 10 );

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

    FileSize blocks = blocks_str ? strtoll( blocks_str, 0, 10 ) : -1;


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

    QString path = QUrl::fromEncoded( raw_path ).path();
    QString name = QUrl::fromEncoded( raw_name ).fileName();

    if ( _lastExcludedDir )
    {
	if ( path.startsWith( _lastExcludedDirUrl ) )
	{
	    // logDebug() << "Excluding " << path << "/" << name << endl;
	    return;
	}
    }

    // Find parent in tree

    DirInfo * parent = _lastDir;

    if ( ! parent && _tree->root() )
    {
	// Try the easy way first - the starting point of this cache

	if ( _toplevel )
	    parent = dynamic_cast<DirInfo *> ( _toplevel->locate( path ) );


	// Fallback: Search the entire tree

	if ( ! parent )
	    parent = dynamic_cast<DirInfo *> ( _tree->locate( path ) );


	if ( ! parent ) // Still nothing?
	{
#if 0
	    logError() << _fileName << ":" << _lineNo << ": "
		      << "Could not locate parent " << path << endl;
#endif

	    return;	// Ignore this cache line completely
	}
    }

    if ( strcasecmp( type, "D" ) == 0 )
    {
	// logDebug() << "Creating DirInfo  for " << name << endl;
	DirInfo * dir = new DirInfo( _tree, parent, name,
				       mode, size, mtime );
	dir->setReadState( DirCached );
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
	    if ( ExcludeRules::instance()->match( dir->url() ) )
	    {
		logDebug() << "Excluding " << name << endl;
		dir->setExcluded();
		dir->setReadState( DirOnRequestOnly );
		_tree->sendFinalizeLocal( dir );
		dir->finalizeLocal();
		_tree->sendReadJobFinished( dir );

		_lastExcludedDir    = dir;
		_lastExcludedDirUrl = _lastExcludedDir->url();
		_lastDir	    = 0;
	    }
	}
    }
    else
    {
	if ( parent )
	{
	    // logDebug() << "Creating FileInfo for " << parent->debugUrl() << "/" << name << endl;

	    FileInfo * item = new FileInfo( _tree, parent, name,
					      mode, size, mtime,
					      blocks, links );
	    parent->insertChild( item );
	    _tree->childAddedNotify( item );
	}
	else
	{
	    logError() << _fileName << ":" << _lineNo << ": "
		      << "No parent for item " << name << endl;
	}
    }
}


bool CacheReader::eof()
{
    if ( ! _ok || ! _cache )
	return true;

    return gzeof( _cache );
}


QString CacheReader::firstDir()
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


bool CacheReader::checkHeader()
{
    if ( ! _ok || ! readLine() )
	return false;

    // logDebug() << "Checking cache file header" << endl;
    QString line( _line );
    splitLine();

    // Check for    [qdirstat <version> cache file]

    if ( fieldsCount() != 4 )	_ok = false;

    if ( _ok )
    {
	if ( strcmp( field( 0 ), "[qdirstat" ) != 0 ||
	     strcmp( field( 2 ), "cache"     ) != 0 ||
	     strcmp( field( 3 ), "file]"     ) != 0 )
	{
	    _ok = false;
	    logError() << _fileName << ":" << _lineNo
		      << ": Unknown file format" << endl;
	}
    }

    if ( _ok )
    {
	QString version = field( 1 );

	// currently not checking version number
	// for future use

	if ( ! _ok )
	    logError() << _fileName << ":" << _lineNo
		      << ": Incompatible cache file version" << endl;
    }

    // logDebug() << "Cache file header check OK: " << _ok << endl;

    if ( ! _ok )
	emit error();

    return _ok;
}


bool CacheReader::readLine()
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
		logError() << _fileName << ":" << _lineNo << ": Read error" << endl;
		emit error();
	    }

	    return false;
	}

	_line = skipWhiteSpace( _buffer );
	killTrailingWhiteSpace( _line );

	// logDebug() << "line[ " << _lineNo << "]: \"" << _line<< "\"" << endl;

    } while ( ! gzeof( _cache ) &&
	      ( *_line == 0   ||	// empty line
		*_line == '#'	  ) );	// comment line

    return true;
}


void CacheReader::splitLine()
{
    _fieldsCount = 0;

    if ( ! _ok || ! _line )
	return;

    if ( *_line == '#' )	// skip comment lines
	*_line = 0;

    char * current = _line;
    char * end	   = _line + strlen( _line );

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


char * CacheReader::field( int no )
{
    if ( no >= 0 && no < _fieldsCount )
	return _fields[ no ];
    else
	return 0;
}


char * CacheReader::skipWhiteSpace( char * cptr )
{
    if ( cptr == 0 )
	return 0;

    while ( *cptr != 0 && isspace( *cptr ) )
	cptr++;

    return cptr;
}


char * CacheReader::findNextWhiteSpace( char * cptr )
{
    if ( cptr == 0 )
	return 0;

    while ( *cptr != 0 && ! isspace( *cptr ) )
	cptr++;

    return *cptr == 0 ? 0 : cptr;
}


void CacheReader::killTrailingWhiteSpace( char * cptr )
{
    char * start = cptr;

    if ( cptr == 0 )
	return;

    cptr = start + strlen( start ) -1;

    while ( cptr >= start && isspace( *cptr ) )
	*cptr-- = 0;
}

