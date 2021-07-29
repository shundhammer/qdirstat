/*
 *   File name: HistogramDraw.cpp
 *   Summary:	Draw routines for file size histogram
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <math.h>

#include "HistogramView.h"
#include "HistogramItems.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"


#define UnicodeMathSigma	0x2211	// 'n-ary summation'

using namespace QDirStat;


void HistogramView::addHistogram()
{
    addHistogramBackground();
    addAxes();
    addYAxisLabel();
    addXAxisLabel();
    addXStartEndLabels();
    addQuartileText();
    addHistogramBars();
    addMarkers();
}


void HistogramView::addHistogramBackground()
{
    QRectF rect( -_leftBorder,
		 _bottomBorder,
		 _histogramWidth   + _leftBorder + _rightBorder,
		 -( _histogramHeight + _topBorder  + _bottomBorder ) );

    _histogramPanel = scene()->addRect( rect, QPen( Qt::NoPen ), _panelBackground );
    _histogramPanel->setZValue( PanelBackgroundLayer );
}


void HistogramView::addAxes()
{
    QPen pen( QColor( Qt::black ), 1 );

    QGraphicsItem * xAxis = scene()->addLine( 0, 0, _histogramWidth, 0, pen );
    QGraphicsItem * yAxis = scene()->addLine( 0, 0, 0, -_histogramHeight, pen );

    xAxis->setZValue( AxisLayer );
    yAxis->setZValue( AxisLayer );
}


void HistogramView::addYAxisLabel()
{
    QString labelText = _useLogHeightScale ? "log<sub>2</sub>(n)   -->" : "n";

    QGraphicsTextItem * item = scene()->addText( "" );
    item->setHtml( labelText );

    QFont font( item->font() );
    font.setBold( true );
    item->setFont( font );

    qreal   textWidth	= item->boundingRect().width();
    qreal   textHeight	= item->boundingRect().height();
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

    item->setZValue( TextLayer );
}


void HistogramView::addXAxisLabel()
{
    QString labelText = tr( "File Size" ) + "  -->";

    QGraphicsSimpleTextItem * item = scene()->addSimpleText( labelText );

    QFont font( item->font() );
    font.setBold( true );
    item->setFont( font );

    qreal   textWidth	= item->boundingRect().width();
    qreal   textHeight	= item->boundingRect().height();
    QPointF labelCenter = QPoint( _histogramWidth / 2, _bottomBorder );

    item->setPos( labelCenter.x() - textWidth / 2,
		  labelCenter.y() - textHeight ); // Align bottom

    item->setZValue( TextLayer );
}


void HistogramView::addXStartEndLabels()
{
    QString startLabel = tr( "Min" );

    if ( _startPercentile > 0 )
	startLabel = QString( "P%1" ).arg( _startPercentile );

    startLabel += "\n" + formatSize( percentile( _startPercentile ) );

    QString endLabel = _endPercentile == 100 ?
	tr( "Max" ) :
	QString( "P%1" ).arg( _endPercentile );

    QString endSizeLabel = formatSize( percentile( _endPercentile ) );

    QGraphicsSimpleTextItem * startItem	  = scene()->addSimpleText( startLabel );
    QGraphicsSimpleTextItem * endItem	  = scene()->addSimpleText( endLabel );
    QGraphicsSimpleTextItem * endSizeItem = scene()->addSimpleText( endSizeLabel );

    qreal endTextHeight = endItem->boundingRect().height();
    qreal endTextWidth	= endItem->boundingRect().width();
    qreal endSizeWidth	= endSizeItem->boundingRect().width();
    qreal y		= _bottomBorder - 2 * endTextHeight;

    startItem->setPos( 0, _bottomBorder - startItem->boundingRect().height() );
    endItem->setPos( _histogramWidth - endTextWidth, y );
    endSizeItem->setPos( _histogramWidth - endSizeWidth, y + endTextHeight );

    startItem->setZValue( TextLayer );
    endItem->setZValue( TextLayer );
    endSizeItem->setZValue( TextLayer );
}


void HistogramView::addQuartileText()
{
    qreal textBorder  = 10.0;
    qreal textSpacing = 30.0;

    qreal x = 0;
    qreal y = -_histogramHeight - _topBorder - textBorder;
    qreal n = bucketsTotalSum();

    if ( n > 0 ) // Only useful if there are any data at all
    {
	QString q1Text = tr( "Q1: %1" ).arg( formatSize( percentile( 25 ) ) );
	QString q3Text = tr( "Q3: %1" ).arg( formatSize( percentile( 75 ) ) );
	QString medianText = tr( "Median: %1" ).arg( formatSize( percentile( 50 ) ) );

	QGraphicsSimpleTextItem * q1Item     = scene()->addSimpleText( q1Text );
	QGraphicsSimpleTextItem * q3Item     = scene()->addSimpleText( q3Text );
	QGraphicsSimpleTextItem * medianItem = scene()->addSimpleText( medianText );

	q1Item->setBrush( _quartilePen.color() );
	q3Item->setBrush( _quartilePen.color() );
	medianItem->setBrush( _medianPen.color() );

	QFont font( medianItem->font() );
	font.setBold( true );

	medianItem->setFont( font );
	q1Item->setFont( font );
	q3Item->setFont( font );

	y -= medianItem->boundingRect().height();

	qreal q1Width	  = q1Item->boundingRect().width();
	qreal q3Width	  = q3Item->boundingRect().width();
	qreal medianWidth = medianItem->boundingRect().width();

	q1Item->setPos( x, y );	     x += q1Width     + textSpacing;
	medianItem->setPos( x, y );  x += medianWidth + textSpacing;
	q3Item->setPos( x, y );	     x += q3Width     + textSpacing;

	q1Item->setZValue( TextLayer );
	q3Item->setZValue( TextLayer );
	medianItem->setZValue( TextLayer );
    }


    //
    // Add text for the total number of files
    //

    QString nText = tr( "Files (n): %1" ).arg( n );
    QGraphicsSimpleTextItem * nTextItem = scene()->addSimpleText( nText );

    QFont font( nTextItem->font() );
    font.setBold( true );
    nTextItem->setFont( font );

    QFontMetrics metrics( font );
    QChar sigma( UnicodeMathSigma );

    if ( metrics.inFont( sigma ) )
	nTextItem->setText( QString( "%1n: %2" ).arg( sigma ).arg( n ) );

    if ( n == 0 )
	y -= nTextItem->boundingRect().height();

    nTextItem->setPos( x, y );
    nTextItem->setZValue( TextLayer );
}


void HistogramView::addHistogramBars()
{
    qreal barWidth = _histogramWidth / _buckets.size();
    qreal maxVal   = _bucketMaxValue;

    if ( _useLogHeightScale )
	maxVal = log2( maxVal );

    for ( int i=0; i < _buckets.size(); ++i )
    {
	// logDebug() << "Adding bar #" << i << " with value " << _buckets[ i ] << endl;
	QRectF rect;
	rect.setX( i * barWidth );
	rect.setY( 0 );
	rect.setHeight( -_histogramHeight );
	rect.setWidth( barWidth );

	qreal val = _buckets[i];

	if ( _useLogHeightScale && val > 1.0 )
	    val = log2( val );

	qreal fillHeight = maxVal == 0 ?
	    0.0 :
	    val / maxVal * _histogramHeight;

	HistogramBar * bar = new HistogramBar( this, i, rect, fillHeight );
	CHECK_NEW( bar );
    }
}


void HistogramView::addMarkers()
{
    qreal totalWidth =
	_percentiles[ _endPercentile   ] -
	_percentiles[ _startPercentile ];

    if ( totalWidth < 1 )
	return;

    QLineF zeroLine( 0, _markerExtraHeight,
		     0, -( _histogramHeight + _markerExtraHeight ) );

    // Show ordinary percentiles (all except Q1, Median, Q3)

    for ( int i = _startPercentile + 1; i < _endPercentile; ++i )
    {
	if ( i == 0 || i == 100 )
	    continue;

	if ( i == 50 && _showMedian )
	    continue;

	if ( ( i == 25 || i == 75 ) && _showQuartiles )
	    continue;

	if ( _percentileStep != 1 )
        {
            bool skip = true;

            if ( i <= _startPercentile + _leftMarginPercentiles && i < 25 )
                skip = false;

            if ( i >= _endPercentile - _rightMarginPercentiles && i > 75 )
                skip = false;

            if  ( skip && _percentileStep != 0 && i % _percentileStep == 0 )
                skip = false;

            if ( skip )
                continue;
        }

	QPen pen = _percentilePen;

	if ( _percentileStep != 0 && _percentileStep != 5 && i % 10 == 0 )
	    pen = _decilePen;

	// logDebug() << "Adding marker for P" << i << endl;
	new PercentileMarker( this, i, "", zeroLine, pen );
    }

    if ( _showQuartiles )
    {
	if ( percentileDisplayed( 25 ) )
	    new PercentileMarker( this, 25, tr( "Q1 (1st Quartile)" ),
				  zeroLine, _quartilePen );

	if ( percentileDisplayed( 75 ) )
	    new PercentileMarker( this, 75, tr( "Q3 (3rd Quartile)" ),
				  zeroLine, _quartilePen );
    }

    if ( _showMedian && percentileDisplayed( 50 ) )
    {
	new PercentileMarker( this, 50, tr( "Median" ),
			      zeroLine, _medianPen );
    }
}


QPointF HistogramView::addText( const QPointF & pos, const QStringList & lines )
{
    QGraphicsTextItem * textItem = scene()->addText( lines.join( "\n" ) );
    textItem->setPos( pos );
    textItem->setDefaultTextColor( Qt::black );

    return QPoint( pos.x(), pos.y() + textItem->boundingRect().height() );
}


QPointF HistogramView::addBoldText( const QPointF & pos, const QString & text )
{
    QGraphicsTextItem * textItem = scene()->addText( text );
    textItem->setPos( pos );

    QFont boldFont( textItem->font() );
    boldFont.setBold( true );
    textItem->setFont( boldFont );
    textItem->setDefaultTextColor( Qt::black );

    return QPoint( pos.x(), pos.y() + textItem->boundingRect().height() );
}

