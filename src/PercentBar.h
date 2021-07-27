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

#include "DirTreeView.h"
#include "DirTreeModel.h"       // RawDataRole


typedef QList<QColor> ColorList;
class QTreeView;


namespace QDirStat
{
    /**
     * Item delegate class to paint the percent bar in the PercentBarCol.
     *
     * This is a generic class that can be used for all kinds of QTreeView-
     * derived widgets, including the simplistic QTreeWidget.
     *
     * This delegate can handle one percent bar in one column; but you can
     * install multiple of them, one for each column that should get a percent
     * bar.
     *
     * The default behaviour is to use a percent value that is displayed as
     * text in that same column and render the percent bar instead. If there is
     * no suitable text with a percent number, it calls its superclass,
     * i.e. the text that is in that column (or nothing if there is no text at
     * all) is displayed.
     *
     * So, simply add a column for this percent bar and display the numeric
     * value (optionally followed by a '%' percent sign) in that column; the
     * delegate will display a graphical percent bar instead of that numeric
     * value.
     *
     * Example:
     *
     *    60.0%    ->   [======    ]
     *
     * If the numeric value should be displayed as well, add another column for
     * it, display the numeric value there as well, and don't install this
     * delegate for that other column.
     *
     * For indented tree levels, the percent bar is indented as well, and a
     * different color is used for each indentation level.
     *
     *
     * Notice that for QDirStat's DirTreeView and its DirTreeModel, there is a
     * special class DirTreePercentBarDelegate that is derived from this, but
     * reimplements the percentData() method to use the custom RawDataRole to
     * get the numeric value from the model.
     **/
    class PercentBarDelegate: public QStyledItemDelegate
    {
	Q_OBJECT

    public:

	/**
	 * Constructor. 'percentBarCol' is the column that this delegate is to
	 * paint the percent bar in.
         *
	 **/
	PercentBarDelegate( QTreeView * treeView, int percentBarCol );

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
	 * Return the default fill colors.
	 **/
	ColorList defaultFillColors() const;

        /**
         * Set the index of the starting color (default: 0).
         **/
        void setStartColorIndex( int index ) { _startColorIndex = index; }

        /**
         * Return the index of the starting color.
         **/
        int startColorIndex() const { return _startColorIndex; }


    public slots:

	/**
	 * Read parameters from the settings file.
	 **/
	void readSettings();

	/**
	 * Write parameters to the settings file.
	 **/
	void writeSettings();


    protected:

	/**
	 * Find out the tree depth level of item 'index' by following its
	 * parent, parent's parent etc. to the top.
	 **/
	int treeLevel( const QModelIndex & index ) const;

        /**
         * Return percentage data for the specified model index.
         * For valid data, this should return a float value from 0.0 to 100.0.
         *
         * This default implementation uses the default Qt::DisplayRole for
         * that model index, i.e. the value that would normally be displayed.
         * It any '%' percent sign and tries to convert the remainder to
         * 'float'.
         *
         * If that does not result in a valid floating point number, it calls
         * the parent class, i.e. the value in that cell is displayed normally,
         * not a percent bar.
         **/
        virtual QVariant percentData( const QModelIndex & index ) const;


	//
	// Data Members
	//

	QTreeView * _treeView;
        int         _percentBarCol;
        int         _invisibleLevels;
        int         _startColorIndex;
	ColorList   _fillColors;
	QColor	    _barBackground;
	int	    _sizeHintWidth;

    }; // class PercentBarDelegate


    /**
     * Specialized item delegate class to paint the percent bar in a
     * DirTreeView that uses a DirTreeModel. It paints the bar in the
     * PercentBarCol, and it fetches the percentage data from the
     * DirTreeModel's custom RawDataRole.
     *
     * This class is not usable outside the context of a DirTreeView and a
     * DirTreeModel.
     **/
    class DirTreePercentBarDelegate: public PercentBarDelegate
    {

    public:
	/**
	 * Constructor.
	 **/
	DirTreePercentBarDelegate( DirTreeView * treeView, int percentBarCol ):
            PercentBarDelegate( treeView, percentBarCol )
        {
            _invisibleLevels = 2;       // invisible root and invisible toplevel
        }

	/**
	 * Destructor.
	 **/
	virtual ~DirTreePercentBarDelegate() {}


    protected:

        /**
         * Get the percentage data for the specified model index. This is where
         * this class is different from the base class: It uses a special data
         * role that is specific to the DirTreeModel. This does not need to
         * parse strings, it receives numeric data directly.
         *
         * Reimplemented from PercentBarDelegate.
         **/
        virtual QVariant percentData( const QModelIndex & index ) const Q_DECL_OVERRIDE
            { return index.data( RawDataRole ); }

    };  // class DirTreePercentBarDelegate


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
