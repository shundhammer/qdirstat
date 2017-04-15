/*
 *   File name: HistogramView.h
 *   Summary:	View widget for histogram rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef HistogramItems_h
#define HistogramItems_h

#include <QGraphicsRectItem>
#include <QGraphicsLineItem>

#include "HistogramItems.h"
#include "HistogramView.h"


namespace QDirStat
{
    /**
     * GraphicsItem class for a histogram bar.
     *
     * This creates an invisible full-height item so it is clickable, even for
     * very small values, and a visible child rectangle to display the correct
     * height.
     **/
    class HistogramBar: public QGraphicsRectItem
    {
    public:
	/**
	 * Constructor. 'number' is the number of the bar (0 being the
	 * leftmost) in the histogram.
	 **/
	HistogramBar( HistogramView * parent,
		      int	      number,
		      const QRectF &  rect,
		      qreal	      fillHeight );

	/**
	 * Return the number of this bar.
	 **/
	int number() const { return _number; }

    protected:
	/**
	 * Mouse press event
	 *
	 * Reimplemented from QGraphicsItem.
	 **/
	virtual void mousePressEvent( QGraphicsSceneMouseEvent * event ) Q_DECL_OVERRIDE;

	HistogramView * _parentView;
	int		_number;
	qreal		_startVal;
	qreal		_endVal;
    };


    /**
     * GraphicsItem for a percentile marker (including median or quartiles).
     **/
    class PercentileMarker: public QGraphicsLineItem
    {
    public:
	PercentileMarker( HistogramView * parent,
			  int		  percentileIndex,
			  const QString & name,
			  const QLineF &  zeroLine,
			  const QPen &	  pen );

	/**
	 * Return the name of this marker; something like "P1", "Min", "Max",
	 * "Median", "Q1", "Q3".
	 **/
	QString name() const { return _name; }

	/**
	 * Return the percentile index (0..100) for this marker.
	 **/
	int percentileIndex() const { return _percentileIndex; }

	/**
	 * Return the percentile value for this marker.
	 **/
	qreal value() const;

    protected:

	QLineF translatedLine( const QLineF &  zeroLine,
			       int	       percentileIndex,
			       HistogramView * parent ) const;

	/**
	 * Mouse press event
	 *
	 * Reimplemented from QGraphicsItem.
	 **/
	virtual void mousePressEvent( QGraphicsSceneMouseEvent * event ) Q_DECL_OVERRIDE;

	HistogramView * _parentView;
	QString		_name;
	int		_percentileIndex;
    };


}	// namespace QDirStat


#endif // ifndef HistogramItems_h
