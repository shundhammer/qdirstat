/*
 *   File name:	ktreemaptile.h
 *   Summary:	High level classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2003-01-07
 */


#ifndef KTreemapTile_h
#define KTreemapTile_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <qcanvas.h>
#include <qrect.h>
#include "kdirtreeiterators.h"


namespace KDirStat
{
    class KFileInfo;
    class KTreemapView;

    enum KOrientation
    {
	KTreemapHorizontal,
	KTreemapVertical,
	KTreemapAuto
    };


    /**
     * Helper class for cushioned treemaps: This class holds the polynome
     * parameters for the cushion surface. The height of each point of such a
     * surface is defined as:
     *
     *     z(x, y) = a*x^2 + b*y^2 + c*x + d*y
     * or
     *     z(x, y) = xx2*x^2 + yy2*y^2 + xx1*x + yy1*y
     *
     * to better keep track of which coefficient belongs where.
     **/
    class KCushionSurface
    {
    public:
	/**
	 * Constructor. All polynome coefficients are set to 0.
	 **/
	KCushionSurface();

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
	void addRidge( KOrientation dim, double height, const QRect & rect );

	/**
	 * Set the cushion's height.
	 **/
	void setHeight( double newHeight ) { _height = newHeight; }

	/**
	 * Returns the cushion's height.
	 **/
	double height() const { return _height; }

	/**
	 * Returns the polynomal coefficient of the second order for X direction.
	 **/
	double xx2() const { return _xx2; }

	/**
	 * Returns the polynomal coefficient of the first order for X direction.
	 **/
	double xx1() const { return _xx1; }

	/**
	 * Returns the polynomal coefficient of the second order for Y direction.
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

    }; // class KCushionSurface



    /**
     * This is the basic building block of a treemap view: One single tile of a
     * treemap. If it corresponds to a leaf in the tree, it will be visible as
     * one tile (one rectangle) of the treemap. If it has children, it will be
     * subdivided again.
     *
     * @short Basic building block of a treemap
     **/
    class KTreemapTile:	public QCanvasRectangle
    {
    public:

	/**
	 * Constructor: Create a treemap tile from 'fileinfo' that fits into a
	 * rectangle 'rect' inside 'parent'.
	 *
	 * 'orientation' is the direction for further subdivision. 'Auto'
	 * selects the wider direction inside 'rect'.
	 **/
	KTreemapTile( KTreemapView *		parentView,
		      KTreemapTile *		parentTile,
		      KFileInfo *		orig,
		      const QRect &		rect,
		      KOrientation		orientation = KTreemapAuto );

    protected:

	/**
	 * Alternate constructor: Like the above, but explicitly specify a
	 * cushion surface rather than using the parent's.
	 **/
	KTreemapTile( KTreemapView *		parentView,
		      KTreemapTile *		parentTile,
		      KFileInfo *		orig,
		      const QRect &		rect,
		      const KCushionSurface &	cushionSurface,
		      KOrientation		orientation = KTreemapAuto );

    public:
	/**
	 * Destructor.
	 **/
	virtual ~KTreemapTile();


	/**
	 * Returns the original @ref KFileInfo item that corresponds to this
	 * treemap tile.
	 **/
	KFileInfo * orig() const { return _orig; }

	/**
	 * Returns the parent @ref KTreemapView.
	 **/
	KTreemapView * parentView() const { return _parentView; }

	/**
	 * Returns the parent @ref KTreemapTile or 0 if there is none.
	 **/
	KTreemapTile * parentTile() const { return _parentTile; }

	/**
	 * Returns this tile's cushion surface parameters.
	 **/
	KCushionSurface & cushionSurface() { return _cushionSurface; }


    protected:

	/**
	 * Create children (sub-tiles) of this tile.
	 **/
	void createChildren	( const QRect &	rect,
				  KOrientation	orientation );

	/**
	 * Create children (sub-tiles) using the simple treemap algorithm:
	 * Alternate between horizontal and vertical subdivision in each
	 * level. Each child will get the entire height or width, respectively,
	 * of the specified rectangle. This algorithm is very fast, but often
	 * results in very thin, elongated tiles.
	 **/
	void createChildrenSimple( const QRect &	rect,
				   KOrientation		orientation );

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
	void createSquarifiedChildren( const QRect & rect );

	/**
	 * Squarify as many children as possible: Try to squeeze members
	 * referred to by 'it' into 'rect' until the aspect ratio doesn't get
	 * better any more. Returns a list of children that should be laid out
	 * in 'rect'. Moves 'it' until there is no more improvement or 'it'
	 * runs out of items.
	 *
	 * 'scale' is the scaling factor between file sizes and pixels.
	 **/
	KFileInfoList squarify( const QRect & 			rect,
				double				scale,
				KFileInfoSortedBySizeIterator & it   );

	/**
	 * Lay out all members of 'row' within 'rect' along its longer side.
	 * Returns the new rectangle with the layouted area subtracted.
	 **/
	QRect layoutRow( const QRect &		rect,
			 double			scale,
			 KFileInfoList & 	row );

	/**
	 * Draw the tile.
	 *
	 * Reimplemented from QCanvasRectangle.
	 **/
	virtual void drawShape( QPainter & painter );

	/**
	 * Render a cushion as described in "cushioned treemaps" by Jarke
	 * J. van Wijk and Huub van de Wetering  of the TU Eindhoven, NL.
	 **/
	QPixmap renderCushion();

	/**
	 * Check if the contrast of the specified image is sufficient to
	 * visually distinguish an outline at the right and bottom borders
	 * and add a grey line there, if necessary.
	 **/
	void ensureContrast( QImage & image );

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

	KTreemapView *	_parentView;
	KTreemapTile *	_parentTile;
	KFileInfo *	_orig;
	KCushionSurface	_cushionSurface;
	QPixmap		_cushion;

    }; // class KTreemapTile

}	// namespace KDirStat



inline kdbgstream & operator<< ( kdbgstream & stream, const QRect & rect )
{
    stream << "("
	   << rect.width() << "x" << rect.height()
	   << "+" << rect.x() << "+" << rect.y()
	   << ")";

    return stream;
}


#endif // ifndef KTreemapTile_h


// EOF
