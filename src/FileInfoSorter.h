/*
 *   File name: FileInfoSorter.h
 *   Summary:	Functor to handle sorting FileInfo objects
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef FileInfoSorter_h
#define FileInfoSorter_h


#include "FileInfo.h"
#include "DataColumns.h"


namespace QDirStat
{
    /**
     * Functor class for sorting FileInfo objects with C++ STL sorting
     * algorithms like std::sort(), std::stable_sort().
     *
     * Those STL algorithms expect two iterators and either a sort function
     * that works like a normal operator<() or a functor object that has an
     * overloaded operator() that will then be called with the two objects to
     * compare. The latter is what this class provides.
     *
     * Example:
     *
     *	   FileInfoList childrenList;
     *	   std::sort( childrenList.begin(),
     *		      childrenList.end(),
     *		      FileInfoSorter( LatestMtimeCol, Qt::DescendingOrder ) );
     *
     * For each element pair to compare, the FileInfoSorter's operator() will
     * be called with that pair as arguments.
     **/
    class FileInfoSorter
    {
    public:
	/**
	 * Constructor. This sets the sort column and sort order that will be
	 * used in subsequent calls.
	 **/
	FileInfoSorter( DataColumn sortCol, Qt::SortOrder sortOrder ):
	    _sortCol( sortCol ),
	    _sortOrder( sortOrder )
	    {}

	/**
	 * Overloaded operator() that does the comparison.
	 * returns 'true' if a < b, false otherwise (i.e., if a >= b).
	 **/
	bool operator() ( FileInfo * a, FileInfo * b );

    private:
	DataColumn    _sortCol;
	Qt::SortOrder _sortOrder;

    };	   // class FileInfoSorter

}      // namespace QDirStat

#endif // FileInfoSorter_h
