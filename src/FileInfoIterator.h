/*
 *   File name: FileInfoIterator.h
 *   Summary:	Support classes for QDirStat - DirTree iterators
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileInfoIterator_h
#define FileInfoIterator_h


#include <QList>
#include "FileInfo.h"


namespace QDirStat
{
    /**
     * Iterator class for children of a FileInfo object. For optimum
     * performance, this iterator class does NOT return children in any
     * specific sort order.
     *
     * Sample usage:
     *
     *	  FileInfoIterator it( node );
     *
     *	  while ( *it )
     *	  {
     *	     logDebug() << *it << ":\t" << (*it)->totalSize() << endl;
     *	     ++it;
     *	  }
     *
     * This will output the URL (path+name) and the total size of each (direct)
     * subdirectory child and each (direct) file child of 'node'.
     *
     * Notice: This does not recurse into subdirectories, and the dot entry is
     * treated just like a subdirectory!
     *
     * @short (unsorted) iterator for FileInfo children.
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
	 * Return the current child object or 0 if there is no more.
	 * Same as operator*() .
	 **/
	FileInfo * current() { return _current; }

	/**
	 * Return the current child object or 0 if there is no more.
	 * Same as current().
	 **/
	FileInfo * operator*() { return current(); }

	/**
	 * Advance to the next child. Same as operator++().
	 **/
	void next();

	/**
	 * Advance to the next child. Same as next().
	 **/
	void operator++() { next(); }

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


    class FileInfoSortedBySizeIterator
    {
    public:

	/**
	 * Constructor. Children below 'minSize' will be ignored by this
	 * iterator.
	 **/
	FileInfoSortedBySizeIterator( FileInfo	    * parent,
				      FileSize	      minSize	= 0,
				      Qt::SortOrder   sortOrder = Qt::DescendingOrder );

	/**
	 * Return the current child object or 0 if there is no more.
	 * Same as operator*() .
	 **/
	FileInfo * current();

	/**
	 * Return the current child object or 0 if there is no more.
	 * Same as current().
	 **/
	FileInfo * operator*() { return current(); }

	/**
	 * Advance to the next child. Same as operator++().
	 **/
	void next();

	/**
	 * Advance to the next child. Same as next().
	 **/
	void operator++() { next(); }

	/**
	 * Return the number of items that will be processed.
	 **/
	int count() { return _sortedChildren.size(); }

    protected:

	FileInfoList _sortedChildren;
	int	     _currentIndex;
    }; //

} // namespace QDirStat


#endif // ifndef FileInfoIterator_h

