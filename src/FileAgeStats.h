/*
 *   File name: FileAgeStats.h
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileAgeStats_h
#define FileAgeStats_h

#include <QHash>
#include <QList>

#include "FileInfo.h"


namespace QDirStat
{
    class YearStats;
    typedef QHash<short, YearStats>     YearStatsHash;
    typedef QList<short>                YearsList;


    /**
     * Class for calculating and storing file age statistics, i.e. statistics
     * about the years of the last modification times of files in a subtree.
     **/
    class FileAgeStats
    {
    public:

	/**
	 * Constructor. If 'subtree' is non-null, immediately collect data from
	 * that subtree.
	 **/
        FileAgeStats( FileInfo * subtree = 0 );

        /**
         * Destructor.
         **/
        virtual ~FileAgeStats();

        /**
         * Recurse through all file elements in the subtree and calculate the
         * data for that subtree.
         **/
    	void collect( FileInfo * subtree );

        /**
         * Clear all internal data.
         **/
        void clear();

        /**
         * Return a sorted list of the years where files with that modification
         * year were found after collecting data.
         **/
        const YearsList & years() { return _yearsList; }

        /**
         * Return year statistics for the specified year.
         **/
        YearStats yearStats( short year );


    protected:

        /**
         * Recurse through all file elements in the subtree and calculate the
         * data for that subtree.
         **/
    	void collectRecursive( FileInfo * subtree );

        /**
         * Sum up the totals over all years and calculate the percentages for
         * each year
         **/
        void calcPercentages();

        /**
         * Fill the _yearsList with all the years in the _yearStats hash and
         * sort the list.
         **/
        void collectYears();


        //
        // Data Members
        //

        YearStatsHash   _yearStats;
        YearsList       _yearsList;
        int             _totalFilesCount;
        FileSize        _totalFilesSize;

    };  // class FileAgesStats


    /**
     * File modification year statistics for one year
     **/
    class YearStats
    {
    public:

	short		year;
	int		filesCount;
	float		filesPercent;	// 0.0 .. 100.0
	FileSize	size;
	float		sizePercent;	// 0.0 .. 100.0

	YearStats():
            year( 0 ),
            filesCount( 0 ),
            filesPercent( 0.0 ),
            size( 0 ),
            sizePercent( 0.0 )
            {}

    };  // class YearStats

}       // namespace QDirStat


#endif  // FileAgeStats_h
