/*
 *   File name: FileInfoIterators.h
 *   Summary:	Support classes for QDirStat - DirTree iterators
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileInfoIterators_h
#define FileInfoIterators_h


#include <QList>
#include "FileInfo.h"


namespace QDirStat
{
    /**
     * Policies how to treat a "dot entry" for iterator objects.
     * See @ref FileInfoIterator for details.
     **/
    typedef enum
    {
	DotEntryIsSubDir	// Treat dot entry as ordinary subdirectory
    } DotEntryPolicy;



    /**
     * Iterator class for children of a @ref FileInfo object. For optimum
     * performance, this iterator class does NOT return children in any
     * specific sort order.
     *
     * Sample usage:
     *
     *	  FileInfoIterator it( node, DotEntryTransparent );
     *
     *	  while ( *it )
     *	  {
     *	     logDebug() << *it << ":\t" << (*it)->totalSize() << endl;
     *	     ++it;
     *	  }
     *
     * This will output the URL (path+name) and the total size of each (direct)
     * subdirectory child and each (direct) file child of 'node'.
     * Notice: This does not recurse into subdirectories!
     *
     * @short (unsorted) iterator for @ref FileInfo children.
     **/
    class FileInfoIterator
    {
    public:
	/**
	 * Constructor: Initialize an iterator object to iterate over the
	 * children of 'parent' (unsorted!). The dot entry is treated as a
	 * subdirectory.
	 **/
	FileInfoIterator( FileInfo * parent );

    protected:
	/**
	 * Alternate constructor to be called from derived classes: Those can
	 * choose not to call next() in the constructor.
	 **/
	FileInfoIterator( FileInfo * parent,
			  bool	     callNext );

    private:
	/**
	 * Internal initialization called from any constructor.
	 **/
	void init( FileInfo * parent,
		   bool	      callNext );

    public:

	/**
	 * Destructor.
	 **/
	virtual ~FileInfoIterator();

	/**
	 * Return the current child object or 0 if there is no more.
	 * Same as @ref operator*() .
	 **/
	virtual FileInfo * current() { return _current; }

	/**
	 * Return the current child object or 0 if there is no more.
	 * Same as @ref current().
	 **/
	FileInfo * operator*() { return current(); }

	/**
	 * Advance to the next child. Same as @ref operator++().
	 **/
	virtual void next();

	/**
	 * Advance to the next child. Same as @ref next().
	 **/
	void operator++() { next(); }

	/**
	 * Returns 'true' if this iterator is finished and 'false' if not.
	 **/
	virtual bool finished() { return _current == 0; }

	/**
	 * Return the number of items that will be processed.
	 * This is an expensive operation.
	 **/
	int count();


    protected:

	FileInfo *	_parent;
	FileInfo *	_current;
	bool		_directChildrenProcessed;
	bool		_dotEntryProcessed;

    };	// class FileInfoIterator

} // namespace QDirStat


#endif // ifndef FileInfoIterators_h

