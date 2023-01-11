/*
 *   File name: PercentileStats.h
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef PercentileStats_h
#define PercentileStats_h

#include <QList>


typedef QList<qreal> QRealList;


namespace QDirStat
{
    class PercentileSums;

    /**
     * Base class for percentile-related statistics calculation.
     *
     * Derived classes have to make sure to populate the internal 'data' list.
     * Typically, some kind of collect() method is used for that.
     *
     * Notice that one data item (one qreal, i.e. one 64 bit double) is
     * stored for each file (or each matching file) in this object, so this is
     * expensive in terms of memory usage. Also, since data usually need to be
     * sorted for those calculations and sorting has at least logarithmic cost
     * O( n * log(n) ), this also has heavy performance impact.
     **/
    class PercentileStats
    {
    public:
	/**
	 * Constructor.
	 **/
	PercentileStats();

        /**
         * Destructor.
         **/
        virtual ~PercentileStats();

	/**
	 * Clear the collected data and shrink the list.
	 **/
	void clear();

	/**
	 * Sort the collected data in ascending order.
	 * This is necessary after all collect() calls.
         *
	 * The functions accessing results like min(), max(), median(),
	 * quantile(), percentile() etc. all implicitly sort the data if they
	 * are not sorted yet.
	 **/
	void sort();

        /**
         * Return the size of the collected data, i.e. the number of data
         * points.
         **/
        int dataSize() const { return _data.size(); }

	/**
	 * Return a reference to the collected data.
	 **/
	QRealList & data() { return _data; }


	// All calculation functions below will sort the internal data first if
	// they are not sorted yet. This is why they are not const.

	/**
	 * Calculate the median.
	 **/
	qreal median();

        /**
         * Calculate the arithmetic average based on the collected data.
         *
         * Notice that this is probably the most expensive way of doing this:
         * The FileInfo class already collected sums and counts during
         * directory reading that might also be used.
         **/
        qreal average();

	/**
	 * Find the minimum value.
	 **/
	qreal min();

	/**
	 * Find the maximum value.
	 **/
	qreal max();

	/**
	 * Calculate a quantile: Find the quantile no. 'number' of order
	 * 'order'.
	 *
	 * The median is quantile( 2, 1 ), the minimum is quantile( 2, 0 ), the
	 * maximum is quantile( 2, 2 ). The first quartile is quantile( 4, 1 ),
	 * the first percentile is quantile( 100, 1 ).
	 **/
	qreal quantile( int order, int number );

        /**
         * Calculate a percentile.
         **/
        qreal percentile( int number ) { return quantile( 100, number ); }

        /**
         * Calculate a quartile.
         **/
        qreal quartile( int number ) { return quantile( 4, number ); }

        /**
         * Return a list of all percentiles from 0 to 100.
         **/
        QRealList percentileList();

        /**
         * Returns both forms of percentile sums.
         **/
        PercentileSums percentileSums();


    protected:

	QRealList _data;
	bool	  _sorted;   
    };

    /**
     * Container for individual and cumulative percentile sums.
     **/
    class PercentileSums
    {
    public:
        /**
         * Constructor.
         **/
        PercentileSums() {}

        /**
         * Returns the size of the sums.
         **/
        int size() const { return _individual.size(); }

        /**
         * Returns true if the sums are empty; otherwise, returns false.
         **/
        bool isEmpty() const { return _individual.isEmpty(); }

        /**
         * Return a lists (0..100) of all accumulated sizes between one percentile
         * and the previous one
         **/
        const QRealList & individual() const { return _individual; }

        /**
         * Return a lists (0..100) of all accumulated sizes between one percentile
         * and the first.
         **/
        const QRealList & cumulative() const { return _cumulative; }

    private:
        QRealList _individual;
        QRealList _cumulative;

        friend PercentileSums PercentileStats::percentileSums();
    };

}	// namespace QDirStat


#endif // ifndef PercentileStats_h

