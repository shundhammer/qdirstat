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
#include <QGraphicsPathItem>
#include <QList>

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
class QSettings;


namespace QDirStat
{
    class TreemapTile;
    class HighlightRect;
    class SceneMask;
    class DirTree;
    class SelectionModel;
    class SelectionModelProxy;
    class CleanupCollection;
    class FileInfoSet;
    class DelayedRebuilder;

    typedef QList<HighlightRect *> HighlightRectList;


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
         * Returns the currently highlighted treemap tile (that was highlighted
         * with a middle click) or 0 if there is none.
         **/
        TreemapTile * highlightedTile() const { return _highlightedTile; }

	/**
	 * Returns this treemap view's DirTree.
	 **/
	DirTree * tree() const { return _tree; }

	/**
	 * Set the selection model. This is important to synchronize current /
	 * selected items between a DirTreeView and this TreemapView.
	 **/
	void setSelectionModel( SelectionModel * selectionModel );

	/**
	 * Return this treemap view's SelectionModel.
	 **/
	SelectionModel * selectionModel() const { return _selectionModel; }

	/**
	 * Set the cleanup collection. If set, all cleanup actions from that
	 * collection will be added to the item context menu.
	 **/
	void setCleanupCollection( CleanupCollection * collection )
	    { _cleanupCollection = collection; }

	/**
	 * Return the cleanup collection or 0 if it is not set.
	 **/
	CleanupCollection * cleanupCollection() const { return _cleanupCollection; }

	/**
	 * Search the treemap for a tile that corresponds to the specified
	 * FileInfo node. Returns 0 if there is none.
	 *
	 * Notice: This is an expensive operation since all treemap tiles need
	 * to be searched.
	 **/
	TreemapTile * findTile( const FileInfo * node );

	/**
	 * Returns a suitable color for 'file' based on a set of internal rules
	 * (according to filename extension, MIME type or permissions).
	 **/
	QColor tileColor( FileInfo * file );

	/**
	 * Use a fixed color for all tiles. To undo this, set an invalid QColor
	 * with the QColor default constructor.
	 **/
	void setFixedColor( const QColor & fixedColor );

        /**
         * Return the tile of the deepest-level highlighted parent or 0 if no
         * parent is currently highlighted. Notice that this returns the real
         * tile corresponding to a directory, not the HighlightRect.
         **/
        TreemapTile * highlightedParent() const;


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
	 * Reset the zoom level: Zoom out as far as possible.
	 **/
	void resetZoom();

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
	 * Disable this treemap view: Clear its contents, resize it to below
	 * the update threshold and hide it.
	 **/
	void disable();

	/**
	 * Re-enable this treemap view after disabling it: Show it, resize it
	 * to half the height of its parent widget and redisplay its content.
	 **/
	void enable();

	/**
	 * Notification that a dir tree node has been deleted.
	 **/
	void deleteNotify( FileInfo * node );

	/**
	 * Sync the selected items and the current item to the selection model.
	 **/
	void sendSelection();

        /**
         * Send a hoverEnter() signal for 'node'.
         **/
        void sendHoverEnter( FileInfo * node );

        /**
         * Send a hoverLeave() signal for 'node'.
         **/
        void sendHoverLeave( FileInfo * node );

        /**
         * Highlight the parent tiles of item 'tile'.
         **/
        void highlightParents( TreemapTile * tile );

        /**
         * Clear previous parent highlights.
         **/
        void clearParentsHighlight();

        /**
         * Highlight the parent tiles of item 'tile' if that tile is not
         * currently highlighted, or clear the highlight if it is.
         **/
        void toggleParentsHighlight( TreemapTile * tile );

        /**
         * Clear the old scene mask if there is one.
         **/
        void clearSceneMask();

	/**
	 * Read parameters from the settings file.
	 **/
	void readSettings();

	/**
	 * Write parameters to the settings file.
	 *
	 * Unlike other classes in this program, this is not done from the
	 * corresponding settings dialog - because there is none. The settings
	 * here are very obscure - strictly for experts; nothing to bother a
	 * normal user with.
	 *
	 * Experts can edit them in the settings file, typically in
	 * ~/.config/QDirStat/QDirStat.conf ; this class writes the settings to
	 * that file in its destructor so those experts can find and edit them.
	 *
	 * If you misconfigured things and want to go back to the defaults,
	 * simply delete that one setting or the section in the settings or the
	 * complete settings file; missing settings are restored to the
	 * defaults when the program exits the next time.
	 **/
	void writeSettings();

    public:

	/**
	 * Rebuild the treemap with 'newRoot' as the new root and the specified
	 * size. If 'newSize' is (0, 0), visibleSize() is used.
	 **/
	void rebuildTreemap( FileInfo *	    newRoot,
			     const QSizeF & newSize = QSize() );

	/**
	 * Schedule a rebuild of the treemap with 'newRoot'. If another rebuild
	 * is scheduled before the timeout is over, nothing will happen until
	 * the last scheduled timeout has elapsed.
	 *
	 * The purpose of this is to avoid unnecessary rebuilds when the user
	 * resizes the window or the treemap subwindow: Only the last rebuild
	 * is really executed (when all the resizing has settled somehow).
	 **/
	void scheduleRebuildTreemap( FileInfo * newRoot );

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
	 * Returns 'true' if treemap tiles are to be squarified upon creation,
	 * 'false' if not.
	 **/
	bool squarify() const { return _squarify; }

