/*
 *   File name: HistogramView.cpp
 *   Summary:	View widget for histogram rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <math.h>
#include <algorithm>

#include <QResizeEvent>

#include "HistogramView.h"
#include "DelayedRebuilder.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

// See also HistogramDraw.cpp and HistogramOverflowPanel.cpp


#define CHECK_PERCENTILE_INDEX( INDEX ) \
    CHECK_INDEX_MSG( (INDEX), 0, 100, "Percentile index out of range" );

#define MinHistogramWidth	 150.0
#define MinHistogramHeight	  80.0

using namespace QDirStat;


HistogramView::HistogramView( QWidget * parent ):
    QGraphicsView( parent )
{
    init();

    _rebuilder = new DelayedRebuilder();
    CHECK_NEW( _rebuilder );

    connect( _rebuilder, SIGNAL( rebuild() ),
	     this,	 SLOT  ( rebuild() ) );

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy	( Qt::ScrollBarAlwaysOff );


    // Improve graphics quality from C64 style to 21st century style
    setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );
}


HistogramView::~HistogramView()
{
    delete _rebuilder;
}


void HistogramView::init()
{
    _histogramPanel = 0;
    _geometryDirty  = true;

    _bucketMaxValue	    = 0;
    _startPercentile	    = 0;    // data min
    _endPercentile	    = 100;  // data max
    _useLogHeightScale	    = false;
    _showMedian		    = true;
    _showQuartiles	    = true;
    _percentileStep	    = 0;
    _leftMarginPercentiles  = 0;
    _rightMarginPercentiles = 5;

    // TO DO: read from and write to QSettings

    _histogramHeight	  = 250.0;
    _histogramWidth	  = 600.0;

    _leftBorder		  = 40.0;
    _rightBorder	  = 10.0;
    _topBorder		  = 30.0;
    _bottomBorder	  = 50.0;
    _viewMargin		  = 10.0;

    _markerExtraHeight	  = 15.0;

    _overflowWidth	  = 150.0;
    _overflowLeftBorder	  = 10.0;
    _overflowRightBorder  = 10.0;
    _overflowSpacing	  = 15.0;
    _pieDiameter	  = 60.0;
    _pieSliceOffset	  = 10.0;

    _panelBackground	  = QBrush( QColor( 0xF0, 0xF0, 0xF0 ) );
    _barBrush		  = QBrush( QColor( 0xB0, 0xB0, 0xD0 ) );
    _barPen		  = QPen( QColor( 0x40, 0x40, 0x50 ), 1 );

    _medianPen		  = QPen( Qt::magenta, 2 );
    _quartilePen	  = QPen( Qt::blue, 2 );
    _percentilePen	  = QPen( QColor( 0xA0, 0xA0, 0xA0 ), 1 );
    _decilePen		  = QPen( QColor( 0x30, 0x80, 0x30 ), 1 );

    _piePen		  = QPen( Qt::black, 2 );
    _overflowSliceBrush	  = QBrush( QColor( 0xD0, 0x40, 0x20 ) );
}


void HistogramView::clear()
{
    _buckets.clear();
    _percentiles.clear();
    _percentileSums.clear();
    init();

    if ( scene() )
    {
	scene()->clear();
	scene()->invalidate( scene()->sceneRect() );
    }
}


qreal HistogramView::bestBucketCount( int n )
{
    if ( n < 2 )
	return 1;

    // Using the "Rice Rule" which gives more reasonable values for the numbers
    // we are likely to encounter in the context of QDirStat.

    qreal result = 2 * pow( n, 1.0/3.0 );

    if ( result > MAX_BUCKET_COUNT )
    {
	logInfo() << "Limiting bucket count to " << MAX_BUCKET_COUNT
		  << " instead of " << result
		  << endl;

	// Enforcing an upper limit so each histogram bar remains wide enough
	// to be clicked on or for tooltips etc.

	result = MAX_BUCKET_COUNT;
    }
    else if ( result < 1.0 )
    {
	result = 1.0;
    }

    return result;
}


qreal HistogramView::bucketWidth( qreal min, qreal max, int bucketCount )
{
    if ( bucketCount < 1 )
	return 0;

    return ( max - min ) / (qreal) bucketCount;
}


qreal HistogramView::bucketStart( int index ) const
{
    qreal offset = percentile( _startPercentile );
    return offset + index * bucketWidth();
}


qreal HistogramView::bucketEnd( int index ) const
{
    qreal offset = percentile( _startPercentile );
    return offset + ( index + 1 ) * bucketWidth();
}


void HistogramView::setBuckets( const QRealList & newBuckets )
{
    _buckets	    = newBuckets;
    _bucketMaxValue = 0;

    for ( int i=0; i < _buckets.size(); ++i )
	_bucketMaxValue = qMax( _bucketMaxValue, _buckets[i] );
}


void HistogramView::setPercentiles( const QRealList & newPercentiles )
{
    CHECK_INDEX_MSG( newPercentiles.size(), 101, 101,
		     "Percentiles size out of range" );

    _percentiles = newPercentiles;
}


void HistogramView::setPercentile( int index, qreal value )
{
    CHECK_PERCENTILE_INDEX( index );

    _percentiles[ index ] = value;
}


qreal HistogramView::percentile( int index ) const
{
    CHECK_PERCENTILE_INDEX( index );

    return _percentiles[ index ];
}


void HistogramView::setStartPercentile( int index )
{
    CHECK_PERCENTILE_INDEX( index );

    bool oldNeedOverflowPanel = needOverflowPanel();
    _startPercentile = index;

    if ( oldNeedOverflowPanel != needOverflowPanel() )
        _geometryDirty = true;
}


void HistogramView::setEndPercentile( int index )
{
    CHECK_PERCENTILE_INDEX( index );

    bool oldNeedOverflowPanel = needOverflowPanel();
    _endPercentile = index;

    if ( oldNeedOverflowPanel != needOverflowPanel() )
        _geometryDirty = true;

    if ( _startPercentile >= _endPercentile )
    {
	logError() << "startPercentile must be less than endPercentile: "
		   << _startPercentile << ".." << _endPercentile
		   << endl;
    }
}


bool HistogramView::percentileDisplayed( int index ) const
{
    return index >= _startPercentile && index <= _endPercentile;
}


void HistogramView::setPercentileSums( const QRealList & newPercentileSums )
{
    CHECK_INDEX_MSG( newPercentileSums.size(), 101, 101,
		     "Percentile sums size out of range" );

    _percentileSums = newPercentileSums;
}


qreal HistogramView::percentileSum( int index ) const
{
    CHECK_PERCENTILE_INDEX( index );

    if ( _percentileSums.isEmpty() )
	return 0.0;

    return _percentileSums[ index ];
}


qreal HistogramView::percentileSum( int fromIndex, int toIndex ) const
{
    CHECK_PERCENTILE_INDEX( fromIndex );
    CHECK_PERCENTILE_INDEX( toIndex );

    if ( _percentileSums.isEmpty() )
	return 0.0;

    qreal sum = 0.0;

    for ( int i=fromIndex; i <= toIndex; ++i )
	sum += _percentileSums[i];

    return sum;
}


qreal HistogramView::bucket( int index ) const
{
    CHECK_INDEX( index, 0, _buckets.size() - 1 );

    return _buckets[ index ];
}


qreal HistogramView::bucketWidth() const
{
    if ( _buckets.isEmpty() || _percentiles.isEmpty() )
	return 0;

    qreal startVal = percentile( _startPercentile );
    qreal endVal   = percentile( _endPercentile	  );

    if ( startVal > endVal )
	THROW( Exception( "Invalid percentile data" ) );

    return ( endVal - startVal ) / (qreal) _buckets.size();
}


qreal HistogramView::bucketsTotalSum() const
{
    qreal sum = 0;

    for ( int i=0; i < _buckets.size(); ++i )
	sum += _buckets[i];

    return sum;
}


void HistogramView::autoStartEndPercentiles()
{
    if ( _percentiles.isEmpty() )
    {
	logError() << "No percentiles set" << endl;
	return;
    }

    qreal q1	= percentile( 25 );
    qreal q3	= percentile( 75 );
    qreal qDist = q3 - q1;

    qreal minVal = qMax( q1 - 3 * qDist, 0.0 );
    qreal maxVal = qMin( q3 + 3 * qDist, percentile( 100 ) );

    bool oldNeedOverflowPanel = needOverflowPanel();

    for ( _startPercentile = 0; _startPercentile <= 25; ++_startPercentile )
    {
	if ( percentile( _startPercentile ) >= minVal )
	    break;
    }

    for ( _endPercentile = 100; _endPercentile >= 75; --_endPercentile )
    {
	if ( percentile( _endPercentile ) <= maxVal )
	    break;
    }

    if ( oldNeedOverflowPanel != needOverflowPanel() )
        _geometryDirty = true;

    logInfo() << "Q1: " << formatSize( q1 )
	      << "  Q3: " << formatSize( q3 )
	      << "  minVal: " << formatSize( minVal )
	      << "  maxVal: " << formatSize( maxVal )
	      << endl;
    logInfo() << "startPercentile: " << _startPercentile
	      << "  " << formatSize( percentile( _startPercentile ) )
	      << "  endPercentile: " << _endPercentile
	      << "  " << formatSize( percentile( _endPercentile	 ) )
	      << endl;
}


bool HistogramView::autoLogHeightScale()
{
    if ( _buckets.isEmpty() )
    {
	logError() << "No buckets set" << endl;
	return false;
    }

    _useLogHeightScale = false;


    if ( _buckets.size() > 3 )
    {
	QRealList data = _buckets;

	std::sort( data.begin(), data.end() );
	qreal largest = data.last();

	// We compare the largest bucket with the P90 percentile of the buckets
	// (not to confuse with the P90 percentile with the data the buckets
	// were collected from!)

	int referencePercentile = 85;
	int pos = data.size() / 100.0 * referencePercentile;
	qreal referencePercentileValue = data.at( pos );

	_useLogHeightScale = largest > referencePercentileValue * 10;

	logInfo() << "Largest bucket: " << largest
		  << " bucket P" << referencePercentile
		  << ": " << referencePercentileValue
		  << "	 -> use log height scale: " << _useLogHeightScale
		  << endl;
    }

    return _useLogHeightScale;
}


void HistogramView::calcGeometry( const QSize & newSize )
{
    _histogramWidth  = newSize.width();
    _histogramWidth -= _leftBorder + _rightBorder + 2 * _viewMargin;

    if ( needOverflowPanel() )
    {
	_histogramWidth -= _overflowSpacing + _overflowWidth;
        _histogramWidth -= _overflowLeftBorder + _overflowRightBorder;
    }

    if ( _histogramWidth < MinHistogramWidth )
	_histogramWidth = MinHistogramWidth;

    _histogramHeight  = newSize.height();
    _histogramHeight -= _bottomBorder + _topBorder + 2 * _viewMargin;
    _histogramHeight -= 30.0; // compensate for text above

    _histogramHeight  = qBound( MinHistogramHeight, _histogramHeight, 1.5 * _histogramWidth );
    _geometryDirty    = false;

#if 0
    logDebug() << "Histogram width: " << _histogramWidth
	       << " height: " << _histogramHeight
	       << endl;
#endif
}


bool HistogramView::needOverflowPanel() const
{
    return _startPercentile > 0 || _endPercentile < 100;
}


void HistogramView::autoResize()
{
    calcGeometry( viewport()->size() );
}


void HistogramView::resizeEvent( QResizeEvent * event )
{
    // logDebug() << "Event size: " << event->size() << endl;

    QGraphicsView::resizeEvent( event );
    calcGeometry( event->size() );

    _rebuilder->scheduleRebuild();
}


void HistogramView::fitToViewport()
{
    // This is the black magic that everybody hates from the bottom of his
    // heart about that drawing stuff: Making sure the graphics actually is
    // visible on the screen without unnecessary scrolling.
    //
    // You would think that a widget as sophisticated as QGraphicsView does
    // this all by itself, but no: Everybody has to waste hours upon hours of
    // life time with this crap.

    QRectF rect = scene()->sceneRect().normalized();
    // logDebug() << "Old scene rect: " << rect << endl;

    scene()->setSceneRect( rect );

    rect.adjust( -_viewMargin, -_viewMargin, _viewMargin, _viewMargin );
    // logDebug() << "New scene rect: " << rect << endl;
    // logDebug() << "Viewport size:  " << viewport()->size() << endl;

    QSize visibleSize = viewport()->size();

    if ( rect.width()  <= visibleSize.width() &&
	 rect.height() <= visibleSize.height()	 )
    {
	logDebug() << "Histogram in " << rect.size()
		   << " fits into visible size " << visibleSize
		   << endl;

	setTransform( QTransform() ); // Reset scaling etc.
	ensureVisible( rect, 0, 0 );
    }
    else
    {
	logDebug() << "Scaling down histogram in " << rect.size()
		   << " to fit into visible size " << visibleSize
		   << endl;

	fitInView( rect, Qt::KeepAspectRatio );
    }
}


void HistogramView::rebuild()
{
    if ( _rebuilder->firstRebuild() )
    {
	_rebuilder->scheduleRebuild();
	return;
    }

    logInfo() << "Rebuilding histogram" << endl;

    if ( _geometryDirty )
        autoResize();

    // QGraphicsScene never resets the min and max in both dimensions where it
    // ever created QGraphicsItems, which makes its sceneRect() call pretty
    // useless. Let's create a new one without those bad old memories.

    if ( scene() )
	delete scene();

    QGraphicsScene * newScene = new QGraphicsScene( this );
    CHECK_NEW( newScene);
    setScene( newScene );
    scene()->setBackgroundBrush( Qt::white );

    if ( _buckets.size() < 1 || _percentiles.size() != 101 )
    {
	scene()->addText( "No data yet" );
	logInfo() << "No data yet" << endl;
	return;
    }

    addHistogram();
    addOverflowPanel();

    fitToViewport();
}


qreal HistogramView::scaleValue( qreal value )
{
    qreal startVal   = _percentiles[ _startPercentile ];
    qreal endVal     = _percentiles[ _endPercentile   ];
    qreal totalWidth = endVal - startVal;
    qreal result     = ( value - startVal ) / totalWidth * _histogramWidth;

    // logDebug() << "Scaling " << formatSize( value ) << " to " << result << endl;

    return result;
}
