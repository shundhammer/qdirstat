/*
 *   File name: FileSizeStats.h
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileSizeStats_h
#define FileSizeStats_h

#include "PercentileStats.h"
#include "FileInfo.h"


namespace QDirStat
{
    /**
     * Helper class for extended file size statistics.
     *
     * This collects file size data for trees or subtrees for later use for
     * calculating a median or quantiles or histograms.
     *
     * Notice that one data item (one qreal, i.e. one 64 bit double) is
     * stored for each file (or each matching file) in this object, so this is
     * expensive in terms of memory usage. Also, since data usually need to be
     * sorted for those calculations and sorting has at least logarithmic cost
     * O( n * log(n) ), this also has heavy performance impact.
     **/
    class FileSizeStats: public PercentileStats
    {
    public:

	/**
	 * Constructor. If 'subtree' is non-null, immediately collect data from
	 * that subtree.
	 **/
	FileSizeStats( FileInfo * subtree = 0 );

	/**
	 * Recurse through all file elements in the subtree and append the own
	 * size for each file to the data collection. Notice that the data are
	 * unsorted after this.
	 **/
	void collect( FileInfo * subtree );

	/**
	 * Recurse through all file elements in the subtree and append the own
	 * size for each file with the specified suffix to the data
	 * collection. Notice that the data are unsorted after this.
	 *
	 * 'suffix' should start with ".", e.g. ".jpg".
	 **/
	void collect( FileInfo * subtree, const QString & suffix );

        /**
         * Fill buckets for a histogram from 'startPercentile' to
         * 'endPercentile'.
         **/
        QRealList fillBuckets( int bucketCount,
                               int startPercentile,
                               int endPercentile );
    };

}	// namespace QDirStat


#endif // ifndef FileSizeStats_h

