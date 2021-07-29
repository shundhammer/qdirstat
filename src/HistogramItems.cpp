/*
 *   File name: HistogramViewItems.cpp
 *   Summary:	QGraphicsItems for file size histogram for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QToolTip>

#include "HistogramItems.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


HistogramBar::HistogramBar( HistogramView * parent,
			    int		    number,
			    const QRectF &  rect,
			    qreal	    fillHeight ):
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

    // setFlags( ItemIsSelectable );
    _parentView->scene()->addItem( this );

    _startVal = _parentView->bucketStart( _number );
    _endVal   = _parentView->bucketEnd  ( _number );

    QString tooltip = QObject::tr( "Bucket #%1:\n%2 Files\n%3 .. %4" )
	.arg( _number + 1 )
	.arg( _parentView->bucket( _number ) )
	.arg( formatSize( _startVal ) )
	.arg( formatSize( _endVal ) );

    setToolTip( tooltip );
    filledRect->setToolTip( tooltip );

    setZValue( HistogramView::InvisibleBarLayer );
    filledRect->setZValue( HistogramView::BarLayer );
}


void HistogramBar::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
    switch ( event->button() )
    {
	case Qt::LeftButton:
	    {
		QGraphicsRectItem::mousePressEvent( event );

#if 0
                // FIXME: This does not work. Why?

                QPointF pos( event->scenePos() );
                QToolTip::showText( QPoint( pos.x(), pos.y() ), toolTip(), _parentView );
#endif

		logDebug() << "Histogram bar #" << _number
			   << ": " << _parentView->bucket( _number ) << " items;"
			   << " range: " << formatSize( _startVal )
			   << " .. " << formatSize( _endVal )
			   << endl;
	    }
	    break;

	default:
	    QGraphicsRectItem::mousePressEvent( event );
	    break;
    }
}




PercentileMarker::PercentileMarker( HistogramView * parent,
				    int		    percentileIndex,
				    const QString & name,
				    const QLineF &  zeroLine,
				    const QPen &    pen ):
    QGraphicsLineItem( translatedLine( zeroLine, percentileIndex, parent ) ),
    _parentView( parent ),
    _name( name ),
    _percentileIndex( percentileIndex )
{
    if ( _name.isEmpty() )
    {
	_name = QObject::tr( "Percentile P%1" ).arg( _percentileIndex );
	setZValue( HistogramView::MarkerLayer );
    }
    else
    {
	setZValue( HistogramView::SpecialMarkerLayer );
    }

    setToolTip( _name + "\n" +
		formatSize( _parentView->percentile( percentileIndex ) ) );

    setPen( pen );
    // setFlags( ItemIsSelectable );
    _parentView->scene()->addItem( this );
}


QLineF PercentileMarker::translatedLine( const QLineF &	 zeroLine,
					 int		 percentileIndex,
					 HistogramView * parent ) const
{
    qreal value = parent->percentile( percentileIndex );
    qreal x	= parent->scaleValue( value );

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

