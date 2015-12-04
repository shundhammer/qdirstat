/*
 *   File name:	ktreemapview.h
 *   Summary:	High level classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifndef KTreemapView_h
#define KTreemapView_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <qcanvas.h>


#define MinAmbientLight			0
#define MaxAmbientLight			200
#define DefaultAmbientLight		40

#define	MinHeightScalePercent		10
#define	MaxHeightScalePercent		200
#define DefaultHeightScalePercent	100
#define DefaultHeightScaleFactor	( DefaultHeightScalePercent / 100.0 )

#define DefaultMinTileSize		3
#define CushionHeight			1.0


class QMouseEvent;
class KConfig;

namespace KDirStat
{
    class KTreemapTile;
    class KTreemapSelectionRect;
    class KDirTree;
    class KFileInfo;

    class KTreemapView:	public QCanvasView
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 **/
	KTreemapView( KDirTree * 	tree,
		      QWidget * 	parent 		= 0,
		      const QSize &	initialSize 	= QSize() );

	/**
	 * Destructor.
	 **/
	virtual ~KTreemapView();

	/**
	 * Returns the (topmost) treemap tile at the specified position
	 * or 0 if there is none.
	 **/
	KTreemapTile * tileAt( QPoint pos );

	/**
	 * Returns the minimum recommended size for this widget.
	 * Reimplemented from QWidget.
	 **/
	virtual QSize minimumSizeHint() const { return QSize( 0, 0 ); }

	/**
	 * Returns this treemap view's currently selected treemap tile or 0 if
	 * there is none.
	 **/
	KTreemapTile * selectedTile() const { return _selectedTile; }


	/**
	 * Returns this treemap view's root treemap tile or 0 if there is none.
	 **/
	KTreemapTile * rootTile() const { return _rootTile; }

	/**
	 * Returns this treemap view's @ref KDirTree.
	 **/
	KDirTree * tree() const { return _tree; }

	/**
	 * Search the treemap for a tile that corresponds to the specified
	 * KFileInfo node. Returns 0 if there is none.
	 *
	 * Notice: This is an expensive operation since all treemap tiles need
	 * to be searched.
	 **/
	KTreemapTile * findTile( KFileInfo * node );

	/**
	 * Returns a suitable color for 'file' based on a set of internal rules
	 * (according to filename extension, MIME type or permissions).
	 **/
	QColor tileColor( KFileInfo * file );


    public slots:

        /**
	 * Make a treemap tile this treemap's selected tile.
	 * 'tile' may be 0. In this case, only the previous selection is
	 * deselected.
	 **/
        void selectTile( KTreemapTile * tile );

	/**
	 * Search the treemap for a tile with the specified KFileInfo node and
	 * select that tile if it is found. If nothing is found or if 'node' is
	 * 0, the previously selected tile is deselected.
	 **/
        void selectTile( KFileInfo * node );

	/**
	 * Zoom in one level towards the currently selected treemap tile:
	 * The entire treemap will be rebuilt with the near-topmost ancestor of
	 * the selected tile as the new root.
	 **/
	void zoomIn();

	/**
	 * Zoom out one level: The parent (if there is any) KFileInfo node of
	 * the current treemap root becomes the new root. This usually works
	 * only after zoomIn().
	 **/
	void zoomOut();

	/**
	 * Select the parent of the currently selected tile (if possible).
	 *
	 * This is very much the same as clicking with the middle mouse button,
	 * but not quite: The middle mouse button cycles back to the tile
	 * clicked at if there is no more parent. This method does not (because
	 * there is no known mouse position).
	 **/
	void selectParent();

	/**
	 * Completely rebuild the entire treemap from the internal tree's root
	 * on.
	 **/
	void rebuildTreemap();

	/**
	 * Clear the treemap contents.
	 **/
	void clear();

	/**
	 * Delete all items of a QCanvas.
	 *
	 * Strangely enough, QCanvas itself does not provide such a function.
	 **/
	static void deleteAllItems( QCanvas * canvas );

	/**
	 * Notification that a dir tree node has been deleted.
	 **/
	void deleteNotify( KFileInfo * node );

	/**
	 * Read some parameters from the global @ref KConfig object.
	 **/
	void readConfig();

    public:

	/**
	 * Rebuild the treemap with 'newRoot' as the new root and the specified
	 * size. If 'newSize' is (0, 0), visibleSize() is used.
	 **/
	void rebuildTreemap( KFileInfo * 	newRoot,
			     const QSize &	newSize = QSize() );

	/**
	 * Returns the visible size of the viewport presuming no scrollbars are
	 * needed - which makes a lot more sense than fiddling with scrollbars
	 * since treemaps can be scaled to make scrollbars unnecessary.
	 **/
	QSize visibleSize();

	/**
	 * Returns the visible width of the viewport presuming no scrollbars
	 * are needed.
	 *
	 * This uses visibleSize() which is a somewhat expensive operation, so
	 * if you need both visibleWidth() and visibleHeight(), better call
	 * visibleSize() once and access its width() and height() methods.
	 **/
	int visibleWidth() { return visibleSize().width(); }

	/**
	 * Returns the visible height of the viewport presuming no scrollbars
	 * are needed.
	 *
	 * This uses visibleSize() which is a somewhat expensive operation, so
	 * if you need both visibleWidth() and visibleHeight(), better call
	 * visibleSize() once and access its width() and height() methods.
	 **/
	int visibleHeight() { return visibleSize().height(); }

	/**
	 * Returns true if it is possible to zoom in with the currently
	 * selected tile, false if not.
	 **/
	bool canZoomIn() const;

	/**
	 * Returns true if it is possible to zoom out with the currently
	 * selected tile, false if not.
	 **/
	bool canZoomOut() const;

	/**
	 * Returns true if it is possible to select the parent of the currently
	 * selected tile, false if not.
	 **/
	bool canSelectParent() const;

	/**
	 * Returns 'true' if the treemap is automatically resized to fit into
	 * the available space, 'false' if not.
	 **/
	bool autoResize() const { return _autoResize; }

	/**
	 * Returns 'true' if treemap tiles are to be squarified upon creation,
	 * 'false' if not.
	 **/
	bool squarify() const { return _squarify; }

	/**
	 * Returns 'true' if cushion shading is to be used, 'false' if not.
	 **/
	bool doCushionShading() const { return _doCushionShading; }

	/**
	 * Returns 'true' if cushion shaded treemap tiles are to be separated
	 * by a grid, 'false' if not.
	 **/
	bool forceCushionGrid() const { return _forceCushionGrid; }

	/**
	 * Returns 'true' if tile boundary lines should be drawn for cushion
	 * treemaps, 'false'  if not.
	 **/
	bool ensureContrast() const { return _ensureContrast; }

	/**
	 * Returns the minimum tile size in pixels. No treemap tiles less than
	 * this in width or height are desired.
	 **/
	int minTileSize() const { return _minTileSize; }

	/**
	 * Returns the cushion grid color.
	 **/
	const QColor & cushionGridColor() const { return _cushionGridColor; }

	/**
	 * Returns the outline color to use if cushion shading is not used.
	 **/
	const QColor & outlineColor() const { return _outlineColor; }

	/**
	 * Returns the fill color for non-directory treemap tiles when cushion
	 * shading is not used.
	 **/
	const QColor & fileFillColor() const { return _fileFillColor; }

	/**
	 * Returns the fill color for directory (or "dotentry") treemap tiles
	 * when cushion shading is not used.
	 **/
	const QColor & dirFillColor() const { return _dirFillColor; }

	/**
	 * Returns the intensity of ambient light for cushion shading 
	 * [0..255]
	 **/
	int ambientLight() const { return _ambientLight; }

	/**
	 * Returns the X coordinate of a directed light source for cushion
	 * shading.
	 **/

	double lightX() const { return _lightX; }

	/**
	 * Returns the Y coordinate of a directed light source for cushion
	 * shading.
	 **/
	double lightY() const { return _lightY; }

	/**
	 * Returns the Z coordinate of a directed light source for cushion
	 * shading.
	 **/
	double lightZ() const { return _lightZ; }

	/**
	 * Returns cushion ridge height degradation factor (0 .. 1.0) for each
	 * level of subdivision.
	 **/
	double heightScaleFactor() const { return _heightScaleFactor; }


    signals:

	/**
	 * Emitted when the currently selected item changes.
	 * Caution: 'item' may be 0 when the selection is cleared.
	 **/
	void selectionChanged( KFileInfo * item );

	/**
	 * Emitted when the treemap changes, e.g. is rebuilt, zoomed in, or
	 * zoomed out.
	 **/
	void treemapChanged();

	/**
	 * Emitted when a context menu for this tile should be opened.
	 * (usually on right click). 'pos' contains the click's mouse
	 * coordinates.
	 **/
	void contextMenu( KTreemapTile * tile, const QPoint & pos );

	/**
	 * Emitted at user activity. Some interactive actions are assigned an
	 * amount of "activity points" that can be used to judge whether or not
	 * the user is actually using this program or if it's just idly sitting
	 * around on the desktop. This is intended for use together with a @ref
	 * KActivityTracker.
	 **/
	void userActivity( int points );


    protected:

	/**
	 * Catch mouse click - emits a selectionChanged() signal.
	 **/
	virtual void contentsMousePressEvent( QMouseEvent * event );

	/**
	 * Catch mouse double click:
	 * 	Left   button double-click zooms in,
	 *	right  button double-click zooms out,
	 * 	middle button double-click rebuilds treemap.
	 **/
	virtual void contentsMouseDoubleClickEvent( QMouseEvent * event );

	/**
	 * Resize the treemap view. Suppress the treemap contents if the size
	 * falls below a minimum size, redisplay it if it grows above that
	 * minimum size.
	 *
	 * Reimplemented from QFrame.
	 **/
	virtual void resizeEvent( QResizeEvent * event );

	/**
	 * Convenience method to read a color from 'config'.
	 **/
	QColor readColorEntry( KConfig * 	config,
			       const char * 	entryName,
			       QColor 		defaultColor );

	// Data members

	KDirTree *		_tree;
	KTreemapTile *		_rootTile;
	KTreemapTile * 		_selectedTile;
	KTreemapSelectionRect *	_selectionRect;
	QString			_savedRootUrl;

	bool			_autoResize;
	bool			_squarify;
	bool			_doCushionShading;
	bool			_forceCushionGrid;
	bool			_ensureContrast;
	int			_minTileSize;

	QColor			_highlightColor;
	QColor			_cushionGridColor;
	QColor			_outlineColor;
	QColor			_fileFillColor;
	QColor			_dirFillColor;

	int			_ambientLight;

	double			_lightX;
	double			_lightY;
	double			_lightZ;

	double 			_heightScaleFactor;

    }; // class KTreemapView



    /**
     * Transparent rectangle to make a treemap tile clearly visible as
     * "selected". Leaf tiles could do that on their own, but higher-level
     * tiles (corresponding to directories) are obscured for the most part, so
     * only a small portion (if any) of their highlighted outline could be
     * visible. This selection rectangle simply draws a two-pixel red outline
     * on top (i.e., great z-height) of everything else. The rectangle is
     * transparent, so the treemap tile contents remain visible.
     **/
    class KTreemapSelectionRect: public QCanvasRectangle
    {
    public:

	/**
	 * Constructor.
	 **/
	KTreemapSelectionRect( QCanvas * canvas, const QColor & color );

	/**
	 * Highlight the specified treemap tile: Resize this selection
	 * rectangle to match this tile and move it to this tile's
	 * position. Show the selection rectangle if it is currently
	 * invisible.
	 **/
	void highlight( KTreemapTile * tile );

    }; // class KTreemapSelectionRect

}	// namespace KDirStat


#endif // ifndef KTreemapView_h


// EOF
