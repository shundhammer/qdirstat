/*
 *   File name: HistogramView.cpp
 *   Summary:	View widget for histogram rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <math.h>
#include <algorithm>

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

#include "HistogramView.h"
#include "FileInfo.h"
#include "Logger.h"
#include "Exception.h"


#define CHECK_PERCENTILE_INDEX( INDEX ) \
    CHECK_INDEX_MSG( (INDEX), 0, 100, "Percentile index out of range" );


using namespace QDirStat;


HistogramView::HistogramView( QWidget * parent ):
    QGraphicsView( parent )
{
    init();
}


HistogramView::~HistogramView()
{
}


void HistogramView::init()
{
    _bucketMaxValue    = 0;
    _startPercentile   = 0;    // data min
    _endPercentile     = 100;  // data max
    _useLogHeightScale = false;
    _showMedian        = true;
    _showQuartiles     = true;
    _percentileStep    = 5;
    _marginPercentiles = 2;


    // TO DO: read from and write to QSettings

    _barBrush      = QBrush( QColor( 0xB0, 0xB0, 0xD0 ) );
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


bool HistogramView::percentileDisplayed( int index ) const
{
    return index >= _startPercentile && index <= _endPercentile;
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


void HistogramView::autoStartEndPercentiles()
{
    if ( _percentiles.isEmpty() )
    {
        logError() << "No percentiles set" << endl;
        return;
    }

    qreal q1    = percentile( 25 );
    qreal q3    = percentile( 75 );
    qreal qDist = q3 - q1;

    qreal minVal = qMax( q1 - 3 * qDist, 0.0 );
    qreal maxVal = qMin( q3 + 3 * qDist, percentile( 100 ) );

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

    logInfo() << "Q1: " << formatSize( q1 )
              << "  Q3: " << formatSize( q3 )
              << "  minVal: " << formatSize( minVal )
              << "  maxVal: " << formatSize( maxVal )
              << endl;
    logInfo() << "startPercentile: " << _startPercentile
              << "  " << formatSize( percentile( _startPercentile ) )
              << "  endPercentile: " << _endPercentile
              << "  " << formatSize( percentile( _endPercentile  ) )
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
                  << "   -> use log height scale: " << _useLogHeightScale
                  << endl;
    }

    return _useLogHeightScale;
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
    else
    {
        qDeleteAll( scene()->items() );
    }

    if ( _buckets.size() < 1 || _percentiles.size() != 101 )
    {
        scene()->addText( "No data yet" );
        logInfo() << "No data yet" << endl;
        return;
    }

    _histogramHeight = 250.0; // FIXME
    _histogramWidth  = 600.0; // FIXME

    _leftBorder      = 40.0;
    _rightBorder     = 10.0;
    _topBorder       = 30.0;
    _bottomBorder    = 50.0;

    _markerExtraHeight = 15.0;


    addHistogramBackground();
    addYAxisLabel();
    addXAxisLabel();
    addXStartEndLabels();
    addHistogramBars();
    addMarkers();
}


void HistogramView::addHistogramBackground()
{
    QBrush brush( QColor( 0xE0, 0xE0, 0xE0, 0x80 ) );
    QPen   pen( Qt::NoPen );

    QRectF rect( -_leftBorder,
                 _bottomBorder,
                 _histogramWidth   + _leftBorder + _rightBorder,
                 -( _histogramHeight + _topBorder  + _bottomBorder ) );

    scene()->addRect( rect, pen, brush );
}


void HistogramView::addYAxisLabel()
{
    QString labelText = _useLogHeightScale ? "log<sub>2</sub>(n)   -->" : "n";

    QGraphicsTextItem * item = scene()->addText( "" );
    item->setHtml( labelText );

    QFont font( item->font() );
    font.setBold( true );
    item->setFont( font );

    qreal   textWidth   = item->boundingRect().width();
    qreal   textHeight  = item->boundingRect().height();
    QPointF labelCenter = QPoint( -_leftBorder / 2, -_histogramHeight / 2 );

    if ( _useLogHeightScale )
    {
        item->setRotation( 270 );
        item->setPos( labelCenter.x() - textHeight / 2,
                      labelCenter.y() + textWidth  / 2 );
    }
    else
    {
        item->setPos( labelCenter.x() - textWidth  / 2,
                      labelCenter.y() - textHeight / 2 );
    }
}


void HistogramView::addXAxisLabel()
{
    QString labelText = tr( "File Size" ) + "  -->";

    QGraphicsSimpleTextItem * item = scene()->addSimpleText( labelText );

    QFont font( item->font() );
    font.setBold( true );
    item->setFont( font );

    qreal   textWidth   = item->boundingRect().width();
    qreal   textHeight  = item->boundingRect().height();
    QPointF labelCenter = QPoint( _histogramWidth / 2, _bottomBorder );

    item->setPos( labelCenter.x() - textWidth / 2,
                  labelCenter.y() - textHeight ); // Align bottom
}


void HistogramView::addXStartEndLabels()
{
    QString startLabel = tr( "Min" ) + "\n0";

    if ( _startPercentile > 0 )
    {
        startLabel = QString( "P%1" ).arg( _startPercentile );
        startLabel += "\n" + formatSize( percentile( _startPercentile ) );
    }

    QString endLabel = _endPercentile == 100 ?
        tr( "Max" ) :
        QString( "P%1" ).arg( _endPercentile );

    QString endSizeLabel = formatSize( percentile( _endPercentile ) );

    QGraphicsSimpleTextItem * startItem   = scene()->addSimpleText( startLabel );
    QGraphicsSimpleTextItem * endItem     = scene()->addSimpleText( endLabel );
    QGraphicsSimpleTextItem * endSizeItem = scene()->addSimpleText( endSizeLabel );

    qreal endTextHeight = endItem->boundingRect().height();
    qreal endTextWidth  = endItem->boundingRect().width();
    qreal endSizeWidth  = endSizeItem->boundingRect().width();
    qreal y             = _bottomBorder - 2 * endTextHeight;

    startItem->setPos( 0, _bottomBorder - startItem->boundingRect().height() );
    endItem->setPos( _histogramWidth - endTextWidth, y );
    endSizeItem->setPos( _histogramWidth - endSizeWidth, y + endTextHeight );
}


void HistogramView::addHistogramBars()
{
    qreal barWidth = _histogramWidth / bucketCount();
    qreal maxVal   = _bucketMaxValue;

    if ( _useLogHeightScale )
        maxVal = log2( maxVal );

    for ( int i=0; i < _buckets.size(); ++i )
    {
        logDebug() << "Adding bar #" << i << " with value " << _buckets[ i ] << endl;
        QRectF rect;
        rect.setX( i * barWidth );
        rect.setY( 0 );
        rect.setHeight( -_histogramHeight );
        rect.setWidth( barWidth );

        qreal val = _buckets[ i ];

        if ( _useLogHeightScale && val > 1.0 )
            val = log2( val );

        qreal fillHeight = val / maxVal * _histogramHeight;

        HistogramBar * bar = new HistogramBar( this, i, rect, fillHeight );
        CHECK_NEW( bar );
    }
}


void HistogramView::addMarkers()
{
    QLineF zeroLine( 0, _markerExtraHeight,
                     0, -( _histogramHeight + _markerExtraHeight ) );

    if ( _showQuartiles )
    {
        if ( percentileDisplayed( 25 ) )
            new PercentileMarker( this, 25, tr( "Q1" ), zeroLine, _quartilePen );

        if ( percentileDisplayed( 75 ) )
            new PercentileMarker( this, 75, tr( "Q3" ), zeroLine, _quartilePen );
    }

    if ( _showMedian && percentileDisplayed( 50 ) )
        new PercentileMarker( this, 50, tr( "Median" ), zeroLine, _medianPen );

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




HistogramBar::HistogramBar( HistogramView * parent,
                            int             number,
                            const QRectF &  rect,
                            qreal           fillHeight ):
    QGraphicsRectItem( rect ),
    _parentView( parent ),
    _number( number )
{
    setPen( Qt::NoPen );

    // Setting NoPen so this rectangle remains invisible: This full-height
    // rectangle is just for clicking. For the bar content, we create a visible
    // separate child item with the correct height.

    QRectF childRect = rect;
    childRect.setHeight( -fillHeight );

    QGraphicsRectItem * filledRect = new QGraphicsRectItem( childRect, this );
    CHECK_NEW( filledRect );

    filledRect->setPen( _parentView->barPen() );
    filledRect->setBrush( _parentView->barBrush() );

    setFlags( ItemIsSelectable );
    _parentView->scene()->addItem( this );
}


void HistogramBar::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
    switch ( event->button() )
    {
	case Qt::LeftButton:
            {
                QGraphicsRectItem::mousePressEvent( event );

                qreal bwidth = _parentView->bucketWidth();
                qreal from   = bwidth * _number;
                qreal to     = from + bwidth;

                logDebug() << "Histogram bar #" << _number
                           << ": " << _parentView->bucket( _number ) << " items;"
                           << " range: " << formatSize( from )
                           << " .. " << formatSize( to )
                           << endl;
            }
	    break;

	default:
	    QGraphicsRectItem::mousePressEvent( event );
	    break;
    }
}




PercentileMarker::PercentileMarker( HistogramView * parent,
                                    int             percentileIndex,
                                    const QString & name,
                                    const QLineF &  zeroLine,
                                    const QPen &    pen ):
    QGraphicsLineItem( translatedLine( zeroLine, percentileIndex, parent ) ),
    _parentView( parent ),
    _name( name ),
    _percentileIndex( percentileIndex )
{
    if ( _name.isEmpty() )
        _name = QString( "P%1" ).arg( _percentileIndex );

    setPen( pen );
    setFlags( ItemIsSelectable );
    _parentView->scene()->addItem( this );
}


QLineF PercentileMarker::translatedLine( const QLineF &  zeroLine,
                                         int             percentileIndex,
                                         HistogramView * parent ) const
{
    qreal value = parent->percentile( percentileIndex );
    qreal x     = parent->scaleValue( value );

    return zeroLine.translated( x, 0 );
}


qreal PercentileMarker::value() const
{
    return _parentView->percentile( _percentileIndex );
}


void PercentileMarker::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
    switch ( event->button() )
    {
	case Qt::LeftButton:
	    QGraphicsLineItem::mousePressEvent( event );
	    logDebug() << "Percentile marker #" << _percentileIndex
                       << ": " << _name
                       << ": " << formatSize( _parentView->percentile( _percentileIndex ) )
                       << endl;
	    break;

	default:
	    QGraphicsLineItem::mousePressEvent( event );
	    break;
    }
}

