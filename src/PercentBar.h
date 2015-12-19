/*
 *   File name: PercentBar.h
 *   Summary:	Functions and item delegate for percent bar
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef PercentBar_h
#define PercentBar_h

#include <QStyledItemDelegate>
#include <QColor>
#include <QList>


typedef QList<QColor> ColorList;


namespace QDirStat
{
    class PercentBarDelegate: public QStyledItemDelegate
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 * 'percentBarCol' is the (view) column to paint the percent bar in.
	 **/
	PercentBarDelegate( QWidget * parent,
			    int	      percentBarCol );

	/**
	 * Destructor.
	 **/
	virtual ~PercentBarDelegate();

	/**
	 * Paint one cell in the view.
	 * Inherited from QStyledItemDelegate.
	 **/
	void paint( QPainter		       * painter,
		    const QStyleOptionViewItem & option,
		    const QModelIndex	       & index ) const Q_DECL_OVERRIDE;

	/**
	 * Return a size hint for one cell in the view.
	 * Inherited from QStyledItemDelegate.
	 **/
	QSize sizeHint( const QStyleOptionViewItem & option,
			const QModelIndex	   & index) const Q_DECL_OVERRIDE;
	/**
	 * Return the percent bar fill colors for each tree level. If there
	 * are more tree levels than colors, the colors will wrap around.
	 *
	 * This object reference can be used directly to add, remove or change
	 * colors.
	 **/
	ColorList & fillColors() { return _fillColors; }

	/**
	 * Return the percent bar column.
	 **/
	int percentBarCol() const { return _percentBarCol; }

	/**
	 * Set the percent bar column.
	 **/
	void setPercentBarCol( int newCol )
	    { _percentBarCol = newCol; }


    protected:

	ColorList _fillColors;
	int	  _percentBarCol;

    }; // class PercentBarDelegate


    /**
     * Paint a percent bar into a widget.
     * 'indent' is the number of pixels to indent the bar.
     **/
    void paintPercentBar( float		 percent,
			  QPainter *	 painter,
			  int		 indent,
			  const QRect  & cellRect,
			  const QColor & fillColor,
			  const QColor & barBackground	 );

    /**
     * Return a color that contrasts with 'contrastColor'.
     **/
    QColor contrastingColor( const QColor &desiredColor,
			     const QColor &contrastColor );

}      // namespace QDirStat

#endif // PercentBar_h
