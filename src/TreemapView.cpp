/*
 *   File name: TreemapView.cpp
 *   Summary:	View widget for treemap rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QResizeEvent>
#include <QRegExp>
#include <QTimer>

#include "TreemapView.h"
#include "DirTree.h"
#include "DirInfo.h"
#include "SelectionModel.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "SignalBlocker.h"
#include "TreemapTile.h"
#include "MimeCategorizer.h"
#include "DelayedRebuilder.h"
#include "Exception.h"
#include "Logger.h"

#define UpdateMinSize	      20

using namespace QDirStat;


// Treemap layers (Z values)

const double TileLayer           = 0.0;
const double SceneMaskLayer      = 1e5;
const double TileHighlightLayer  = 1e6;
const double SceneHighlightLayer = 1e10;


TreemapView::TreemapView( QWidget * parent ):
    QGraphicsView( parent ),
    _tree(0),
    _selectionModel(0),
    _selectionModelProxy(0),
    _cleanupCollection(0),
    _rebuilder(0),
    _rootTile(0),
    _currentItem(0),
    _currentItemRect(0),
    _sceneMask(0),
    _newRoot(0),
    _highlightedTile(0),
    _useFixedColor(false),
    _useDirGradient(true)
{
    // logDebug() << endl;

    readSettings();

    // Default values for light sources taken from Wiik / Wetering's paper
    // about "cushion treemaps".

    _lightX = 0.09759;
    _lightY = 0.19518;
    _lightZ = 0.9759;

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy	( Qt::ScrollBarAlwaysOff );

    _rebuilder = new DelayedRebuilder( this );
    CHECK_NEW( _rebuilder );

    connect( _rebuilder, SIGNAL( rebuild() ),
	     this,	 SLOT  ( rebuildTreemapDelayed() ) );
}


TreemapView::~TreemapView()
{
    // Write settings back to file so the user can change them in that file:
    // There is no settings dialog for this class because the settings are all
    // pretty obscure - strictly for experts.
    writeSettings();
}


void TreemapView::clear()
{
    if ( scene() )
	qDeleteAll( scene()->items() );

    _currentItem     = 0;
    _currentItemRect = 0;
    _rootTile	     = 0;
    _sceneMask       = 0;
    _parentHighlightList.clear();
}


void TreemapView::setDirTree( DirTree * newTree )
{
    // logDebug() << endl;
    _tree = newTree;

    if ( ! _tree )
	return;

    if ( _tree->firstToplevel() )
    {
	if ( ! _rootTile )
	{
	    // The treemap might already be created indirectly by
	    // rebuildTreemap() called from resizeEvent() triggered by resize()
	    // above. If this is so, don't do it again.

	    rebuildTreemap( _tree->firstToplevel() );
	}
    }

    connect( _tree, SIGNAL( deletingChild   ( FileInfo * )  ),
	     this,  SLOT  ( deleteNotify    ( FileInfo * ) ) );

    connect( _tree, SIGNAL( childDeleted()   ),
	     this,  SLOT  ( rebuildTreemap() ) );

    connect( _tree, SIGNAL( clearing() ),
	     this,  SLOT  ( clear()    ) );

    connect( _tree, SIGNAL( finished()	     ),
	     this,  SLOT  ( rebuildTreemap() ) );
}


void TreemapView::setSelectionModel( SelectionModel * selectionModel )
{
    // logDebug() << endl;
    _selectionModel = selectionModel;
    CHECK_PTR( _selectionModel );

    if ( _selectionModelProxy )
	delete _selectionModelProxy;

    _selectionModelProxy = new SelectionModelProxy( selectionModel, this );
    CHECK_PTR( _selectionModelProxy );

    connect( this,	      SIGNAL( selectionChanged( FileInfo * ) ),
	     _selectionModel, SLOT  ( selectItem      ( FileInfo * ) ) );

    connect( this,	      SIGNAL( currentItemChanged( FileInfo * ) ),
	     _selectionModel, SLOT  ( setCurrentItem	( FileInfo * ) ) );

    connect( this,	      SIGNAL( currentItemChanged( FileInfo * ) ),
	     _selectionModel, SLOT  ( setCurrentBranch	( FileInfo * ) ) );


    // Use the proxy for all receiving signals!

    connect( _selectionModelProxy, SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,		   SLOT	 ( updateCurrentItem ( FileInfo *		 ) ) );

    connect( _selectionModelProxy, SIGNAL( selectionChanged( FileInfoSet ) ),
	     this,		   SLOT	 ( updateSelection ( FileInfoSet ) ) );
}


void TreemapView::readSettings()
{
    Settings settings;
    settings.beginGroup( "Treemaps" );

    _ambientLight	= settings.value( "AmbientLight"     , DefaultAmbientLight ).toInt();
    _heightScaleFactor	= settings.value( "HeightScaleFactor", DefaultHeightScaleFactor ).toDouble();
    _squarify		= settings.value( "Squarify"	     , true  ).toBool();
    _doCushionShading	= settings.value( "CushionShading"   , true  ).toBool();
    _enforceContrast	= settings.value( "EnforceContrast"  , false ).toBool();
    _forceCushionGrid	= settings.value( "ForceCushionGrid" , false ).toBool();
    _useDirGradient	= settings.value( "UseDirGradient"   , true  ).toBool();
    _minTileSize	= settings.value( "MinTileSize"	     , DefaultMinTileSize ).toInt();

    _currentItemColor	= readColorEntry( settings, "CurrentItemColor"	, Qt::red		     );
    _selectedItemsColor = readColorEntry( settings, "SelectedItemsColor", Qt::yellow		     );
    _cushionGridColor	= readColorEntry( settings, "CushionGridColor"	, QColor( 0x80, 0x80, 0x80 ) );
    _outlineColor	= readColorEntry( settings, "OutlineColor"	, Qt::black		     );
    _fileFillColor	= readColorEntry( settings, "FileFillColor"	, QColor( 0xde, 0x8d, 0x53 ) );
    _dirFillColor	= readColorEntry( settings, "DirFillColor"	, QColor( 0x10, 0x7d, 0xb4 ) );
    _dirGradientStart	= readColorEntry( settings, "DirGradientStart"	, QColor( 0x60, 0x60, 0x70 ) );
    _dirGradientEnd	= readColorEntry( settings, "DirGradientEnd"	, QColor( 0x70, 0x70, 0x80 ) );

    settings.endGroup();
}


void TreemapView::writeSettings()
{
    // logDebug() << endl;

    Settings settings;
    settings.beginGroup( "Treemaps" );

    settings.setValue( "AmbientLight"	   , _ambientLight	 );
    settings.setValue( "HeightScaleFactor" , _heightScaleFactor	 );
    settings.setValue( "Squarify"	   , _squarify		 );
    settings.setValue( "CushionShading"	   , _doCushionShading	 );
    settings.setValue( "EnforceContrast"   , _enforceContrast	 );
    settings.setValue( "ForceCushionGrid"  , _forceCushionGrid	 );
    settings.setValue( "UseDirGradient"	   , _useDirGradient	 );
    settings.setValue( "MinTileSize"	   , _minTileSize	 );

    writeColorEntry( settings, "CurrentItemColor"  , _currentItemColor	 );
    writeColorEntry( settings, "SelectedItemsColor", _selectedItemsColor );
    writeColorEntry( settings, "CushionGridColor"  , _cushionGridColor	 );
    writeColorEntry( settings, "OutlineColor"	   , _outlineColor	 );
    writeColorEntry( settings, "FileFillColor"	   , _fileFillColor	 );
    writeColorEntry( settings, "DirFillColor"	   , _dirFillColor	 );
    writeColorEntry( settings, "DirGradientStart"  , _dirGradientStart	 );
    writeColorEntry( settings, "DirGradientEnd"	   , _dirGradientEnd	 );

    settings.endGroup();
}


void TreemapView::zoomIn()
{
    if ( ! canZoomIn() )
	return;

    TreemapTile * newRootTile = _currentItem;

    while ( newRootTile &&
	    newRootTile->parentTile() != _rootTile &&
	    newRootTile->parentTile() ) // This should never happen, but who knows?
    {
	newRootTile = newRootTile->parentTile();
    }

    if ( newRootTile )
    {
	FileInfo * newRoot = newRootTile->orig();

	if ( newRoot->isDirInfo() )
	    rebuildTreemap( newRoot );
    }
}


void TreemapView::zoomOut()
{
    if ( ! canZoomOut() )
	return;

    FileInfo * newRoot = _rootTile->orig();

    if ( newRoot->parent() && newRoot->parent() != _tree->root() )
	newRoot = newRoot->parent();

    rebuildTreemap( newRoot );
}


void TreemapView::resetZoom()
{
    if ( _tree && _tree->firstToplevel() )
	rebuildTreemap( _tree->firstToplevel() );
}


bool TreemapView::canZoomIn() const
{
    if ( ! _currentItem || ! _rootTile )
	return false;

    if ( _currentItem == _rootTile )
	return false;

    TreemapTile * newRootTile = _currentItem;

    while ( newRootTile->parentTile() != _rootTile &&
	    newRootTile->parentTile() ) // This should never happen, but who knows?
    {
	newRootTile = newRootTile->parentTile();
    }

    if ( newRootTile )
    {
	FileInfo * newRoot = newRootTile->orig();

	if ( newRoot->isDirInfo() )
	    return true;
    }

    return false;
}


bool TreemapView::canZoomOut() const
{
    if ( ! _rootTile || ! _tree->firstToplevel() )
	return false;

    return _rootTile->orig() != _tree->firstToplevel();
}


void TreemapView::rebuildTreemap()
{
    FileInfo * root = 0;

    if ( ! _savedRootUrl.isEmpty() )
    {
	// logDebug() << "Restoring old treemap with root " << _savedRootUrl << endl;

	root = _tree->locate( _savedRootUrl, true );	// node, findPseudoDirs
    }

    if ( ! root )
	root = _rootTile ? _rootTile->orig() : _tree->firstToplevel();

    rebuildTreemap( root, sceneRect().size() );
    _savedRootUrl = "";
}


void TreemapView::rebuildTreemap( FileInfo *	 newRoot,
				  const QSizeF & newSz )
{
    // logDebug() << endl;

    QSizeF newSize = newSz;

    if ( newSz.isEmpty() )
	newSize = visibleSize();

    // Delete all old stuff.
    clear();

    if ( ! scene() )
    {
	QGraphicsScene * scene = new QGraphicsScene( this );
	CHECK_NEW( scene);
	setScene( scene );
    }

    QRectF rect = QRectF( 0.0, 0.0, (double) newSize.width(), (double) newSize.height() );
    scene()->setSceneRect( rect );

    if ( newSize.width() >= UpdateMinSize && newSize.height() >= UpdateMinSize )
    {
	// The treemap contents is displayed if larger than a certain minimum
	// visible size. This is an easy way for the user to avoid
	// time-consuming delays when deleting a lot of files: Simply make the
	// treemap (sub-) window very small.

	// Fill the new scene

	if ( newRoot )
	{
	    _rootTile = new TreemapTile( this,		// parentView
					 0,		// parentTile
					 newRoot,	// orig
					 rect,
					 TreemapAuto );
	}


	// Synchronize selection with other views

	if ( _selectionModel )
	{
	    updateSelection( _selectionModel->selectedItems() );
	    updateCurrentItem( _selectionModel->currentItem() );
	}
    }
    else
    {
	// logDebug() << "Too small - suppressing treemap contents" << endl;
    }

    emit treemapChanged();
}


void TreemapView::scheduleRebuildTreemap( FileInfo * newRoot )
{
    _newRoot = newRoot;
    _rebuilder->scheduleRebuild();
}


void TreemapView::rebuildTreemapDelayed()
{
    rebuildTreemap( _newRoot );
}


void TreemapView::deleteNotify( FileInfo * )
{
    if ( _rootTile )
    {
	if ( _rootTile->orig() != _tree->firstToplevel() )
	{
	    // If the user zoomed the treemap in, save the root's URL so the
	    // current state can be restored upon the next rebuildTreemap()
	    // call (which is triggered by the childDeleted() signal that the
	    // tree emits after deleting is done).
	    //
	    // Intentionally using debugUrl() here rather than just url() so
	    // the correct zoom can be restored even when a dot entry is the
	    // current treemap root.

	    _savedRootUrl = _rootTile->orig()->debugUrl();
	}
	else
	{
	    // A shortcut for the most common case: No zoom. Simply use the
	    // tree's root for the next treemap rebuild.

	    _savedRootUrl = "";
	}
    }
    else
    {
	// Intentionally leaving _savedRootUrl alone: Otherwise multiple
	// deleteNotify() calls might cause a previously saved _savedRootUrl to
	// be unnecessarily deleted, thus the treemap couldn't be restored as
	// it was.
    }

    clear();
}


void TreemapView::resizeEvent( QResizeEvent * event )
{
    // logDebug() << endl;
    QGraphicsView::resizeEvent( event );

    if ( ! _tree )
	return;

    bool tooSmall = event->size().width()  < UpdateMinSize ||
		    event->size().height() < UpdateMinSize;

    if ( tooSmall && _rootTile )
    {
	// logDebug() << "Suppressing treemap contents" << endl;
	scheduleRebuildTreemap( _rootTile->orig() );
    }
    else if ( ! tooSmall && ! _rootTile )
    {
	if ( _tree && _tree->firstToplevel() )
	{
	    // logDebug() << "Redisplaying suppressed treemap contents" << endl;
	    scheduleRebuildTreemap( _tree->firstToplevel() );
	}
    }
    else if ( _rootTile )
    {
	// logDebug() << "Auto-resizing treemap" << endl;
	scheduleRebuildTreemap( _rootTile->orig() );
    }
}


void TreemapView::disable()
{
    // logDebug() << "Disabling treemap view" << endl;

    clear();
    resize( width(), 1 );
    hide();

    emit treemapChanged();
}


void TreemapView::enable()
{
    if ( ! isVisible() )
    {
	// logDebug() << "Enabling treemap view" << endl;
	show();
	QWidget * parentWidget = qobject_cast<QWidget *>( parent() );

	if ( parentWidget )
	    resize( parentWidget->height(), width() );

	scheduleRebuildTreemap( _tree->firstToplevel() );
    }
}


void TreemapView::setCurrentItem( TreemapTile * tile )
{
    // logDebug() << tile << endl;

    TreemapTile * oldCurrent = _currentItem;
    _currentItem = tile;

    if ( _currentItem )
    {
        if ( highlightedParent() != _currentItem->parentTile() )
            clearParentsHighlight();

	if ( ! _currentItemRect )
	    _currentItemRect = new CurrentItemHighlighter( scene(), _currentItemColor );
    }

    if ( _currentItemRect )
    {
	if ( _currentItem == _rootTile )
	    _currentItemRect->hide(); // Don't highlight the root tile
	else
	    _currentItemRect->highlight( _currentItem );
    }

    if ( oldCurrent != _currentItem && _selectionModelProxy )
    {
	// logDebug() << "Sending currentItemChanged " << _currentItem << endl;

	SignalBlocker sigBlocker( _selectionModelProxy ); // Prevent signal ping-pong
	emit currentItemChanged( _currentItem ? _currentItem->orig() : 0 );
    }
}


void TreemapView::setCurrentItem( FileInfo * node )
{
    // logDebug() << node << endl;

    if ( node && _rootTile )
    {
	FileInfo * treemapRoot = _rootTile->orig();

	// Check if the new current item is inside the current treemap
	// (it might be zoomed).

	while ( ! node->isInSubtree( treemapRoot ) &&
		treemapRoot->parent() &&
		treemapRoot->parent() != _tree->root() )
	{
	    treemapRoot = treemapRoot->parent(); // try one level higher
	}

	if ( treemapRoot != _rootTile->orig() )	  // need to zoom out?
	{
	    logDebug() << "Zooming out to " << treemapRoot << " to make current item visible" << endl;
	    rebuildTreemap( treemapRoot );
	}
    }

    setCurrentItem( findTile( node ) );
}


void TreemapView::updateSelection( const FileInfoSet & newSelection )
{
    if ( ! scene() )
	return;

    // logDebug() << newSelection.size() << " items selected" << endl;
    SignalBlocker sigBlocker( this );
    scene()->clearSelection();

    foreach ( const FileInfo * item, newSelection )
    {
	// logDebug() << "	 Selected: " << item << endl;
	TreemapTile * tile = findTile( item );

	if ( tile )
	    tile->setSelected( true );
    }

    updateCurrentItem( _currentItem ? _currentItem->orig() : 0 );
}


void TreemapView::sendSelection()
{
    if ( ! scene() || ! _selectionModel )
	return;

    SignalBlocker sigBlocker( _selectionModelProxy );
    QList<QGraphicsItem *> selectedTiles = scene()->selectedItems();

    if ( selectedTiles.size() == 1 && selectedTiles.first() == _currentItem )
    {
	// This is the most common case: One tile is selected.
	// Reduce number of signals in that case.

	_selectionModel->setCurrentItem( _currentItem->orig(),
					 true ); // select
    }
    else // Multi-selection
    {
	FileInfoSet selectedItems;

	foreach ( QGraphicsItem * item, selectedTiles )
	{
	    TreemapTile * tile = dynamic_cast<TreemapTile *>( item );

	    if ( tile )
		selectedItems << tile->orig();
	}

	_selectionModel->setSelectedItems( selectedItems );
	_selectionModel->setCurrentItem( _currentItem ? _currentItem->orig() : 0 );
    }
}


void TreemapView::updateCurrentItem( FileInfo * currentItem )
{
    if ( ! scene() )
	return;

    SignalBlocker sigBlocker( this );
    setCurrentItem( currentItem );
}



TreemapTile * TreemapView::findTile( const FileInfo * fileInfo )
{
    if ( ! fileInfo || ! scene() )
	return 0;

    foreach ( QGraphicsItem * graphicsItem, scene()->items() )
    {
	TreemapTile * tile = dynamic_cast<TreemapTile *>(graphicsItem);

	if ( tile && tile->orig() == fileInfo )
	    return tile;
    }

    return 0;
}


QSize TreemapView::visibleSize()
{
    QSize size = viewport()->size();
    // logDebug() << "Visible size: " << size.width() << " x " << size.height() << endl;

    return size;
}


void TreemapView::setFixedColor( const QColor & color )
{
    _fixedColor	   = color;
    _useFixedColor = _fixedColor.isValid();
}


QColor TreemapView::tileColor( FileInfo * file )
{
    if ( _useFixedColor )
	return _fixedColor;

    if ( file )
    {
	if ( file->isFile() )
	{
	    MimeCategory * category = MimeCategorizer::instance()->category( file );

	    if ( category )
		return category->color();
	    else
	    {
		// Special case: Executables
		if ( ( file->mode() & S_IXUSR  ) == S_IXUSR )
		    return Qt::magenta;		// TO DO: Configurable
	    }
	}
	else // Directories
	{
	    // TO DO
	    return Qt::blue;
	}
    }

    return Qt::white;
}


void TreemapView::highlightParents( TreemapTile * tile )
{
    if ( ! tile )
    {
        clearParentsHighlight();
        return;
    }

    _highlightedTile = tile;
    TreemapTile * parent = tile->parentTile();
    TreemapTile * currentHighlight = highlightedParent();

    if ( currentHighlight && currentHighlight != parent )
        clearParentsHighlight();

    TreemapTile * topParent = 0;
    int lineWidth = 2;  // For the first (the direct) parent

    while ( parent && parent != _rootTile )
    {
        HighlightRect * highlight = new HighlightRect( parent, Qt::white, lineWidth );
        CHECK_NEW( highlight );
        _parentHighlightList << highlight;
        highlight->setToolTip( parent->orig()->debugUrl() );

        topParent = parent;
        parent = parent->parentTile();
        lineWidth = 1;  // For all higher-level parents
    }

    if ( topParent )
    {
        clearSceneMask();
        _sceneMask = new SceneMask( topParent, 0.6 );
    }
}


void TreemapView::clearParentsHighlight()
{
    qDeleteAll( _parentHighlightList );
    _parentHighlightList.clear();
    _highlightedTile = 0;
    clearSceneMask();
}


void TreemapView::toggleParentsHighlight( TreemapTile * tile )
{
    if ( tile == _highlightedTile )
        clearParentsHighlight();
    else
        highlightParents( tile );
}


void TreemapView::clearSceneMask()
{
    if ( _sceneMask )
        delete _sceneMask;

    _sceneMask = 0;
}


TreemapTile * TreemapView::highlightedParent() const
{
    TreemapTile * tile = 0;

    if ( ! _parentHighlightList.empty() )
        tile = _parentHighlightList.first()->tile();

    return tile;
}


void TreemapView::sendHoverEnter( FileInfo * node )
{
    emit hoverEnter( node );
}


void TreemapView::sendHoverLeave( FileInfo * node )
{
    emit hoverLeave( node );
}






HighlightRect::HighlightRect( QGraphicsScene * scene, const QColor & color, int lineWidth ):
    QGraphicsRectItem(),
    _tile(0)
{
    QPen pen( color, lineWidth );
    pen.setStyle( Qt::DotLine );
    setPen( QPen( color, lineWidth ) );
    setPen( pen );
    setZValue( SceneHighlightLayer );
    hide();
    scene->addItem( this );
}


HighlightRect::HighlightRect( TreemapTile * tile, const QColor & color, int lineWidth ):
    QGraphicsRectItem(),
    _tile( tile )
{
    CHECK_PTR( tile );

    setPen( QPen( color, lineWidth ) );
    setZValue( TileHighlightLayer + tile->zValue() );
    tile->scene()->addItem( this );
    highlight( tile );
}


QPainterPath HighlightRect::shape() const
{
    if ( ! _tile )
        return QGraphicsRectItem::shape();

    // Return just the outline as the shape so any tooltip is only displayed on
    // the outline, not inside as well; but use more than the line thickness of
    // 1 or 2 pixels to make it humanly possible to position the mouse cursor
    // close enough.
    //
    // Notice that it's still only on the inside of the line to avoid bad side
    // effects with QGraphicsView's internal mechanisms.

    const int thickness = 10;

    QPainterPath path;
    path.addRect( _tile->rect() );
    path.addRect( _tile->rect().adjusted( thickness,   thickness,
                                          -thickness, -thickness ) );
    return path;
}


void HighlightRect::highlight( TreemapTile * tile )
{
    if ( tile )
    {
	QRectF tileRect = tile->rect();
	tileRect.moveTo( mapFromScene( tile->mapToScene( tileRect.topLeft() ) ) );
	setRect( tileRect );

	if ( ! isVisible() )
	    show();
    }
    else
    {
	if ( isVisible() )
	    hide();
    }
}


void HighlightRect::setPenStyle( Qt::PenStyle style )
{
    QPen highlightPen = pen();
    highlightPen.setStyle( style );
    setPen( highlightPen );
}


void HighlightRect::setPenStyle( TreemapTile * tile )
{
    if ( tile && tile->isSelected() )
	setPenStyle( Qt::SolidLine );
    else
	setPenStyle( Qt::DotLine );
}



void CurrentItemHighlighter::highlight( TreemapTile * tile )
{
    HighlightRect::highlight( tile );
    setPenStyle( tile );
}




SceneMask::SceneMask( TreemapTile * tile, float opacity ):
    QGraphicsPathItem(),
    _tile( tile )
{
    // logDebug() << "Adding scene mask for " << tile->orig() << endl;
    CHECK_PTR( tile );

    QPainterPath path;
    path.addRect( tile->scene()->sceneRect() );

    // Since the default OddEvenFillRule leaves overlapping areas unfilled,
    // adding the tile's rect that is inside the scene rect leaves the tile
    // "cut out", i.e. unobscured.

    path.addRect( tile->rect() );
    setPath( path );

    const int grey = 0x30;
    QColor color( grey, grey, grey, opacity * 255 );
    setBrush( color );

    setZValue( SceneMaskLayer + tile->zValue() );
    tile->scene()->addItem( this );
}
