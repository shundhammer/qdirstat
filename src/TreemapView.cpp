/*
 *   File name: TreemapView.cpp
 *   Summary:	View widget for treemap rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QResizeEvent>
#include <QRegExp>

#include "DirTree.h"
#include "SelectionModel.h"
#include "TreemapView.h"
#include "TreemapTile.h"
#include "Exception.h"
#include "Logger.h"
#include "SignalBlocker.h"


using namespace QDirStat;

#define UpdateMinSize	20



TreemapView::TreemapView( QWidget * parent ):
    QGraphicsView( parent ),
    _tree( 0 ),
    _selectionModel( 0 ),
    _rootTile( 0 ),
    _currentItem( 0 ),
    _currentItemRect( 0 )
{
    logDebug() << endl;
    readConfig();

    // Default values for light sources taken from Wiik / Wetering's paper
    // about "cushion treemaps".

    _lightX = 0.09759;
    _lightY = 0.19518;
    _lightZ = 0.9759;

    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy	( Qt::ScrollBarAlwaysOff );
}


TreemapView::~TreemapView()
{
}


void TreemapView::clear()
{
    if ( scene() )
	qDeleteAll( scene()->items() );

    _currentItem     = 0;
    _currentItemRect = 0;
    _rootTile	     = 0;
}


void TreemapView::setDirTree( DirTree * newTree )
{
    logDebug() << endl;
    _tree = newTree;
    CHECK_PTR( _tree );

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

    connect( _tree, SIGNAL( finished()	     ),
	     this,  SLOT  ( rebuildTreemap() ) );
}


void TreemapView::setSelectionModel( SelectionModel * selectionModel )
{
    logDebug() << endl;
    _selectionModel = selectionModel;
    CHECK_PTR( _selectionModel );

    connect( this,	      SIGNAL( selectionChanged( FileInfo * ) ),
	     _selectionModel, SLOT  ( selectItem      ( FileInfo * ) ) );

    connect( this,	      SIGNAL( currentItemChanged( FileInfo * ) ),
	     _selectionModel, SLOT  ( setCurrentItem    ( FileInfo * ) ) );

    connect( _selectionModel, SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,	      SLOT  ( updateCurrentItem ( FileInfo *		 ) ) );

    connect( _selectionModel, SIGNAL( selectionChanged( FileInfoSet ) ),
	     this,	      SLOT  ( updateSelection ( FileInfoSet ) ) );
}


void TreemapView::readConfig()
{
#if 0
    KConfig * config = kapp->config();
    config->setGroup( "Treemaps" );

    _ambientLight	= config->readNumEntry( "AmbientLight"		,  DefaultAmbientLight );

    _heightScaleFactor	= config->readDoubleNumEntry( "HeightScaleFactor" , DefaultHeightScaleFactor );
    _squarify		= config->readBoolEntry( "Squarify"		, true	);
    _doCushionShading	= config->readBoolEntry( "CushionShading"	, true	);
    _ensureContrast	= config->readBoolEntry( "EnsureContrast"	, true	);
    _forceCushionGrid	= config->readBoolEntry( "ForceCushionGrid"	, false );
    _minTileSize	= config->readNumEntry ( "MinTileSize"		, DefaultMinTileSize );

    _currentItemColor	= readColorEntry( config, "CurrentItemColor"	, Qt::red		     );
    _selectedItemsColor = readColorEntry( config, "SelectedItemsColor"	, Qt::yellow		     );
    _cushionGridColor	= readColorEntry( config, "CushionGridColor"	, QColor( 0x80, 0x80, 0x80 ) );
    _outlineColor	= readColorEntry( config, "OutlineColor"	, Qt::black		     );
    _fileFillColor	= readColorEntry( config, "FileFillColor"	, QColor( 0xde, 0x8d, 0x53 ) );
    _dirFillColor	= readColorEntry( config, "DirFillColor"	, QColor( 0x10, 0x7d, 0xb4 ) );
#else

    // FIXME: Read from config
    // FIXME: Read from config
    // FIXME: Read from config

    _ambientLight	= DefaultAmbientLight;

    _heightScaleFactor	= DefaultHeightScaleFactor;
    _squarify		= true;
    _doCushionShading	= true;
    _ensureContrast	= true;
    _forceCushionGrid	= false;
    _minTileSize	= DefaultMinTileSize;

    _currentItemColor	= Qt::red;
    _selectedItemsColor = Qt::yellow;
    _cushionGridColor	= QColor( 0x80, 0x80, 0x80 );
    _outlineColor	= Qt::black;
    _fileFillColor	= QColor( 0xde, 0x8d, 0x53 );
    _dirFillColor	= QColor( 0x10, 0x7d, 0xb4 );

#endif
}


#if 0
QColor TreemapView::readColorEntry( KConfig * config, const char * entryName, QColor defaultColor )
{
    return config->readColorEntry( entryName, &defaultColor );
}
#endif


TreemapTile * TreemapView::tileAt( QPoint pos )
{
    TreemapTile * tile = 0;

#if 1
    Q_UNUSED( pos );
#else
    QCanvasItemList coll = canvas()->collisions( pos );
    QCanvasItemList::Iterator it = coll.begin();

    while ( it != coll.end() && tile == 0 )
    {
	tile = dynamic_cast<TreemapTile *> (*it);
	++it;
    }
#endif

    return tile;
}


#if 0
void TreemapView::contentsMousePressEvent( QMouseEvent * event )
{
    // logDebug() << endl;

    TreemapTile * tile = tileAt( event->pos() );

    if ( ! tile )
	return;

    switch ( event->button() )
    {
	case LeftButton:
	    setCurrentItem( tile );
	    break;

	case MidButton:
	    // Select clicked tile's parent, if available

	    if ( _currentItem &&
		 _currentItem->rect().contains( event->pos() ) )
	    {
		if ( _currentItem->parentTile() )
		    tile = _currentItem->parentTile();
	    }

	    // Intentionally handling the middle button like the left button if
	    // the user clicked outside the (old) selected tile: Simply select
	    // the clicked tile. This makes using this middle mouse button
	    // intuitive: It can be used very much like the left mouse button,
	    // but it has added functionality. Plus, it cycles back to the
	    // clicked tile if the user has already clicked all the way up the
	    // hierarchy (i.e. the topmost directory is highlighted).

	    setCurrentItem( tile );
	    break;

	case RightButton:

	    if ( tile )
	    {
		if ( _currentItem &&
		     _currentItem->rect().contains( event->pos() ) )
		{
		    // If a directory (non-leaf tile) is already selected,
		    // don't override this by

		    emit contextMenu( _currentItem, event->globalPos() );
		}
		else
		{
		    setCurrentItem( tile );
		    emit contextMenu( tile, event->globalPos() );
		}
	    }
	    break;

	default:
	    // event->button() is an enum, so g++ complains
	    // if there are unhandled cases.
	    break;
    }
}
#endif


#if 0
void TreemapView::contentsMouseDoubleClickEvent( QMouseEvent * event )
{
    // logDebug() << endl;

    TreemapTile * tile = tileAt( event->pos() );

    if ( ! tile )
	return;

    switch ( event->button() )
    {
	case LeftButton:
	    if ( tile )
	    {
		setCurrentItem( tile );
		zoomIn();
	    }
	    break;

	case MidButton:
	    zoomOut();
	    break;

	case RightButton:
	    // Double-clicking the right mouse button is pretty useless - the
	    // first click opens the context menu: Single clicks are always
	    // delivered first. Even if that would be caught by using timers,
	    // it would still be very awkward to use: Click too slow, and
	    // you'll get the context menu rather than what you really wanted -
	    // then you'd have to get rid of the context menu first.
	    break;

	default:
	    // Prevent compiler complaints about missing enum values in switch
	    break;
    }
}
#endif


void TreemapView::zoomIn()
{
    if ( ! _currentItem || ! _rootTile )
	return;

    TreemapTile * newRootTile = _currentItem;

    while ( newRootTile->parentTile() != _rootTile &&
	    newRootTile->parentTile() ) // This should never happen, but who knows?
    {
	newRootTile = newRootTile->parentTile();
    }

    if ( newRootTile )
    {
	FileInfo * newRoot = newRootTile->orig();

	if ( newRoot->isDir() || newRoot->isDotEntry() )
	    rebuildTreemap( newRoot );
    }
}


void TreemapView::zoomOut()
{
    if ( _rootTile )
    {
	FileInfo * root = _rootTile->orig();

	if ( root->parent() )
	    root = root->parent();

	rebuildTreemap( root );
    }
}


void TreemapView::selectParent()
{
    if ( _currentItem && _currentItem->parentTile() )
	setCurrentItem( _currentItem->parentTile() );
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

	if ( newRoot->isDir() || newRoot->isDotEntry() )
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


bool TreemapView::canSelectParent() const
{
    return _currentItem && _currentItem->parentTile();
}


void TreemapView::rebuildTreemap()
{
    FileInfo * root = 0;

    if ( ! _savedRootUrl.isEmpty() )
    {
	// logDebug() << "Restoring old treemap with root " << _savedRootUrl << endl;

	root = _tree->locate( _savedRootUrl, true );	// node, findDotEntries
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
    logDebug() << endl;
    QGraphicsView::resizeEvent( event );

    if ( ! _tree )
	return;

    bool tooSmall = event->size().width()  < UpdateMinSize ||
		    event->size().height() < UpdateMinSize;

    if ( tooSmall && _rootTile )
    {
	logDebug() << "Suppressing treemap contents" << endl;
	rebuildTreemap( _rootTile->orig() );
    }
    else if ( ! tooSmall && ! _rootTile )
    {
	if ( _tree && _tree->firstToplevel() )
	{
	    logDebug() << "Redisplaying suppressed treemap contents" << endl;
	    rebuildTreemap( _tree->firstToplevel() );
	}
    }
    else if ( _rootTile )
    {
	logDebug() << "Auto-resizing treemap" << endl;
	rebuildTreemap( _rootTile->orig() );
    }
}


void TreemapView::setCurrentItem( TreemapTile * tile )
{
    logDebug() << tile << endl;

    TreemapTile * oldCurrent = _currentItem;
    _currentItem = tile;

    if ( _currentItem )
    {
	if ( ! _currentItemRect )
	    _currentItemRect = new HighlightRect( scene(), _currentItemColor );
    }

    if ( _currentItemRect )
    {
        if ( _currentItem == _rootTile )
            _currentItemRect->hide(); // Don't highlight the root tile
        else
            _currentItemRect->highlight( _currentItem );
    }

    if ( oldCurrent != _currentItem )
    {
        logDebug() << "Sending currentItemChanged " << _currentItem << endl;
	emit currentItemChanged( _currentItem ? _currentItem->orig() : 0 );
    }
}


void TreemapView::setCurrentItem( FileInfo * node )
{
    logDebug() << node << endl;

    setCurrentItem( findTile( node ) );
}


void TreemapView::updateSelection( const FileInfoSet & newSelection )
{
    if ( ! scene() )
	return;

    SignalBlocker sigBlocker( this );
    scene()->clearSelection();

    foreach ( FileInfo * item, newSelection )
    {
	TreemapTile * tile = findTile( item );

	if ( tile )
	    tile->setSelected( true );
    }
}


void TreemapView::updateCurrentItem( FileInfo * currentItem )
{
    if ( ! scene() )
	return;

    SignalBlocker sigBlocker( this );
    setCurrentItem( currentItem );
}



TreemapTile * TreemapView::findTile( FileInfo * fileInfo )
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
    logDebug() << "Visible size: " << size.width() << " x " << size.height() << endl;

    return size;
}


QColor TreemapView::tileColor( FileInfo * file )
{
    if ( file )
    {
	if ( file->isFile() )
	{
	    // Find the filename extension: Everything after the first '.'
	    QString ext = file->name().section( '.', 1 );

	    while ( ! ext.isEmpty() )
	    {
		QString lowerExt = ext.toLower();

		// Try case sensitive comparisions first

		if ( ext == "~"		)	return Qt::red;
		if ( ext == "bak"	)	return Qt::red;

		if ( ext == "c"		)	return Qt::blue;
		if ( ext == "cpp"	)	return Qt::blue;
		if ( ext == "cc"	)	return Qt::blue;
		if ( ext == "h"		)	return Qt::blue;
		if ( ext == "hpp"	)	return Qt::blue;
		if ( ext == "el"	)	return Qt::blue;

		if ( ext == "o"		)	return QColor( 0xff, 0xa0, 0x00 );
		if ( ext == "lo"	)	return QColor( 0xff, 0xa0, 0x00 );
		if ( ext == "Po"	)	return QColor( 0xff, 0xa0, 0x00 );
		if ( ext == "al"	)	return QColor( 0xff, 0xa0, 0x00 );
		if ( ext == "moc.cpp"	)	return QColor( 0xff, 0xa0, 0x00 );
		if ( ext == "moc.cc"	)	return QColor( 0xff, 0xa0, 0x00 );
		if ( ext == "elc"	)	return QColor( 0xff, 0xa0, 0x00 );
		if ( ext == "la"	)	return QColor( 0xff, 0xa0, 0x00 );
		if ( ext == "a"		)	return QColor( 0xff, 0xa0, 0x00 );
		if ( ext == "rpm"	)	return QColor( 0xff, 0xa0, 0x00 );

		if ( lowerExt == "tar.bz2" )	return Qt::green;
		if ( lowerExt == "tar.gz"  )	return Qt::green;
		if ( lowerExt == "tgz"	)	return Qt::green;
		if ( lowerExt == "bz2"	)	return Qt::green;
		if ( lowerExt == "bz"	)	return Qt::green;
		if ( lowerExt == "gz"	)	return Qt::green;

		if ( lowerExt == "html" )	return Qt::blue;
		if ( lowerExt == "htm"	)	return Qt::blue;
		if ( lowerExt == "txt"	)	return Qt::blue;
		if ( lowerExt == "doc"	)	return Qt::blue;

		if ( lowerExt == "png"	)	return Qt::cyan;
		if ( lowerExt == "jpg"	)	return Qt::cyan;
		if ( lowerExt == "jpeg" )	return Qt::cyan;
		if ( lowerExt == "gif"	)	return Qt::cyan;
		if ( lowerExt == "tif"	)	return Qt::cyan;
		if ( lowerExt == "tiff" )	return Qt::cyan;
		if ( lowerExt == "bmp"	)	return Qt::cyan;
		if ( lowerExt == "xpm"	)	return Qt::cyan;
		if ( lowerExt == "tga"	)	return Qt::cyan;

		if ( lowerExt == "wav"	)	return Qt::yellow;
		if ( lowerExt == "mp3"	)	return Qt::yellow;

		if ( lowerExt == "avi"	)	return QColor( 0xa0, 0xff, 0x00 );
		if ( lowerExt == "mov"	)	return QColor( 0xa0, 0xff, 0x00 );
		if ( lowerExt == "mpg"	)	return QColor( 0xa0, 0xff, 0x00 );
		if ( lowerExt == "mpeg" )	return QColor( 0xa0, 0xff, 0x00 );

		if ( lowerExt == "pdf"	)	return Qt::blue;
		if ( lowerExt == "ps"	)	return Qt::cyan;


		// Some DOS/Windows types

		if ( lowerExt == "exe"	)	return Qt::magenta;
		if ( lowerExt == "com"	)	return Qt::magenta;
		if ( lowerExt == "dll"	)	return QColor( 0xff, 0xa0, 0x00 );
		if ( lowerExt == "zip"	)	return Qt::green;
		if ( lowerExt == "arj"	)	return Qt::green;


		// No match so far? Try the next extension. Some files might have
		// more than one, e.g., "tar.bz2" - if there is no match for
		// "tar.bz2", there might be one for just "bz2".

		ext = ext.section( '.', 1 );
	    }

	    // Shared libs
	    if ( QRegExp( "lib.*\\.so.*" ).exactMatch( file->name() ) )
		return QColor( 0xff, 0xa0, 0x00 );

	    // Very special, but common: Core dumps
	    if ( file->name() == "core" )	return Qt::red;

	    // Special case: Executables
	    if ( ( file->mode() & S_IXUSR  ) == S_IXUSR )	return Qt::magenta;
	}
	else // Directories
	{
	    // TO DO
	    return Qt::blue;
	}
    }

    return Qt::white;
}






HighlightRect::HighlightRect( QGraphicsScene * scene, const QColor & color, int lineWidth ):
    QGraphicsRectItem()
{
    setPen( QPen( color, lineWidth ) );
    setZValue( 1e10 );		// Higher than everything else
    hide();
    scene->addItem( this );
}


HighlightRect::HighlightRect( TreemapTile * tile, const QColor & color, int lineWidth ):
    QGraphicsRectItem()
{
    CHECK_PTR( tile );

    setPen( QPen( color, lineWidth ) );
    setZValue( 1e8 );	       // Not quite as high as the scene-wide highlight rect
    tile->scene()->addItem( this );
    highlight( tile );
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

