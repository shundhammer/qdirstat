/*
 *   File name: HistogramView.cpp
 *   Summary:	View widget for histogram rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <math.h>
#include <QGraphicsRectItem>

#include "HistogramView.h"
#include "Logger.h"
#include "Exception.h"


#define CHECK_PERCENTILE_INDEX( INDEX ) \
    CHECK_INDEX_MSG( (INDEX), 0, 100, "Percentile index out of range" );


using namespace QDirStat;


HistogramView::HistogramView( QWidget * parent ):
    QGraphicsView( parent )
{
    logInfo() << "HistogramView ctor" << endl;
    redisplay();
}


HistogramView::~HistogramView()
{
    logInfo() << "HistogramView dtor" << endl;
}


void HistogramView::init()
{
    _bucketMaxValue    = 0;
    _startPercentile   = 0;   // data min
    _endPercentile     = 100; // data max
    _showMedian        = true;
    _showQuartiles     = true;
    _percentileStep    = 5;
    _marginPercentiles = 2;


    // TO DO: read from and write to QSettings

    _barBrush      = QBrush( QColor( 0x70, 0x70, 0x80 ) );
    _barPen        = QPen  ( QColor( 0x40, 0x40, 0x50 ), 1 );

    _medianPen     = QPen( Qt::magenta, 2 );
    _quartilePen   = QPen( Qt::blue, 2 );
    _percentilePen = QPen( QColor( 0x30, 0x30, 0x30 ), 1 );
    _decilePen     = QPen( QColor( 0x30, 0x50, 0x30 ), 2 );
}


void HistogramView::clear()
{
    _buckets.clear();
    _percentiles.clear();
    init();

    if ( scene() )
	qDeleteAll( scene()->items() );
}


qreal HistogramView::bestBucketCount( int n )
{
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


void HistogramView::setBuckets( const QRealList & newBuckets )
{
    _buckets        = newBuckets;
    _bucketMaxValue = 0;

    for ( int i=0; i < _buckets.size(); ++i )
        _bucketMaxValue = qMax( _bucketMaxValue, _buckets[i] );
}


void HistogramView::setPercentiles( const QRealList & newPercentiles )
{
    CHECK_INDEX_MSG( newPercentiles.size(), 100, 100,
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

    _startPercentile = index;
}


void HistogramView::setEndPercentile( int index )
{
    CHECK_PERCENTILE_INDEX( index );

    _endPercentile = index;

    if ( _startPercentile >= _endPercentile )
    {
        logError() << "startPercentile must be less than endPercentile: "
                   << _startPercentile << ".." << _endPercentile
                   << endl;
    }
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
    qreal endVal   = percentile( _endPercentile   );

    if ( startVal >= endVal )
        THROW( Exception( "Invalid percentile data" ) );

    return ( endVal - startVal ) / (qreal) _buckets.size();
}


void HistogramView::redisplay()
{

    logInfo() << "Displaying histogram" << endl;

    if ( ! scene() )
    {
	QGraphicsScene * scene = new QGraphicsScene( this );
	CHECK_NEW( scene);
	setScene( scene );
    }

    if ( _buckets.size() < 1 || _percentiles.size() != 100 )
    {
        scene()->addText( "No data yet" );
        logInfo() << "No data yet" << endl;
        return;
    }

    QPen pen( Qt::black );
    pen.setWidth( 2 );

    QBrush brush( Qt::blue );

    scene()->addRect( 0, 0, 100, 100, pen, brush );
}
