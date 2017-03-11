/*
 *   File name: FileSizeStats.h
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileSizeStats_h
#define FileSizeStats_h

#include "FileInfo.h"


namespace QDirStat
{
    class DirTree;
    typedef QList<FileSize> FileSizeList;

    /**
     * Helper class for extended file size statistics.
     *
     * This collects file size data for trees or subtrees for later use for
     * calculating a median or quantiles or histograms.
     *
     * Notice that one data item (one FileSize, i.e. one 64 bit long long) is
     * stored for each file (or each matching file) in this object, so this is
     * expensive in terms of memory usage. Also, since data usually need to be
     * sorted for those calculations and sorting has at least logarithmic cost
     * O( n * log(n) ), this also has heavy performance impact.
     **/
    class FileSizeStats
    {
    public:

	/**
	 * Constructor.
	 **/
	FileSizeStats();

	/**
	 * Clear the collected data and shrink the list.
	 **/
	void clear();

	/**
	 * Recurse through all file elements in the tree and append the own
	 * size for each file to the data collection. Notice that the data are
	 * unsorted after this.
	 **/
	void collect( DirTree * tree );

	/**
	 * Recurse through all file elements in the subtree and append the own
	 * size for each file to the data collection. Notice that the data are
	 * unsorted after this.
	 **/
	void collect( FileInfo * subtree );

	/**
	 * Recurse through all file elements in the tree and append the own
	 * size for each file with the specified suffix to the data
	 * collection. Notice that the data are unsorted after this.
	 *
	 * 'suffix' should start with ".", e.g. ".jpg".
	 **/
	void collect( DirTree * tree, const QString & suffix );

	/**
	 * Recurse through all file elements in the subtree and append the own
	 * size for each file with the specified suffix to the data
	 * collection. Notice that the data are unsorted after this.
	 *
	 * 'suffix' should start with ".", e.g. ".jpg".
	 **/
	void collect( FileInfo * subtree, const QString & suffix );

	/**
	 * Sort the collected data in ascending order.
	 * This is necessary after all collect() calls.
	 * The
	 **/
	void sort();

	/**
	 * Return a reference to the collected data.
	 **/
	FileSizeList & data() { return _data; }


	// All calculation functions below will sort the internal data first if
	// they are not sorted yet. This is why they are not const.

	/**
	 * Calculate the median.
	 **/
	FileSize median();

	/**
	 * Find the minimum value.
	 **/
	FileSize min();

	/**
	 * Find the maximum value.
	 **/
	FileSize max();

	/**
	 * Calculate a quantile: Find the quantile no. 'quantileNumber' of
	 * order 'quantileOrder'.
	 *
	 * The median is quantile( 2, 1 ), the minimum is quantile( 2, 0 ), the
	 * maximum is quantile( 2, 2 ). The first quartile is quantile( 4, 1 ),
	 * the first percentile is quantile( 100, 1 ).
	 **/
	FileSize quantile( int quantileOrder, int quantileNumber );

    protected:

	FileSizeList _data;
	bool	     _sorted;
    };

}	// namespace QDirStat


#endif // ifndef FileSizeStats_h

