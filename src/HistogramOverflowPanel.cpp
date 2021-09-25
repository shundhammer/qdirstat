/*
 *   File name: HistogramOverflowPanel.cpp
 *   Summary:	Overflow panel drawing for file size histogram
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QGraphicsItem>

#include "HistogramView.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


void HistogramView::addOverflowPanel()
{
    if ( ! needOverflowPanel() )
	return;

    // Panel for the overflow area

    QRectF histPanelRect = _histogramPanel->boundingRect().normalized();

    QRectF rect( histPanelRect.topRight().x() + _overflowSpacing,
		 histPanelRect.topRight().y(),
		 _overflowWidth + _overflowLeftBorder + _overflowRightBorder,
		 histPanelRect.height() );

    QGraphicsRectItem * cutoffPanel =
	scene()->addRect( rect, QPen( Qt::NoPen ), _panelBackground );


    // Headline

    QPointF nextPos( rect.x() + _overflowLeftBorder, rect.y() );

    nextPos = addBoldText( nextPos, tr( "Cut off" ) );


    // Text about cut-off percentiles and size

    qreal filesInHistogram = bucketsTotalSum();
    qreal totalFiles = bucketsTotalSum() / ( _endPercentile - _startPercentile ) * 100.0;
    int missingFiles = totalFiles - filesInHistogram;

    QStringList lines;

    if ( _startPercentile > 0 )
    {
        lines << "";
        lines << tr( "Min (P0) .. P%1" ).arg( _startPercentile );
        lines << tr( "%1 .. %2" )
            .arg( formatSize( percentile( 0 ) ) )
            .arg( formatSize( percentile( _startPercentile ) ) );
    }

    if ( _endPercentile < 100 )
    {
        lines << "";
        lines << tr( "P%1 .. Max (P100)" ).arg( _endPercentile );
        lines << tr( "%1 .. %2" )
            .arg( formatSize( percentile( _endPercentile ) ) )
            .arg( formatSize( percentile( 100 ) ) );
    }

    nextPos = addText( nextPos, lines );


    // Upper pie chart: Number of files cut off

    nextPos.setY( nextPos.y() + _pieSliceOffset );
    QRectF pieRect( QRectF( nextPos, QSizeF( _pieDiameter, _pieDiameter ) ) );

    int cutoff = _startPercentile + 100 - _endPercentile;
    nextPos = addPie( pieRect,
		      100 - cutoff, cutoff,
		      _barBrush, _overflowSliceBrush );

    // Caption for the upper pie chart

    lines.clear();
    lines << tr( "%1% of all files" ).arg( cutoff );
    lines << ( missingFiles == 1 ?
	       tr( "1 file total" ) :
	       tr( "%1 files total" ).arg( missingFiles ) );
    lines << "";
    nextPos = addText( nextPos, lines );


    // Lower pie chart: Disk space disregarded

    qreal histogramDiskSpace = percentileSum( _startPercentile, _endPercentile );
    qreal cutoffDiskSpace    = percentileSum( 0, _startPercentile );

    if ( _endPercentile < 100 )
        cutoffDiskSpace += percentileSum( _endPercentile, 100 );

    qreal cutoffSpacePercent = 100.0 * cutoffDiskSpace / ( histogramDiskSpace + cutoffDiskSpace );

    nextPos.setY( nextPos.y() + _pieSliceOffset );
    pieRect = QRectF( nextPos, QSizeF( _pieDiameter, _pieDiameter ) );

    if ( cutoffDiskSpace > histogramDiskSpace )
    {
	nextPos = addPie( pieRect,
			  cutoffDiskSpace, histogramDiskSpace,
			  _overflowSliceBrush, _barBrush );
    }
    else
    {
	nextPos = addPie( pieRect,
			  histogramDiskSpace, cutoffDiskSpace,
			  _barBrush, _overflowSliceBrush );
    }


    // Caption for the lower pie chart

    lines.clear();
    lines << tr( "%1% of disk space" ).arg( cutoffSpacePercent, 0, 'f', 1 );
    lines << tr( "%1 total" ).arg( formatSize( cutoffDiskSpace ) );
    lines << "";
    nextPos = addText( nextPos, lines );


    // Make sure the panel is tall enough to fit everything in

    if ( nextPos.y() > cutoffPanel->rect().bottom() )
    {
	QRectF rect( cutoffPanel->rect() );
	rect.setBottomLeft( QPointF( rect.x(), nextPos.y()  ) );
	cutoffPanel->setRect( rect );
    }
}


QPointF HistogramView::addPie( const QRectF & rect,
			       qreal	      val1,
			       qreal	      val2,
			       const QBrush & brush1,
			       const QBrush & brush2 )
{
    if ( val1 + val2 == 0.0 )
	return rect.topLeft();

    const qreal FullCircle = 360.0 * 16.0; // Qt uses 1/16 degrees
    qreal angle1 = val1 / ( val1 + val2 ) * FullCircle;
    qreal angle2 = FullCircle - angle1;

    QGraphicsEllipseItem * slice1 = scene()->addEllipse( rect );

    slice1->setStartAngle( angle2 / 2.0 );
    slice1->setSpanAngle( angle1 );
    slice1->setBrush( brush1 );
    slice1->setPen( _piePen );

    QRectF rect2( rect );
    rect2.moveTopLeft( rect.topLeft() + QPoint( _pieSliceOffset, 0.0 ) );
    QGraphicsEllipseItem * slice2 = scene()->addEllipse( rect2 );

    slice2->setStartAngle( -angle2 / 2.0 );
    slice2->setSpanAngle( angle2 );
    slice2->setBrush( brush2 );
    slice2->setPen( _piePen );

    QList<QGraphicsItem *> slices;
    slices << slice1 << slice2;

    QGraphicsItemGroup * pie = scene()->createItemGroup( slices );
    QPointF pieCenter = rect.center();

    // Figuring out the following arcane sequence took me well over 2 hours.
    //
    // Seriously, trolls, WTF?! One of the most common things to do is to
    // rotate a QGraphicsItem around its center. But this is the most difficult
    // thing to do, and, adding insult to injury, IT IS NOT EXPLAINED IN THE
    // DOCUMENTATION!

    QTransform transform;
    transform.translate( pieCenter.x(), pieCenter.y() );
    transform.rotate( -45.0 );
    transform.translate( -pieCenter.x(), -pieCenter.y() );
    pie->setTransform( transform );

    return QPoint( rect.x(), rect.y() + pie->boundingRect().height() );
}

