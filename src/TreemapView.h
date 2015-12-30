/*
 *   File name: TreemapView.h
 *   Summary:	View widget for treemap rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef TreemapView_h
#define TreemapView_h


#include <QGraphicsView>
#include <QGraphicsRectItem>

#include "FileInfo.h"


#define MinAmbientLight		   0
#define MaxAmbientLight		   200
#define DefaultAmbientLight	   40

#define MinHeightScalePercent	   10
#define MaxHeightScalePercent	   200
#define DefaultHeightScalePercent  100
#define DefaultHeightScaleFactor   ( DefaultHeightScalePercent / 100.0 )

#define DefaultMinTileSize	   3
#define CushionHeight		   1.0


class QMouseEvent;


namespace QDirStat
{
    class TreemapTile;
    class HighlightRect;
    class DirTree;
    class SelectionModel;
    class SelectionModelProxy;


    /**
     * View widget that displays a DirTree as a treemap.
     **/
    class TreemapView:	public QGraphicsView
    {
	Q_OBJECT

    public:
	/**
	 * Constructor. Remember to set the directory tree with setDirTree()
	 * and the selection model with setSelectionModel() after creating this
	 * widget.
	 **/
	TreemapView( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~TreemapView();

	/**
	 * Set the directory tree to work on. Without this, this widget will
	 * not display anything.
	 **/
	void setDirTree( DirTree * tree );

	/**
	 * Set the selection model. This is important to synchronize current /
	 * selected items between a DirTreeView and this TreemapView.
	 **/
	void setSelectionModel( SelectionModel * selectionModel );

	/**
	 * Returns the (topmost) treemap tile at the specified position
	 * or 0 if there is none.
	 **/
	TreemapTile * tileAt( QPoint pos );

	/**
	 * Returns the minimum recommended size for this widget.
	 * Reimplemented from QWidget.
	 **/
	virtual QSize minimumSizeHint() const { return QSize( 0, 0 ); }

	/**
	 * Returns this treemap view's current item tile or 0 if there is none.
	 **/
	TreemapTile * currentItem() const { return _currentItem; }

	/**
	 * Returns this treemap view's root treemap tile or 0 if there is none.
	 **/
	TreemapTile * rootTile() const { return _rootTile; }

	/**
	 * Returns this treemap view's @ref DirTree.
	 **/
	DirTree * tree() const { return _tree; }

	/**
	 * Search the treemap for a tile that corresponds to the specified
	 * FileInfo node. Returns 0 if there is none.
	 *
	 * Notice: This is an expensive operation since all treemap tiles need
	 * to be searched.
	 **/
	TreemapTile * findTile( FileInfo * node );

	/**
	 * Returns a suitable color for 'file' based on a set of internal rules
	 * (according to filename extension, MIME type or permissions).
	 **/
	QColor tileColor( FileInfo * file );


    public slots:

	/**
	 * Update the selected items that have been selected in another view.
	 **/
	void updateSelection( const FileInfoSet & newSelection );

	/**
	 * Update the current item that has been changed in another view.
	 **/
	void updateCurrentItem( FileInfo * currentItem );

	/**
	 * Make a treemap tile this treemap's current item.
	 * 'tile' may be 0. In this case, only the previous selection is
	 * deselected.
	 **/
	void setCurrentItem( TreemapTile * tile );

	/**
	 * Search the treemap for a tile with the specified FileInfo node and
	 * make that tile the current item if it is found. If nothing is found
	 * or if 'node' is 0, the highlighting is removed from the previous
	 * current item.
	 **/
	void setCurrentItem( FileInfo * node );

	/**
	 * Zoom in one level towards the currently selected treemap tile:
	 * The entire treemap will be rebuilt with the near-topmost ancestor of
	 * the selected tile as the new root.
	 **/
	void zoomIn();

	/**
	 * Zoom out one level: The parent (if there is any) FileInfo node of
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
	 * Notification that a dir tree node has been deleted.
	 **/
	void deleteNotify( FileInfo * node );

        /**
         * Sync the selected items and the current item to the selection model.
         **/
        void sendSelection();

	/**
	 * Read some parameters from the config file
	 **/
	void readConfig();

    public:

	/**
	 * Rebuild the treemap with 'newRoot' as the new root and the specified
	 * size. If 'newSize' is (0, 0), visibleSize() is used.
	 **/
	void rebuildTreemap( FileInfo *	    newRoot,
			     const QSizeF & newSize = QSize() );

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
	 * Return the frame color for selected items.
	 **/
	const QColor & selectedItemsColor() const { return _selectedItemsColor; }

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
	void selectionChanged( FileInfo * item );

	/**
	 * Emitted when the current item changes.
	 **/
	void currentItemChanged( FileInfo * newCurrent );

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
	void contextMenu( TreemapTile * tile, const QPoint & pos );


    protected:

#if 0
	/**
	 * Catch mouse click - emits a selectionChanged() signal.
	 **/
	virtual void contentsMousePressEvent( QMouseEvent * event );

	/**
	 * Catch mouse double click:
	 *	Left   button double-click zooms in,
	 *	right  button double-click zooms out,
	 *	middle button double-click rebuilds treemap.
	 **/
	virtual void contentsMouseDoubleClickEvent( QMouseEvent * event );
#endif

	/**
	 * Resize the treemap view. Suppress the treemap contents if the size
	 * falls below a minimum size, redisplay it if it grows above that
	 * minimum size.
	 *
	 * Reimplemented from QFrame.
	 **/
	virtual void resizeEvent( QResizeEvent * event );

#if 0
	/**
	 * Convenience method to read a color from 'config'.
	 **/
	QColor readColorEntry( KConfig *    config,
			       const char * entryName,
			       QColor	    defaultColor );
#endif

	// Data members

	DirTree		    * _tree;
	SelectionModel	    * _selectionModel;
	SelectionModelProxy * _selectionModelProxy;
	TreemapTile	    * _rootTile;
	TreemapTile	    * _currentItem;
	HighlightRect	    * _currentItemRect;
	QString		      _savedRootUrl;

	bool   _squarify;
	bool   _doCushionShading;
	bool   _forceCushionGrid;
	bool   _ensureContrast;
	int    _minTileSize;

	QColor _currentItemColor;
	QColor _selectedItemsColor;
	QColor _cushionGridColor;
	QColor _outlineColor;
	QColor _fileFillColor;
	QColor _dirFillColor;

	int    _ambientLight;

	double _lightX;
	double _lightY;
	double _lightZ;

	double _heightScaleFactor;

    }; // class TreemapView



    /**
     * Transparent rectangle to make a treemap tile clearly visible as the
     * current item or as selected.
     *
     * Leaf tiles can do that themselves, but directory tiles are typically
     * completely obscured by their children, so no highlight border they draw
     * themselves will ever become visible.
     *
     * This highlight rectangle simply draws a colored red outline on top
     * (i.e., great z-height) of everything else. The rectangle is transparent,
     * so the treemap tile contents remain visible.
     **/
    class HighlightRect: public QGraphicsRectItem
    {
    public:

	/**
	 * Create a highlight rectangle for the entire scene. This is most
	 * useful for the current item.
	 **/
	HighlightRect( QGraphicsScene * scene, const QColor & color, int lineWidth = 2 );

	/**
	 * Create a highlight rectangle for one specific tile and highlight it
	 * right away. This is most useful for selected items if more than one
	 * item is selected. The z-height of this is lower than for the
	 * scene-wide highlight rectangle.
	 **/
	HighlightRect( TreemapTile * tile, const QColor & color, int lineWidth = 2 );

	/**
	 * Highlight the specified treemap tile: Resize this selection
	 * rectangle to match this tile and move it to this tile's
	 * position. Show the selection rectangle if it is currently
	 * invisible.
	 **/
	void highlight( TreemapTile * tile );

    }; // class TreemapSelectionRect

}	// namespace QDirStat


#endif // ifndef TreemapView_h

