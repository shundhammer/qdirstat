/*
 *   File name:	kdirtreeiterators.h
 *   Summary:	Support classes for QDirStat - DirTree iterators
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirTreeIterators_h
#define DirTreeIterators_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "kdirtree.h"


namespace QDirStat
{
    /**
     * Policies how to treat a "dot entry" for iterator objects.
     * See @ref FileInfoIterator for details.
     **/
    typedef enum
    {
	KDotEntryTransparent,	// Flatten hierarchy - move dot entry children up
	KDotEntryAsSubDir,	// Treat dot entry as ordinary subdirectory
	KDotEntryIgnore		// Ignore dot entry and its children completely
    } KDotEntryPolicy;


    typedef enum
    {
	KUnsorted,
	KSortByName,
	KSortByTotalSize,
	KSortByLatestMtime
    } FileInfoSortOrder;


    // Forward declarations
    class FileInfoList;


    /**
     * Iterator class for children of a @ref FileInfo object. For optimum
     * performance, this iterator class does NOT return children in any
     * specific sort order. If you need that, use @ref FileInfoSortedIterator
     * instead.
     *
     * Sample usage:
     *
     *    FileInfoIterator it( node, KDotEntryTransparent );
     *
     *    while ( *it )
     *    {
     *       logDebug() << *it << ":\t" << (*it)->totalSize() << endl;
     *       ++it;
     *    }
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
	 * KDotEntryTransparent (default):
	 *
	 * Treat the dot entry as if it wasn't there - pretend to move all its
	 * children up to the real parent. This makes a directory look very
	 * much like the directory on disk, without the dot entry.  'current()'
	 * or 'operator*()' will never return the dot entry, but all of its
	 * children. Subdirectories will be processed before any file children.
	 *
	 * KDotEntryIsSubDir:
	 *
	 * Treat the dot entry just like any other subdirectory. Don't iterate
	 * over its children, too (unlike KDotEntryTransparent above).
	 * 'current()' or 'operator*()' will return the dot entry, but none of
	 * its children (unless, of course, you create an iterator with the dot
	 * entry as the parent).
	 *
	 * KDotEntryIgnore:
	 *
	 * Ignore the dot entry and its children completely. Useful if children
	 * other than subdirectories are not interesting anyway. 'current()'
	 * or 'operator*()' will never return the dot entry nor any of its
	 * children.
	 *
	 **/
	FileInfoIterator( FileInfo *		parent,
			   KDotEntryPolicy	dotEntryPolicy = KDotEntryTransparent );

    protected:
	/**
	 * Alternate constructor to be called from derived classes: Those can
	 * choose not to call next() in the constructor.
	 **/
	FileInfoIterator	( FileInfo *		parent,
				  KDotEntryPolicy	dotEntryPolicy,
				  bool			callNext );

    private:
	/**
	 * Internal initialization called from any constructor.
	 **/
	void init		( FileInfo *		parent,
				  KDotEntryPolicy	dotEntryPolicy,
				  bool			callNext );

    public:

	/**
	 * Destructor.
	 **/
	virtual 	~FileInfoIterator();

	/**
	 * Return the current child object or 0 if there is no more.
	 * Same as @ref operator*() .
	 **/
	virtual FileInfo * current()	{ return _current; }

	/**
	 * Return the current child object or 0 if there is no more.
	 * Same as @ref current().
	 **/
	FileInfo *	operator*()	{ return current(); }

	/**
	 * Advance to the next child. Same as @ref operator++().
	 **/
	virtual void	next();

	/**
	 * Advance to the next child. Same as @ref next().
	 **/
	void		operator++()	{ next(); }

	/**
	 * Returns 'true' if this iterator is finished and 'false' if not.
	 **/
	virtual bool	finished()	{ return _current == 0; }

	/**
	 * Check whether or not the current child is a directory, i.e. can be
	 * cast to @ref DirInfo * .
	 **/
	bool		currentIsDir() { return _current && _current->isDirInfo(); }

	/**
	 * Return the current child object cast to @ref DirInfo * or 0 if
	 * there either is no more or it isn't a directory. Check with @ref
	 * currentIsDir() before using this!
	 **/
	DirInfo *	currentDir() { return currentIsDir() ? (KDirInfo *) _current : 0; }

	/**
	 * Return the number of items that will be processed.
	 * This is an expensive operation.
	 **/
	int		count();


    protected:

	FileInfo *	_parent;
	KDotEntryPolicy	_policy;
	FileInfo *	_current;
	bool		_directChildrenProcessed;
	bool		_dotEntryProcessed;
	bool		_dotEntryChildrenProcessed;

    };	// class FileInfoIterator



    /**
     * Iterator class for children of a @ref FileInfo object. This iterator
     * returns children sorted by name: Subdirectories first, then the dot
     * entry (if desired - depending on policy), then file children (if
     * desired). Note: If you don't need the sorting feature, you might want to
     * use @ref KFileItemIterator instead which has better performance.
     *
     * @short sorted iterator for @ref FileInfo children.
     **/
    class FileInfoSortedIterator: public FileInfoIterator
    {
    public:
	/**
	 * Constructor. Specify the sorting order with 'sortOrder' and 'ascending'.
	 * See @ref FileInfoIterator for more details.
	 **/
	FileInfoSortedIterator( FileInfo *		parent,
				 KDotEntryPolicy	dotEntryPolicy	= KDotEntryTransparent,
				 FileInfoSortOrder	sortOrder	= KSortByName,
				 bool			ascending	= true );
	/**
	 * Destructor.
	 **/
	virtual ~FileInfoSortedIterator();

	/**
	 * Return the current child object or 0 if there is no more.
	 *
	 * Inherited from @ref FileInfoIterator.
	 * Overwritten to overcome some shortcomings of C++:
	 * Virtual methods cannot be used in the constructor.
	 **/
	virtual FileInfo * current();

	/**
	 * Advance to the next child. Same as @ref operator++().
	 * Sort by name, sub directories first, then the dot entry (if
	 * desired), then files (if desired).
	 *
	 * Inherited from @ref FileInfoIterator.
	 **/
	virtual void next();

	/**
	 * Returns 'true' if this iterator is finished and 'false' if not.
	 *
	 * Inherited from @ref FileInfoIterator.
	 **/
	virtual bool finished();


    protected:

	/**
	 * Delayed initialization for class parts that rely on availability of
	 * virtual methods. This is a kludge to overcome a major shortcoming of
	 * C++: Virtual methods are not available in the constructor yet.
	 * This is a neverending cause of trouble.
	 **/
	void delayedInit();

	/**
	 * Make a 'default order' children list:
	 * First all subdirectories sorted by name,
	 * then the dot entry (depending on policy),
	 * then the dot entry's children (depending on policy).
	 **/
	virtual void makeDefaultOrderChildrenList();

	/**
	 * Make a sorted children list according to the current sort
	 * criteria - unless KSortByName is requested, in which case
	 * makeDefaultOrderChildrenList() above is used.
	 **/
	virtual void makeChildrenList();

	
	// Data members

	FileInfoList *		_childrenList;
	FileInfoSortOrder	_sortOrder;
	bool			_ascending;
	bool			_initComplete;

    };	// class FileInfoSortedIterator



    /**
     * Specialized FileInfo iterator that sorts by (total) size, yet
     * disregards children below a minimum size. This can considerably improve
     * performance if the number of children that need to be sorted decreases
     * dramatically.
     *
     * For example, treemaps can only display a limited portion of large
     * directory trees since the number of available pixels is very
     * limited. Thus, files (or directories) below a certain size usually don't
     * get a individual visual representation anyway, so they may as well be
     * omitted right away - no need for expensive list sorting operations.
     **/
    class FileInfoSortedBySizeIterator: public FileInfoSortedIterator
    {
    public:

	/**
	 * Constructor. Children below 'minSize' will be ignored by this iterator.
	 **/
	FileInfoSortedBySizeIterator( FileInfo *	parent,
				       KFileSize	minSize		= 0,
				       KDotEntryPolicy	dotEntryPolicy	= KDotEntryTransparent,
				       bool		ascending 	= false );

	/**
	 * Destructor.
	 **/
	virtual ~FileInfoSortedBySizeIterator() {};


    protected:

	/**
	 * Create the (sorted) children list. Disregard children below minSize.
	 * Reimplemented from FileInfoSortedIterator.
	 **/
	virtual void makeChildrenList();


	// Data members
	
	KFileSize	_minSize;

    }; // class FileInfoSortedBySizeIterator



    /**
     * Internal helper class for sorting iterators.
     **/
    class FileInfoList: public QPtrList<FileInfo>
    {
    public:

	/**
	 * Constructor.
	 **/
	FileInfoList( FileInfoSortOrder sortOrder = KSortByName,
		       bool ascending = true );

	/**
	 * Destructor.
	 **/
	virtual ~FileInfoList();

	/**
	 * Returns the sum of all the total sizes in the list.
	 **/
	KFileSize sumTotalSizes();

	
    protected:
	/**
	 * Comparison function. This is why this class is needed at all.
	 **/
	virtual int compareItems( QCollection::Item it1, QCollection::Item it2 );

	FileInfoSortOrder 	_sortOrder;
	bool			_ascending;
    };


    typedef QPtrListIterator<FileInfo> FileInfoListIterator;



    //----------------------------------------------------------------------
    //			       Static Functions
    //----------------------------------------------------------------------

    /**
     * Generic comparison function as expected by all kinds of sorting etc.
     * algorithms. Requires operator<() and operator==() to be defined for this
     * class.
     **/
    template<class T>
    inline int compare( T val1, T val2 )
    {
	if	( val1 <  val2 )	return -1;
	else if	( val1 == val2 ) 	return  0;
	else 				return  1;
    }

}	// namespace QDirStat


#endif // ifndef DirTreeIterators_h


// EOF
