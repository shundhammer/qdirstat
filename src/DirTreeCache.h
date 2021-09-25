/*
 *   File name: DirTreeCache.h
 *   Summary:	QDirStat cache reader / writer
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirTreeCache_h
#define DirTreeCache_h


#include <stdio.h>
#include <zlib.h>
#include "DirTree.h"

#define DEFAULT_CACHE_NAME	".qdirstat.cache.gz"
#define CACHE_FORMAT_VERSION	"1.0"
#define MAX_CACHE_LINE_LEN	1024
#define MAX_FIELDS_PER_LINE	32


namespace QDirStat
{
    class CacheWriter
    {
    public:

	/**
	 * Write 'tree' to file 'fileName' in gzip format (using zlib).
	 *
	 * Check CacheWriter::ok() to see if writing the cache file went OK.
	 **/
	CacheWriter( const QString & fileName, DirTree *tree );

	/**
	 * Destructor
	 **/
	virtual ~CacheWriter();

	/**
	 * Returns true if writing the cache file went OK.
	 **/
	bool ok() const { return _ok; }

	/**
	 * Format a file size as string - with trailing "G", "M", "K" for
	 * "Gigabytes", "Megabytes, "Kilobytes", respectively (provided there
	 * is no fractional part - 27M is OK, 27.2M is not).
	 **/
	QString formatSize( FileSize size );


    protected:

	/**
	 * Write cache file in gzip format.
	 * Returns 'true' if OK, 'false' upon error.
	 **/
	bool writeCache( const QString & fileName, DirTree *tree );

	/**
	 * Write 'item' recursively to cache file 'cache'.
	 * Uses zlib to write gzip-compressed files.
	 **/
	void writeTree( gzFile cache, FileInfo * item );

	/**
	 * Write 'item' to cache file 'cache' without recursion.
	 * Uses zlib to write gzip-compressed files.
	 **/
	void writeItem( gzFile cache, FileInfo * item );

        /**
         * Return the 'path' in an URL-encoded form, i.e. with some special
         * characters escaped in percent notation (" " -> "%20").
         **/
        QByteArray urlEncoded( const QString & path );

	//
	// Data members
	//

	bool _ok;
    };



    class CacheReader: public QObject
    {
	Q_OBJECT

    public:

	/**
	 * Begin reading cache file 'fileName'. The cache file remains open
	 * until this object is destroyed.
	 **/
	CacheReader( const QString & fileName,
		     DirTree	   * tree,
		     DirInfo	   * parent = 0 );

	/**
	 * Destructor
	 **/
	virtual ~CacheReader();

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
	DirTree * tree() const { return _tree; }

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
	void childAdded( FileInfo *newChild );

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
	 * Check this cache's header (see if it is a QDirStat cache at all)
	 **/
	bool checkHeader();

	/**
	 * Use _fields to add one item to _tree.
	 **/
	void addItem();

	/**
	 * Read the next line that is not empty or a comment and store it in
	 * _line.
         *
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
	 * Split up a file name with path into its path and its name component
	 * and return them in path_ret and name_ret, respectively.
	 *
	 * Example:
	 *     "/some/dir/somewhere/myfile.obj"
	 * ->  "/some/dir/somewhere", "myfile.obj"
	 **/
	void splitPath( const QString & fileNameWithPath,
			QString	      & path_ret,
			QString	      & name_ret ) const;

	/**
	 * Build a full path from path + file name (without path).
	 **/
	QString buildPath( const QString & path, const QString & name ) const;

	/**
	 * Return an unescaped version of 'rawPath'.
	 **/
	QString unescapedPath( const QString & rawPath ) const;

	/**
	 * Clean a path: Replace duplicate (or triplicate or more) slashes with
	 * just one. QUrl doesn't seem to handle those well.
	 **/
	QString cleanPath( const QString & rawPath ) const;

	/**
	 * Returns the number of fields in the current input line after
	 * splitLine().
	 **/
	int fieldsCount() const { return _fieldsCount; }

	/**
	 * Recursively set the read status of all dirs from 'dir' on, send tree
	 * signals and finalize local (i.e. clean up empty or unneeded dot
	 * entries).
	 **/
	void finalizeRecursive( DirInfo * dir );

        /**
         * Cascade a read error up to the toplevel directory node read by this
         * cache file.
         **/
        void setReadError( DirInfo * dir );


	//
	// Data members
	//

	DirTree *	_tree;
	gzFile		_cache;
	char		_buffer[ MAX_CACHE_LINE_LEN ];
	char *		_line;
	int		_lineNo;
	QString		_fileName;
	char *		_fields[ MAX_FIELDS_PER_LINE ];
	int		_fieldsCount;
	bool		_ok;
        int             _errorCount;
	DirInfo *	_toplevel;
	DirInfo *	_lastDir;
	DirInfo *	_lastExcludedDir;
	QString		_lastExcludedDirUrl;
        QRegExp         _multiSlash;
    };

}	// namespace QDirStat


#endif // ifndef DirTreeCache_h

