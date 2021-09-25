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


class QGraphicsSceneMouseEvent;

typedef QList<qreal> QRealList;


namespace QDirStat
{
    class DelayedRebuilder;

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
         * zValue (altitude) for the different graphics elements
         **/
        enum GraphicsItemLayers
        {
            PanelBackgroundLayer = -100,
            MiscLayer = 0, // Default if no zValue specified
            InvisibleBarLayer,
            BarLayer,
            AxisLayer,
            MarkerLayer,
            SpecialMarkerLayer,
            TextLayer
        };


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
	 * Set the percentiles for the data points all at once. Unlike the
	 * buckets, these have a value; in the context of QDirStat, this is the
	 * FileSize.
	 *
	 * The definition of a percentile n is "the data value where n percent
	 * of all sorted data are taken into account". The median is the 50th
	 * percentile. By a little stretch of the definition, percentile 0 is
	 * the data minimum, percentile 100 is the data maximum.
	 *
	 * The interval between one percentile and the next contains exactly 1%
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
	 * Return the percentile from which on to display data, i.e. the left
	 * border of the histogram. Use percentile() with the result of this to
	 * get the numeric value.
	 **/
	int startPercentile() const { return _startPercentile; }

	/**
	 * Set the percentile (0..100) until which on to display data, i.e. set
	 * the right border of the histogram. The real value to use is taken
	 * from the stored percentiles.
	 **/
	void setEndPercentile( int index );

	/**
	 * Return the percentile until which on to display data, i.e. the right
	 * border of the histogram. Use percentile() with the result of this to
	 * get the numeric value.
	 **/
	int endPercentile() const { return _endPercentile; }

	/**
	 * Automatically determine the best start and end percentile.
	 **/
	void autoStartEndPercentiles();

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

        /**
         * Return the start value of bucket no. 'index'.
         **/
        qreal bucketStart( int index ) const;

        /**
         * Return the end value of bucket no. 'index'.
         **/
        qreal bucketEnd( int index ) const;

	/**
	 * Return the total sum of all buckets.
	 **/
	qreal bucketsTotalSum() const;

	/**
	 * Set the percentile sums.
	 **/
	void setPercentileSums( const QRealList & newPercentileSums );

	/**
	 * Return the percentile sum for percentile no. 'index' (0..100),
	 * i.e. the accumulated values between percentile index-1 and index.
	 **/
	qreal percentileSum( int index ) const;

	/**
	 * Return the percentile sums from 'fromIndex' including to 'toIndex'.
	 **/
	qreal percentileSum( int fromIndex, int toIndex ) const;

	/**
	 * Enable or disable showing the median (percentile 50) as an overlay
	 * over the histogram.
	 **/
	void setShowMedian( bool show = true ) { _showMedian = show; }

	/**
	 * Return 'true' if the median is shown as an overlay, 'false' if not.
	 **/
	bool showMedian() const { return _showMedian; }

	/**
	 * Enable or disable showing the 1st and 3rd quartiles (Q1 and Q3,
	 * percentile 25 and 75, respectively) as an overlay over the
	 * histogram.
	 **/
	void setShowQuartiles( bool show = true ) { _showQuartiles = show; }

	/**
	 * Return 'true' if the 1st and 3rd quartiles are shown as an overlay,
	 * 'false' if not.
	 **/
	bool showQuartiles() const { return _showQuartiles; }

	/**
	 * Enable or disable showing percentiles as an overlay over the
	 * histogram. 'step' specifies how many of them to display; with the
	 * default '5' it will display P5, P10, P15 etc.; step = 0 disables
	 * them completely.
	 **/
	void setPercentileStep( int step = 5 ) { _percentileStep = step; }

	/**
	 * Return the percentile step or 0 if no percentiles are shown.
	 **/
	int percentileStep() const { return _percentileStep; }

	/**
	 * Set how many percentiles to display as an overlay at the left margin
	 * in addition to those shown with showPercentiles().
	 *
	 * A value of 2 with a histogram showing data from min to max means
	 * show also P1 and P2.
	 *
	 * A value of 2 with a histogram showing data from P3 to P97 means show
	 * also P4 and P5.
	 *
	 * A value of 0 means show no additional percentiles.
	 **/
	void setLeftMarginPercentiles( int number = 0 )
	    { _leftMarginPercentiles = number; }

	/**
	 * Return the left margin percentiles or 0 if none are shown.
	 **/
	int leftMarginPercentiles() { return _leftMarginPercentiles; }

	/**
	 * Set how many percentiles to display as an overlay at the right
	 * margin in addition to those shown with showPercentiles().
	 *
	 * A value of 2 with a histogram showing data from min to max means
	 * show also P98 and P99.
	 *
	 * A value of 2 with a histogram showing data from P3 to P97 means show
	 * also P95 and P96.
	 *
	 * A value of 0 means show no additional percentiles.
	 **/
	void setRightMarginPercentiles( int number= 2 )
	    { _leftMarginPercentiles = number; }

