/*
 *   File name: HistogramView.h
 *   Summary:	View widget for histogram rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef HistogramView_h
#define HistogramView_h


#include <QGraphicsView>
#include <QList>

#define MAX_BUCKET_COUNT 100


typedef QList<qreal> QRealList;


namespace QDirStat
{
    /**
     * Histogram widget.
     *
     * This widget is based on buckets and percentiles, both of which have to
     * be fed from the outside, i.e. the data collection is abstracted.
     *
     * The histogram can be displayed in a traditional way, i.e. from the
     * minimum data value (percentile 0 or P0) to the maximum data value
     * (percentile 100 or P100). But in many cases, this greatly distorts the
     * display because of outliers (data points way outside the range of
     * "normal" data points), so this histogram can also display from a given
     * percentile (startPercentile) to another given percentile
     * (endPercentile).
     *
     * In many cases, those outliers are only a very small percentage, so
     * displaying not from P0..P100, but from P3..P97 instead gives a much more
     * meaningful histogram, while still displaying 96% of all data: By
     * definition, each percentile contains 1% of all data points, so you lose
     * one percent at the left and one percent at the right for each percentile
     * omitted like this.
     *
     * That "cutoff" should be clearly communicated to the user so he does not
     * get the impression that this histogram in that mode displays all data;
     * it does not. It does display the meaningful part of the data, though.
     *
     * In addition to that, the percentiles (or at least every 5th of them,
     * depending on configuration) as well as the median, the 1st and the 3rd
     * quartile (Q1 and Q3) can be displayed as an overlay to the histogram.
     **/
    class HistogramView: public QGraphicsView
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 **/
	HistogramView( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~HistogramView();

        /**
         * Clear all data and all displayed graphics.
         **/
        void clear();

        /**
         * Calculate the best bucket count according to the "Rice Rule" for n
         * data points, but limited to MAX_BUCKET_COUNT.
         *
         * See also https://en.wikipedia.org/wiki/Histogram
         **/
        static qreal bestBucketCount( int n );

        /**
         * Calculate the bucket width from min to max for 'bucketCount'
         * buckets.
         **/
        static qreal bucketWidth( qreal min, qreal max, int bucketCount );

        /**
         * Set the data as "buckets". Each bucket contains the number of data
         * points (not their value!) from one interval of bucketWidth width.
         *
         * The type of the buckets is qreal even though by mathematical
         * definition it should be int, but QGraphicsView uses qreal
         * everywhere, so this is intended to minimize the hassle converting
         * back and forth. The bucket values can safely be converted to int
         * with no loss of precision.
         **/
        void setBuckets( const QRealList & newBuckets );

        /**
         * Set the percentiles for the data points all at once. Unlike the
         * buckets, these have a value; in the context of QDirStat, this is the
         * FileSize.
         *
         * The definition of a percentile n is "the data value where n percent
         * of all sorted data are taken into account". The median is the 50th
         * percentile. By a little stretch of the definition, percentile 0 is
         * the data minimum, percentile 100 is the data maximum.
         *
         * The inverval between one percentile and the next contains exactly 1%
         * of the data points.
         **/
        void setPercentiles( const QRealList & newPercentiles );

        /**
         * Set one percentile value. If you use that, make sure you iterate
         * over all of them (0..100) to set them all.
         **/
        void setPercentile( int index, qreal value );

        /**
         * Return the stored value for percentile no. 'index' (0..100).
         **/
        qreal percentile( int index ) const;

        /**
         * Set the percentile (0..100) from which on to display data, i.e. set
         * the left border of the histogram. The real value to use is taken
         * from the stored percentiles.
         **/
        void setStartPercentile( int index );

        /**
         * Set the percentile (0..100) until which on to display data, i.e. set
         * the right border of the histogram. The real value to use is taken
         * from the stored percentiles.
         **/
        void setEndPercentile( int index );

        /**
         * Return the percentile from which on to display data, i.e. the left
         * border of the histogram. Use percentile() with the result of this to
         * get the numeric value.
         **/
        int startPercentile() const { return _startPercentile; }

        /**
         * Return the percentile until which on to display data, i.e. the right
         * border of the histogram. Use percentile() with the result of this to
         * get the numeric value.
         **/
        int endPercentile() const { return _endPercentile; }

        /**
         * Return the current number of data buckets, i.e. the number of
         * histogram bars.
         **/
        int bucketCount() const { return _buckets.size(); }

        /**
         * Return the number of data points in bucket no. 'index'.
         **/
        qreal bucket( int index ) const;

        /**
         * Return the width of a bucket. All buckets have the same width.
         *
         * Notice that this value can only be obtained after all relevant data
         * are set: buckets, percentiles, startPercentile, endPercentile.
         **/
        qreal bucketWidth() const;


    public slots:

        /**
         * Display or redisplay the histogram based on the current data.
         **/
        void redisplay();

        /**
         * Enable or disable showing the median (percentile 50) as an overlay
         * over the histogram.
         **/
        void showMedian( bool show = true ) { _showMedian = show; }

        /**
         * Enable or disable showing the 1st and 3rd quartiles (Q1 and Q3,
         * percentile 25 and 75, respectively) as an overlay over the
         * histogram.
         **/
        void showQuartiles( bool show = true ) { _showQuartiles = show; }

        /**
         * Enable or disable showing percentiles as an overlay over the
         * histogram. 'step' specifies how many of them to display; with the
         * default '5' it will display P5, P10, P15 etc.; step = 0 disables
         * them completely.
         **/
        void showPercentiles( int step = 5 ) { _percentileStep = step; }

        /**
         * Set how many percentiles to display as an overlay at the margins (at
         * the left and at the right border) in addition to those shown with
         * showPercentiles().
         *
         * A value of 2 with a histogram showing data from min to max means
         * show also P1, P2, P98 and P99.
         *
         * A value of 2 with a histogram showing data from P3 to P97 means show
         * also P4, P5, P95 and P96.
         *
         * A value of 0 means show no additional percentiles.
         **/
        void showMarginPercentiles( int margin = 2 ) { _marginPercentiles = margin; }


    protected:

        void init();


        //
        // Data members
        //

        QRealList _buckets;
        QRealList _percentiles;
        qreal     _bucketMaxValue;

        int       _startPercentile;
        int       _endPercentile;

        bool      _showMedian;
        bool      _showQuartiles;
        int       _percentileStep;
        int       _marginPercentiles;

        QBrush    _barBrush;
        QPen      _barPen;

        QPen      _medianPen;
        QPen      _quartilePen;
        QPen      _percentilePen;
        QPen      _decilePen;
    };

}	// namespace QDirStat


#endif // ifndef HistogramView_h
