/*
 *   File name: DirTreeIterators.h
 *   Summary:	Support classes for QDirStat - DirTree iterators
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirTreeIterators_h
#define DirTreeIterators_h


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
	DotEntryTransparent,	// Flatten hierarchy - move dot entry children up
	DotEntryAsSubDir,	// Treat dot entry as ordinary subdirectory
	DotEntryIgnore		// Ignore dot entry and its children completely
    } DotEntryPolicy;



    /**
     * Iterator class for children of a @ref FileInfo object. For optimum
     * performance, this iterator class does NOT return children in any
     * specific sort order. If you need that, use @ref FileInfoSortedIterator
     * instead.
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
	 * children of 'parent' (unsorted!), depending on 'dotEntryPolicy':
	 *
	 * DotEntryTransparent (default):
	 *
	 * Treat the dot entry as if it wasn't there - pretend to move all its
	 * children up to the real parent. This makes a directory look very
	 * much like the directory on disk, without the dot entry.  'current()'
	 * or 'operator*()' will never return the dot entry, but all of its
	 * children. Subdirectories will be processed before any file children.
	 *
	 * DotEntryIsSubDir:
	 *
	 * Treat the dot entry just like any other subdirectory. Don't iterate
	 * over its children, too (unlike DotEntryTransparent above).
	 * 'current()' or 'operator*()' will return the dot entry, but none of
	 * its children (unless, of course, you create an iterator with the dot
	 * entry as the parent).
	 *
	 * DotEntryIgnore:
	 *
	 * Ignore the dot entry and its children completely. Useful if children
	 * other than subdirectories are not interesting anyway. 'current()'
	 * or 'operator*()' will never return the dot entry nor any of its
	 * children.
	 *
	 **/
	FileInfoIterator( FileInfo *	 parent,
			  DotEntryPolicy dotEntryPolicy = DotEntryTransparent );

    protected:
	/**
	 * Alternate constructor to be called from derived classes: Those can
	 * choose not to call next() in the constructor.
	 **/
	FileInfoIterator( FileInfo *	 parent,
			  DotEntryPolicy dotEntryPolicy,
			  bool		 callNext );

    private:
	/**
	 * Internal initialization called from any constructor.
	 **/
	void init( FileInfo *	  parent,
		   DotEntryPolicy dotEntryPolicy,
		   bool		  callNext );

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
	 * Check whether or not the current child is a directory, i.e. can be
	 * cast to @ref DirInfo * .
	 **/
	bool currentIsDir() { return _current && _current->isDirInfo(); }

	/**
	 * Return the current child object cast to @ref DirInfo * or 0 if
	 * there either is no more or it isn't a directory. Check with @ref
	 * currentIsDir() before using this!
	 **/
	DirInfo * currentDir() { return currentIsDir() ? (DirInfo *) _current : 0; }

	/**
	 * Return the number of items that will be processed.
	 * This is an expensive operation.
	 **/
	int count();


    protected:

	FileInfo *	_parent;
	DotEntryPolicy	_policy;
	FileInfo *	_current;
	bool		_directChildrenProcessed;
	bool		_dotEntryProcessed;
	bool		_dotEntryChildrenProcessed;

    };	// class FileInfoIterator

} // namespace QDirStat


#endif // ifndef DirTreeIterators_h