	/**
	 * Return the right margin percentiles or 0 if none are shown.
	 **/
	int rightMarginPercentiles() { return _rightMarginPercentiles; }

	/**
	 * Enable or disable a logarithmic (log2) height scale.
	 **/
	void setUseLogHeightScale( bool enable ) { _useLogHeightScale = enable; }

	/**
	 * Return 'true' if a logarithmic height scale is used or 'false' if
	 * not.
	 **/
	bool useLogHeightScale() const { return _useLogHeightScale; }

	/**
	 * Automatically determine if a logarithmic height scale should be
	 * used. Set the internal _useLogHeightScale variable accordingly and
	 * return it.
	 **/
	bool autoLogHeightScale();

	/**
	 * Convert a data value to the corresponding X axis point in the
	 * histogram.
	 **/
	qreal scaleValue( qreal value );

	// Pens and brushes for the various elements of the histograms

	QBrush barBrush()      const { return _barBrush;      }
	QPen   barPen()	       const { return _barPen;	      }
	QPen   medianPen()     const { return _medianPen;     }
	QPen   quartilePen()   const { return _quartilePen;   }
	QPen   percentilePen() const { return _percentilePen; }
	QPen   decilePen()     const { return _decilePen;     }


    public slots:

	/**
	 * Rebuild the histogram based on the current data.
	 **/
	void rebuild();


    protected:

	/**
	 * Common one-time initializations
	 **/
	void init();

	/**
	 * Return 'true' if percentile no. 'index' is in range for being
	 * displayed, i.e. if it is between _startPercentile and
	 * _endPercentile.
	 **/
	bool percentileDisplayed( int index ) const;


	// Graphical Elements

	void addHistogram();
	void addHistogramBackground();
	void addAxes();
	void addYAxisLabel();
	void addXAxisLabel();
	void addXStartEndLabels();
	void addQuartileText();
	void addHistogramBars();
	void addMarkers();

	void addOverflowPanel();

        /**
         * Add a text item at 'pos' and return the bottom left of its bounding
         * rect.
         **/
	QPointF addText( const QPointF & pos, const QStringList & lines );

        /**
         * Add a bold font text item at 'pos' and return the bottom left of its
         * bounding rect.
         **/
	QPointF addBoldText( const QPointF & pos, const QString & text );

	/**
	 * Add a pie diagram with two values val1 and val2.
         * Return the bottom left of the bounding rect.
	 **/
	QPointF addPie( const QRectF & rect,
			qreal	       val1,
			qreal	       val2,
			const QBrush & brush1,
			const QBrush & brush2 );

	/**
	 * Fit the graphics into the viewport.
	 **/
	void fitToViewport();

	/**
	 * Resize the view.
	 *
	 * Reimplemented from QFrame.
	 **/
	virtual void resizeEvent( QResizeEvent * event ) Q_DECL_OVERRIDE;

        /**
         * Calculate the content geometry to fit into 'newSize'.
         **/
        void calcGeometry( const QSize & newSize );

        /**
         * Resize the widget content to the current window size.
         **/
        void autoResize();

        /**
         * Return 'true' if an overflow ("cutoff") panel is needed.
         **/
        bool needOverflowPanel() const;


	//
	// Data Members
	//

	DelayedRebuilder * _rebuilder;
	QGraphicsItem	 * _histogramPanel;
        bool               _geometryDirty;


	// Statistics Data

	QRealList _buckets;
	QRealList _percentiles;
	QRealList _percentileSums;
	qreal	  _bucketMaxValue;

	int	  _startPercentile;
	int	  _endPercentile;

	bool	  _useLogHeightScale;

	// Flags and Settings

	bool	  _showMedian;
	bool	  _showQuartiles;
	int	  _percentileStep;
	int	  _leftMarginPercentiles;
	int	  _rightMarginPercentiles;

	// Brushes and Pens

	QBrush	  _panelBackground;
	QBrush	  _barBrush;
	QPen	  _barPen;

	QPen	  _medianPen;
	QPen	  _quartilePen;
	QPen	  _percentilePen;
	QPen	  _decilePen;

	QPen	  _piePen;
	QBrush	  _overflowSliceBrush;

	// Geometry

	qreal	  _histogramWidth;
	qreal	  _histogramHeight;

	qreal	  _leftBorder;		// left of histogram
	qreal	  _rightBorder;
	qreal	  _topBorder;
	qreal	  _bottomBorder;

	qreal	  _markerExtraHeight;

	qreal	  _overflowWidth;
	qreal	  _overflowLeftBorder;
	qreal	  _overflowRightBorder;
	qreal	  _overflowSpacing;	// between histogram and overflow area
	qreal	  _pieDiameter;
	qreal	  _pieSliceOffset;

	qreal	  _viewMargin;		// around all elements of the view
    };

}	// namespace QDirStat


#endif // ifndef HistogramView_h