	/**
	 * Returns 'true' if cushion shading is to be used, 'false' if not.
	 **/
	bool doCushionShading() const { return _doCushionShading; }

        /**
         * Returns 'true' if directories should be rendered with a gradient,
         * 'false' if not.
         **/
        bool useDirGradient() const { return _useDirGradient; }

	/**
	 * Returns 'true' if cushion shaded treemap tiles are to be separated
	 * by a grid, 'false' if not.
	 **/
	bool forceCushionGrid() const { return _forceCushionGrid; }

	/**
	 * Returns 'true' if tile boundary lines should be drawn for cushion
	 * treemaps, 'false'  if not.
	 **/
	bool enforceContrast() const { return _enforceContrast; }

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
         * Returns the start color for directory (or "dotentry") treemap tiles
         * if a directory gradient should be used.
         **/
        const QColor & dirGradientStart() const { return _dirGradientStart; }

        /**
         * Returns the end color for directory (or "dotentry") treemap tiles
         * if a directory gradient should be used.
         **/
        const QColor & dirGradientEnd() const { return _dirGradientEnd; }


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

        /**
         * Emitted when the mouse cursor enters a hover over 'item'.
         **/
        void hoverEnter( FileInfo * item );

        /**
         * Emitted when the mouse cursor leaves the hover over 'item'.
         **/
        void hoverLeave( FileInfo * item );


    protected slots:

	/**
	 * Rebuild the treemap if no more pending rebuilds are scheduled.
	 **/
	void rebuildTreemapDelayed();

    protected:

	/**
	 * Resize the treemap view. Suppress the treemap contents if the size
	 * falls below a minimum size, redisplay it if it grows above that
	 * minimum size.
	 *
	 * Reimplemented from QFrame.
	 **/
	virtual void resizeEvent( QResizeEvent * event ) Q_DECL_OVERRIDE;


	// Data members

	DirTree		    * _tree;
	SelectionModel	    * _selectionModel;
	SelectionModelProxy * _selectionModelProxy;
	CleanupCollection   * _cleanupCollection;
        DelayedRebuilder    * _rebuilder;
	TreemapTile	    * _rootTile;
	TreemapTile	    * _currentItem;
	HighlightRect	    * _currentItemRect;
        SceneMask           * _sceneMask;
	FileInfo	    * _newRoot;
        TreemapTile         * _highlightedTile;
        HighlightRectList     _parentHighlightList;
	QString		      _savedRootUrl;

	bool   _squarify;
	bool   _doCushionShading;
	bool   _forceCushionGrid;
	bool   _enforceContrast;
	bool   _useFixedColor;
	int    _minTileSize;
        bool   _useDirGradient;

	QColor _currentItemColor;
	QColor _selectedItemsColor;
	QColor _cushionGridColor;
	QColor _outlineColor;
	QColor _fileFillColor;
	QColor _dirFillColor;
        QColor _dirGradientStart;
        QColor _dirGradientEnd;
	QColor _fixedColor;

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
	virtual void highlight( TreemapTile * tile );

	/**
	 * Set the pen style. Recommended: Qt::SolidLine or Qt::DotLine.
	 **/
	void setPenStyle( Qt::PenStyle style = Qt::SolidLine );

	/**
	 * Set the pen style according to the 'selected' status of 'tile'.
	 **/
	void setPenStyle( TreemapTile * tile );

        /**
         * Return the tile that this highlights or 0 if there is none.
         **/
        TreemapTile * tile() const { return _tile; }

        /**
         * Return the shape of this item; in this case only the outline,
         * leaving the inside hollow to avoid displaying the tooltip there as
         * well.
         *
         * Reimplemented from QGraphicsRectItem / QGraphicsItem.
         **/
        virtual QPainterPath shape() const Q_DECL_OVERRIDE;

    protected:

        TreemapTile * _tile;

    }; // class TreemapSelectionRect


    /**
     * Highlighter for the treemap view's current tile.
     * This one is shared; it moves around from tile to tile.
     **/
    class CurrentItemHighlighter: public HighlightRect
    {
    public:
	CurrentItemHighlighter( QGraphicsScene * scene, const QColor & color, int lineWidth = 2 ):
	    HighlightRect( scene, color, lineWidth )
	    {}

	virtual void highlight( TreemapTile * tile );
    };


    /**
     * Highlighter for the treemap view's current item.
     *
     * This one is created on demand for each directory when the directory is
     * selected; this cannot be done in the tile's paint() method since the
     * tile will mostly be obscured by its children. This highlighter hovers
     * above the children as long as the directory is selected.
     **/
    class SelectedItemHighlighter: public HighlightRect
    {
    public:
	SelectedItemHighlighter( TreemapTile * tile, const QColor & color, int lineWidth = 2 ):
	    HighlightRect( tile, color, lineWidth )
	    {}
    };



    /**
     * Semi-transparent mask that covers the complete scene except for one
     * tile.
     **/
    class SceneMask: public QGraphicsPathItem
    {
    public:
        /**
         * Constructor: Create a semi-transparent mask that covers the complete
         * scene (the complete treemap), but leaves 'tile' uncovered.
         *
         * 'opacity' (0.0 .. 1.0) indicates how transparent the mask is:
         * 0.0 -> completely transparent; 1.0 -> solid.
         **/
        SceneMask( TreemapTile * tile, float opacity );

        /**
         * Return the tile that this masks.
         **/
        TreemapTile * tile() const { return _tile; }

    protected:

        TreemapTile * _tile;
    };

}	// namespace QDirStat


#endif // ifndef TreemapView_h

