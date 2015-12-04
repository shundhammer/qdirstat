/*
 *   File name: kdirtreecache.h
 *   Summary:	KDirStat cache reader / writer
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-12
 */


#ifndef KDirTreeCache_h
#define KDirTreeCache_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdio.h>
#include <zlib.h>
#include "kdirtree.h"

#ifndef NOT_USED
#    define NOT_USED(PARAM)	( (void) (PARAM) )
#endif


#define DEFAULT_CACHE_NAME	".kdirstat.cache.gz"
#define MAX_CACHE_LINE_LEN	1024
#define MAX_FIELDS_PER_LINE	32


namespace KDirStat
{
    class KCacheWriter
    {
    public:

	/**
	 * Write 'tree' to file 'fileName' in gzip format (using zlib).
	 *
	 * Check CacheWriter::ok() to see if writing the cache file went OK.
	 **/
	KCacheWriter( const QString & fileName, KDirTree *tree );

	/**
	 * Destructor
	 **/
	virtual ~KCacheWriter();

	/**
	 * Returns true if writing the cache file went OK.
	 **/
	bool ok() const { return _ok; }

	/**
	 * Format a file size as string - with trailing "G", "M", "K" for
	 * "Gigabytes", "Megabytes, "Kilobytes", respectively (provided there
	 * is no fractional part - 27M is OK, 27.2M is not).
	 **/
	QString formatSize( KFileSize size );


    protected:

	/**
	 * Write cache file in gzip format.
	 * Returns 'true' if OK, 'false' upon error.
	 **/
	bool writeCache( const QString & fileName, KDirTree *tree );

	/**
	 * Write 'item' recursively to cache file 'cache'.
	 * Uses zlib to write gzip-compressed files.
	 **/
	void writeTree( gzFile cache, KFileInfo * item );

	/**
	 * Write 'item' to cache file 'cache' without recursion.
	 * Uses zlib to write gzip-compressed files.
	 **/
	void writeItem( gzFile cache, KFileInfo * item );

	//
	// Data members
	//

	bool _ok;
    };



    class KCacheReader: public QObject
    {
	Q_OBJECT

    public:

	/**
	 * Begin reading cache file 'fileName'. The cache file remains open
	 * until this object is destroyed.
	 **/
	KCacheReader( const QString &	fileName,
		      KDirTree *	tree,
		      KDirInfo *	parent = 0 );

	/**
	 * Destructor
	 **/
	virtual ~KCacheReader();

	/**
	 * Read at most maxLines from the cache file (check with eof() if the
	 * end of file is reached yet) or the entire file (if maxLines is 0).
	 *
	 * Returns true if OK and there is more to read, false otherwise.
	 **/
	bool read( int maxLines = 0 );

	/**
	 * Returns true if the end of the cache file is reached (or if there
	 * was an error).
	 **/
	bool eof();

	/**
	 * Returns true if writing the cache file went OK.
	 **/
	bool ok() const { return _ok; }

	/**
	 * Resets the reader so all data lines of the cache can be read with
	 * subsequent read() calls.
	 **/
	void rewind();

	/**
	 * Returns the absolute path of the first directory in this cache file
	 * or an empty string if there is none.
	 *
	 * This method expects the cache file to be just opened without any
	 * previous read() operations on the file. If this is not the case,
	 * call rewind() immediately before firstDir().
	 *
	 * After firstDir(), some records of the cache file will be read.
	 * Make sure to call rewind() if you intend to read from this cache
	 * file afterwards.
	 **/
	QString firstDir();

	/**
	 * Returns the tree associated with this reader.
	 **/
	KDirTree * tree() const { return _tree; }

	/**
	 * Skip leading whitespace from a string.
	 * Returns a pointer to the first character that is non-whitespace.
	 **/
	static char * skipWhiteSpace( char * cptr );

	/**
	 * Find the next whitespace in a string.
	 *
	 * Returns a pointer to the next whitespace character
	 * or a null pointer if there is no more whitespace in the string.
	 **/
	static char * findNextWhiteSpace( char * cptr );

	/**
	 * Remove all trailing whitespace from a string - overwrite it with 0
	 * bytes.
	 *
	 * Returns the new string length.
	 **/
	static void killTrailingWhiteSpace( char * cptr );


    signals:

	/**
	 * Emitted when a child has been added.
	 **/
	void childAdded( KFileInfo *newChild );

	/**
	 * Emitted when reading this cache is finished.
	 **/
	void finished();

	/**
	 * Emitted if there is a read error.
	 **/
	void error();


    protected:

	/**
	 * Check this cache's header (see if it is a KDirStat cache at all)
	 **/
	bool checkHeader();

	/**
	 * Use _fields to add one item to _tree.
	 **/
	void addItem();

	/**
	 * Read the next line that is not empty or a comment and store it in _line.
	 * Returns true if OK, false if error.
	 **/
	bool readLine();

	/**
	 * split the current input line into fields separated by whitespace.
	 **/
	void splitLine();

	/**
	 * Returns the start of field no. 'no' in the current input line
	 * after splitLine().
	 **/
	char * field( int no );

	/**
	 * Returns the number of fields in the current input line after splitLine().
	 **/
	int fieldsCount() const { return _fieldsCount; }


	//
	// Data members
	//

	KDirTree *	_tree;
	gzFile		_cache;
	char		_buffer[ MAX_CACHE_LINE_LEN ];
	char *		_line;
	int		_lineNo;
	QString		_fileName;
	char *		_fields[ MAX_FIELDS_PER_LINE ];
	int		_fieldsCount;
	bool		_ok;
	KDirInfo *	_toplevel;
	KDirInfo * 	_lastDir;
	KDirInfo *	_lastExcludedDir;
	QString		_lastExcludedDirUrl;
    };

}	// namespace KDirStat


#endif // ifndef KDirTreeCache_h


// EOF
