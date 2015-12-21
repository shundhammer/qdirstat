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
class QTreeView;


namespace QDirStat
{
    /**
     * Item delegate class to paint the percent bar in the PercentBarCol.
     *
     * This class uses the custom RawDataRole to obtain the data (the subtree
     * percent) in numeric format from the model.
     **/
    class PercentBarDelegate: public QStyledItemDelegate
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 * 'percentBarCol' is the (view) column to paint the percent bar in.
	 **/
	PercentBarDelegate( QTreeView * treeView,
			    int		percentBarCol );

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

	/**
	 * Find out the tree depth level of item 'index' by following its
	 * parent, parent's parent etc. to the top.
	 **/
	int treeLevel( const QModelIndex & index ) const;

	//
	// Data Members
	//

	QTreeView * _treeView;
	ColorList   _fillColors;
	QColor	    _barBackground;
	int	    _percentBarCol;

    }; // class PercentBarDelegate


    /**
     * Paint a percent bar into a widget.
     * 'indentPixel' is the number of pixels to indent the bar.
     **/
    void paintPercentBar( float		 percent,
			  QPainter *	 painter,
			  int		 indentPixel,
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
