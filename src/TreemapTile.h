/*
 *   File name: TreemapTile.h
 *   Summary:	Treemap rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef TreemapTile_h
#define TreemapTile_h


#include <QGraphicsRectItem>
#include <QRectF>

#include "FileInfoIterator.h"


class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent;


namespace QDirStat
{
    class FileInfo;
    class TreemapView;
    class HighlightRect;

    enum Orientation
    {
	TreemapHorizontal,
	TreemapVertical,
	TreemapAuto
    };


    /**
     * Helper class for cushioned treemaps: This class holds the polynome
     * parameters for the cushion surface. The height of each point of such a
     * surface is defined as:
     *
     *	   z(x, y) = a*x^2 + b*y^2 + c*x + d*y
     * or
     *	   z(x, y) = xx2*x^2 + yy2*y^2 + xx1*x + yy1*y
     *
     * to better keep track of which coefficient belongs where.
     **/
    class CushionSurface
    {
    public:
	/**
	 * Constructor. All polynome coefficients are set to 0.
	 **/
	CushionSurface();

	/**
	 * Adds a ridge of the specified height in dimension 'dim' within
	 * rectangle 'rect' to this surface. It's real voodo magic.
	 *
	 * Just kidding - read the paper about "cushion treemaps" by Jarke
	 * J. van Wiik and Huub van de Wetering from the TU Eindhoven, NL for
	 * more details.
	 *
	 * If you don't want to get all that involved: The coefficients are
	 * changed in some way.
	 **/
	void addRidge( Orientation dim, double height, const QRectF & rect );

	/**
	 * Set the cushion's height.
	 **/
	void setHeight( double newHeight ) { _height = newHeight; }

	/**
	 * Returns the cushion's height.
	 **/
	double height() const { return _height; }

	/**
	 * Returns the polynomal coefficient of the second order for X
	 * direction.
	 **/
	double xx2() const { return _xx2; }

	/**
	 * Returns the polynomal coefficient of the first order for X direction.
	 **/
	double xx1() const { return _xx1; }

	/**
	 * Returns the polynomal coefficient of the second order for Y
	 * direction.
	 **/
	double yy2() const { return _yy2; }

	/**
	 * Returns the polynomal coefficient of the first order for Y direction.
	 **/
	double yy1() const { return _yy1; }


    protected:

	/**
	 * Calculate a new square polynomal coefficient for adding a ridge of
	 * specified height between x1 and x2.
	 **/
	double squareRidge( double squareCoefficient, double height, int x1, int x2 );

	/**
	 * Calculate a new linear polynomal coefficient for adding a ridge of
	 * specified height between x1 and x2.
	 **/
	double linearRidge( double linearCoefficient, double height, int x1, int x2 );


	// Data members

	double _xx2, _xx1;
	double _yy2, _yy1;
	double _height;

    }; // class CushionSurface



    /**
     * This is the basic building block of a treemap view: One single tile of a
     * treemap. If it corresponds to a leaf in the tree, it will be visible as
     * one tile (one rectangle) of the treemap. If it has children, it will be
     * subdivided again.
     *
     * @short Basic building block of a treemap
     **/
    class TreemapTile:	public QGraphicsRectItem
    {
    public:

	/**
	 * Constructor: Create a treemap tile from 'fileinfo' that fits into a
	 * rectangle 'rect' inside 'parent'.
	 *
	 * 'orientation' is the direction for further subdivision. 'Auto'
	 * selects the wider direction inside 'rect'.
	 **/
	TreemapTile( TreemapView  * parentView,
		     TreemapTile  * parentTile,
		     FileInfo	  * orig,
		     const QRectF & rect,
		     Orientation    orientation = TreemapAuto );

    protected:

	/**
	 * Alternate constructor: Like the above, but explicitly specify a
	 * cushion surface rather than using the parent's.
	 **/
	TreemapTile( TreemapView	  * parentView,
		     TreemapTile	  * parentTile,
		     FileInfo		  * orig,
		     const QRectF	  & rect,
		     const CushionSurface & cushionSurface,
		     Orientation	    orientation = TreemapAuto );

    public:
	/**
	 * Destructor.
	 **/
	virtual ~TreemapTile();

	/**
	 * Returns the original FileInfo item that corresponds to this
	 * treemap tile.
	 **/
	FileInfo * orig() const { return _orig; }

	/**
	 * Returns the parent TreemapView.
	 **/
	TreemapView * parentView() const { return _parentView; }

	/**
	 * Returns the parent TreemapTile or 0 if there is none.
	 **/
	TreemapTile * parentTile() const { return _parentTile; }

	/**
	 * Returns this tile's cushion surface parameters.
	 **/
	CushionSurface & cushionSurface() { return _cushionSurface; }


    protected:

	/**
	 * Create children (sub-tiles) of this tile.
	 **/
	void createChildren( const QRectF & rect,
			     Orientation    orientation );

	/**
	 * Create children (sub-tiles) using the simple treemap algorithm:
	 * Alternate between horizontal and vertical subdivision in each
	 * level. Each child will get the entire height or width, respectively,
	 * of the specified rectangle. This algorithm is very fast, but often
	 * results in very thin, elongated tiles.
	 **/
	void createChildrenSimple( const QRectF & rect,
				   Orientation	  orientation );

	/**
	 * Create children using the "squarified treemaps" algorithm as
	 * described by Mark Bruls, Kees Huizing, and Jarke J. van Wijk of the
	 * TU Eindhoven, NL.
	 *
	 * This algorithm is not quite so simple and involves more expensive
	 * operations, e.g., sorting the children of each node by size first,
	 * try some variations of the layout and maybe backtrack to the
	 * previous attempt. But it results in tiles that are much more
	 * square-like, i.e. have more reasonable width-to-height ratios. It is
	 * very much less likely to get thin, elongated tiles that are hard to
	 * point at and even harder to compare visually against each other.
	 *
	 * This implementation includes some improvements to that basic
	 * algorithm. For example, children below a certain size are
	 * disregarded completely since they will not get an adequate visual
	 * representation anyway (it would be way too small). They are
	 * summarized in some kind of 'misc stuff' area in the parent treemap
	 * tile - in fact, part of the parent directory's tile can be "seen
	 * through".
	 *
	 * In short, a lot of small children that don't have any useful effect
	 * for the user in finding wasted disk space are omitted from handling
	 * and, most important, don't need to be sorted by size (which has a
	 * cost of O(n*ln(n)) in the best case, so reducing n helps a lot).
	 **/
	void createSquarifiedChildren( const QRectF & rect );

	/**
	 * Squarify as many children as possible: Try to squeeze members
	 * referred to by 'it' into 'rect' until the aspect ratio doesn't get
	 * better any more. Returns a list of children that should be laid out
	 * in 'rect'. Moves 'it' until there is no more improvement or 'it'
	 * runs out of items.
	 *
	 * 'scale' is the scaling factor between file sizes and pixels.
	 **/
	FileInfoList squarify( const QRectF & rect,
			       double	     scale,
			       FileInfoSortedBySizeIterator & it   );

	/**
	 * Lay out all members of 'row' within 'rect' along its longer side.
	 * Returns the new rectangle with the layouted area subtracted.
	 **/
	QRectF layoutRow( const QRectF	& rect,
			  double	  scale,
			  FileInfoList	& row );

	/**
	 * Paint this tile.
	 *
	 * Reimplemented from QGraphicsRectItem.
	 **/
	virtual void paint( QPainter			   * painter,
			    const QStyleOptionGraphicsItem * option,
			    QWidget			   * widget = 0) Q_DECL_OVERRIDE;

	/**
	 * Notification that item attributes (such as the 'selected' state)
	 * have changed.
	 *
	 * Reimplemented from QGraphicsItem.
	 **/
	virtual QVariant itemChange( GraphicsItemChange	  change,
				     const QVariant	& value) Q_DECL_OVERRIDE;

	/**
	 * Mouse press event: Handle setting the current item.
	 *
	 * Reimplemented from QGraphicsItem.
	 **/
	virtual void mousePressEvent( QGraphicsSceneMouseEvent * event ) Q_DECL_OVERRIDE;

	/**
	 * Mouse release event: Handle marking item selection.
	 *
	 * Reimplemented from QGraphicsItem.
	 **/
	virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event ) Q_DECL_OVERRIDE;

	/**
	 * Mouse double click event:
	 *	Left   button double-click zooms in,
	 *	right  button double-click zooms out,
	 *	middle button double-click rebuilds treemap.
	 *
	 * Reimplemented from QGraphicsItem.
	 **/
	virtual void mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event ) Q_DECL_OVERRIDE;

	/**
	 * Mouse wheel event: Zoom in or out
	 *
	 * Reimplemented from QGraphicsItem.
	 **/
	virtual void wheelEvent( QGraphicsSceneWheelEvent * event) Q_DECL_OVERRIDE;

	/**
	 * Context menu event.
	 *
	 * Reimplemented from QGraphicsItem.
	 **/
	virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event ) Q_DECL_OVERRIDE;

        /**
         * Hover enter event.
	 *
	 * Reimplemented from QGraphicsItem.
         **/
        virtual void hoverEnterEvent( QGraphicsSceneHoverEvent * event ) Q_DECL_OVERRIDE;

        /**
         * Hover leave event.
	 *
	 * Reimplemented from QGraphicsItem.
         **/
        virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent * event ) Q_DECL_OVERRIDE;

	/**
	 * Render a cushion as described in "cushioned treemaps" by Jarke
	 * J. van Wijk and Huub van de Wetering	 of the TU Eindhoven, NL.
	 **/
	QPixmap renderCushion();

	/**
	 * Check if the contrast of the specified image is sufficient to
	 * visually distinguish an outline at the right and bottom borders
	 * and add a grey line there, if necessary.
	 **/
	void enforceContrast( QImage & image );

	/**
	 * Returns a color that gives a reasonable contrast to 'col': Lighter
	 * if 'col' is dark, darker if 'col' is light.
	 **/
	QRgb contrastingColor( QRgb col );

    private:

	/**
	 * Initialization common to all constructors.
	 **/
	void init();


    protected:

	// Data members

	TreemapView *	_parentView;
	TreemapTile *	_parentTile;
	FileInfo *	_orig;
	CushionSurface	_cushionSurface;
	QPixmap		_cushion;
	HighlightRect * _highlighter;

    }; // class TreemapTile



    inline QTextStream & operator<< ( QTextStream & stream, TreemapTile * tile )
    {
	if ( tile )
	    stream << tile->orig();
	else
	    stream << "<NULL TreemapTile *>";

	return stream;
    }

}	// namespace QDirStat



#endif // ifndef TreemapTile_h

