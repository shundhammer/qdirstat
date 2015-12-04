/*
 *   File name: ktreemapview.cpp
 *   Summary:	High level classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <qevent.h>
#include <qregexp.h>

#include <kapp.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>

#include "kdirtree.h"
#include "ktreemapview.h"
#include "ktreemaptile.h"


using namespace KDirStat;

#define UpdateMinSize	20



KTreemapView::KTreemapView( KDirTree * tree, QWidget * parent, const QSize & initialSize )
    : QCanvasView( parent )
    , _tree( tree )
    , _rootTile( 0 )
    , _selectedTile( 0 )
    , _selectionRect( 0 )
{
    // kdDebug() << k_funcinfo << endl;

    readConfig();

    // Default values for light sources taken from Wiik / Wetering's paper
    // about "cushion treemaps".

    _lightX		= 0.09759;
    _lightY		= 0.19518;
    _lightZ		= 0.9759;

    if ( _autoResize )
    {
	setHScrollBarMode( AlwaysOff );
	setVScrollBarMode( AlwaysOff );
    }

    if ( initialSize.isValid() )
	resize( initialSize );

    if ( tree && tree->root() )
    {
	if ( ! _rootTile )
	{
	    // The treemap might already be created indirectly by
	    // rebuildTreemap() called from resizeEvent() triggered by resize()
	    // above. If this is so, don't do it again.

	    rebuildTreemap( tree->root() );
	}
    }

    connect( this,	SIGNAL( selectionChanged( KFileInfo * ) ),
	     tree,	SLOT  ( selectItem	( KFileInfo * ) ) );

    connect( tree,	SIGNAL( selectionChanged( KFileInfo * ) ),
	     this,	SLOT  ( selectTile	( KFileInfo * ) ) );

    connect( tree,	SIGNAL( deletingChild	( KFileInfo * )	),
	     this,	SLOT  ( deleteNotify	( KFileInfo * ) ) );

    connect( tree,	SIGNAL( childDeleted()	 ),
	     this,	SLOT  ( rebuildTreemap() ) );
}


KTreemapView::~KTreemapView()
{
}


void
KTreemapView::clear()
{
    if ( canvas() )
	deleteAllItems( canvas() );

    _selectedTile	= 0;
    _selectionRect	= 0;
    _rootTile		= 0;
}


void
KTreemapView::deleteAllItems( QCanvas * canvas )
{
    if ( ! canvas )
	return;

    QCanvasItemList all = canvas->allItems();

    for ( QCanvasItemList::Iterator it = all.begin(); it != all.end(); ++it )
	delete *it;
}


void
KTreemapView::readConfig()
{
    KConfig * config = kapp->config();
    config->setGroup( "Treemaps" );

    _ambientLight	= config->readNumEntry( "AmbientLight"		,  DefaultAmbientLight );

    _heightScaleFactor	= config->readDoubleNumEntry( "HeightScaleFactor" , DefaultHeightScaleFactor );
    _autoResize		= config->readBoolEntry( "AutoResize"		, true	);
    _squarify		= config->readBoolEntry( "Squarify"		, true	);
    _doCushionShading	= config->readBoolEntry( "CushionShading"	, true	);
    _ensureContrast	= config->readBoolEntry( "EnsureContrast"	, true	);
    _forceCushionGrid	= config->readBoolEntry( "ForceCushionGrid"	, false	);
    _minTileSize	= config->readNumEntry ( "MinTileSize"		, DefaultMinTileSize );

    _highlightColor	= readColorEntry( config, "HighlightColor"	, red			     );
    _cushionGridColor	= readColorEntry( config, "CushionGridColor"	, QColor( 0x80, 0x80, 0x80 ) );
    _outlineColor	= readColorEntry( config, "OutlineColor"	, black			     );
    _fileFillColor	= readColorEntry( config, "FileFillColor"	, QColor( 0xde, 0x8d, 0x53 ) );
    _dirFillColor	= readColorEntry( config, "DirFillColor"	, QColor( 0x10, 0x7d, 0xb4 ) );

    if ( _autoResize )
    {
	setHScrollBarMode( AlwaysOff );
	setVScrollBarMode( AlwaysOff );
    }
    else
    {
	setHScrollBarMode( QScrollView::Auto );
	setVScrollBarMode( QScrollView::Auto );
    }
}


QColor
KTreemapView::readColorEntry( KConfig * config, const char * entryName, QColor defaultColor )
{
    return config->readColorEntry( entryName, &defaultColor );
}


KTreemapTile *
KTreemapView::tileAt( QPoint pos )
{
    KTreemapTile * tile = 0;

    QCanvasItemList coll = canvas()->collisions( pos );
    QCanvasItemList::Iterator it = coll.begin();

    while ( it != coll.end() && tile == 0 )
    {
	tile = dynamic_cast<KTreemapTile *> (*it);
	++it;
    }

    return tile;
}


void
KTreemapView::contentsMousePressEvent( QMouseEvent * event )
{
    // kdDebug() << k_funcinfo << endl;

    KTreemapTile * tile = tileAt( event->pos() );

    if ( ! tile )
	return;

    switch ( event->button() )
    {
	case LeftButton:
	    selectTile( tile );
	    emit userActivity( 1 );
	    break;

	case MidButton:
	    // Select clicked tile's parent, if available

	    if ( _selectedTile &&
		 _selectedTile->rect().contains( event->pos() ) )
	    {
		if ( _selectedTile->parentTile() )
		    tile = _selectedTile->parentTile();
	    }

	    // Intentionally handling the middle button like the left button if
	    // the user clicked outside the (old) selected tile: Simply select
	    // the clicked tile. This makes using this middle mouse button
	    // intuitive: It can be used very much like the left mouse button,
	    // but it has added functionality. Plus, it cycles back to the
	    // clicked tile if the user has already clicked all the way up the
	    // hierarchy (i.e. the topmost directory is highlighted).

	    selectTile( tile );
	    emit userActivity( 1 );
	    break;

	case RightButton:

	    if ( tile )
	    {
		if ( _selectedTile &&
		     _selectedTile->rect().contains( event->pos() ) )
		{
		    // If a directory (non-leaf tile) is already selected,
		    // don't override this by

		    emit contextMenu( _selectedTile, event->globalPos() );
		}
		else
		{
		    selectTile( tile );
		    emit contextMenu( tile, event->globalPos() );
		}

		emit userActivity( 3 );
	    }
	    break;

	default:
	    // event->button() is an enum, so g++ complains
	    // if there are unhandled cases.
	    break;
    }
}


void
KTreemapView::contentsMouseDoubleClickEvent( QMouseEvent * event )
{
    // kdDebug() << k_funcinfo << endl;

    KTreemapTile * tile = tileAt( event->pos() );

    if ( ! tile )
	return;

    switch ( event->button() )
    {
	case LeftButton:
	    if ( tile )
	    {
		selectTile( tile );
		zoomIn();
		emit userActivity( 5 );
	    }
	    break;

	case MidButton:
	    zoomOut();
	    emit userActivity( 5 );
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


void
KTreemapView::zoomIn()
{
    if ( ! _selectedTile || ! _rootTile )
	return;

    KTreemapTile * newRootTile = _selectedTile;

    while ( newRootTile->parentTile() != _rootTile &&
	    newRootTile->parentTile() ) // This should never happen, but who knows?
    {
	newRootTile = newRootTile->parentTile();
    }

    if ( newRootTile )
    {
	KFileInfo * newRoot = newRootTile->orig();

	if ( newRoot->isDir() || newRoot->isDotEntry() )
	    rebuildTreemap( newRoot );
    }
}


void
KTreemapView::zoomOut()
{
    if ( _rootTile )
    {
	KFileInfo * root = _rootTile->orig();

	if ( root->parent() )
	    root = root->parent();

	rebuildTreemap( root );
    }
}


void
KTreemapView::selectParent()
{
    if ( _selectedTile && _selectedTile->parentTile() )
	selectTile( _selectedTile->parentTile() );
}


bool
KTreemapView::canZoomIn() const
{
    if ( ! _selectedTile || ! _rootTile )
	return false;

    if ( _selectedTile == _rootTile )
	return false;

    KTreemapTile * newRootTile = _selectedTile;

    while ( newRootTile->parentTile() != _rootTile &&
	    newRootTile->parentTile() ) // This should never happen, but who knows?
    {
	newRootTile = newRootTile->parentTile();
    }

    if ( newRootTile )
    {
	KFileInfo * newRoot = newRootTile->orig();

	if ( newRoot->isDir() || newRoot->isDotEntry() )
	    return true;
    }

    return false;
}


bool
KTreemapView::canZoomOut() const
{
    if ( ! _rootTile || ! _tree->root() )
	return false;

    return _rootTile->orig() != _tree->root();
}


bool
KTreemapView::canSelectParent() const
{
    return _selectedTile && _selectedTile->parentTile();
}


void
KTreemapView::rebuildTreemap()
{
    KFileInfo * root = 0;

    if ( ! _savedRootUrl.isEmpty() )
    {
	// kdDebug() << "Restoring old treemap with root " << _savedRootUrl << endl;

	root = _tree->locate( _savedRootUrl, true );	// node, findDotEntries
    }

    if ( ! root )
	root = _rootTile ? _rootTile->orig() : _tree->root();

    rebuildTreemap( root, canvas()->size() );
    _savedRootUrl = "";
}


void
KTreemapView::rebuildTreemap( KFileInfo *	newRoot,
			      const QSize &	newSz )
{
    // kdDebug() << k_funcinfo << endl;

    QSize newSize = newSz;

    if ( newSz.isEmpty() )
	newSize = visibleSize();


    // Delete all old stuff.
    clear();

    // Re-create a new canvas

    if ( ! canvas() )
    {
	QCanvas * canv = new QCanvas( this );
	CHECK_PTR( canv );
	setCanvas( canv );
    }

    canvas()->resize( newSize.width(), newSize.height() );

    if ( newSize.width() >= UpdateMinSize && newSize.height() >= UpdateMinSize )
    {
	// The treemap contents is displayed if larger than a certain minimum
	// visible size. This is an easy way for the user to avoid
	// time-consuming delays when deleting a lot of files: Simply make the
	// treemap (sub-) window very small.

	// Fill the new canvas

	if ( newRoot )
	{
	    _rootTile = new KTreemapTile( this,		// parentView
					  0,		// parentTile
					  newRoot,	// orig
					  QRect( QPoint( 0, 0), newSize ),
					  KTreemapAuto );
	}


	// Synchronize selection with the tree

	if ( _tree->selection() )
	    selectTile( _tree->selection() );
    }
    else
    {
	// kdDebug() << "Too small - suppressing treemap contents" << endl;
    }

    emit treemapChanged();
}


void
KTreemapView::deleteNotify( KFileInfo * )
{
    if ( _rootTile )
    {
	if ( _rootTile->orig() != _tree->root() )
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


void
KTreemapView::resizeEvent( QResizeEvent * event )
{
    QCanvasView::resizeEvent( event );

    if ( _autoResize )
    {
	bool tooSmall =
	    event->size().width()  < UpdateMinSize ||
	    event->size().height() < UpdateMinSize;

	if ( tooSmall && _rootTile )
	{
	    // kdDebug() << "Suppressing treemap contents" << endl;
	    rebuildTreemap( _rootTile->orig() );
	}
	else if ( ! tooSmall && ! _rootTile )
	{
	    if ( _tree->root() )
	    {
		// kdDebug() << "Redisplaying suppressed treemap contents" << endl;
		rebuildTreemap( _tree->root() );
	    }
	}
	else if ( _rootTile )
	{
	    // kdDebug() << "Auto-resizing treemap" << endl;
	    rebuildTreemap( _rootTile->orig() );
	}
    }
}


void
KTreemapView::selectTile( KTreemapTile * tile )
{
    // kdDebug() << k_funcinfo << endl;

    KTreemapTile * oldSelection = _selectedTile;
    _selectedTile = tile;


    // Handle selection (highlight) rectangle

    if ( _selectedTile )
    {
	if ( ! _selectionRect )
	    _selectionRect = new KTreemapSelectionRect( canvas(), _highlightColor );
    }

    if ( _selectionRect )
	_selectionRect->highlight( _selectedTile );

    canvas()->update();

    if ( oldSelection != _selectedTile )
    {
	emit selectionChanged( _selectedTile ? _selectedTile->orig() : 0 );
    }
}


void
KTreemapView::selectTile( KFileInfo * node )
{
    selectTile( findTile( node ) );
}



KTreemapTile *
KTreemapView::findTile( KFileInfo * node )
{
    if ( ! node )
	return 0;

    QCanvasItemList itemList = canvas()->allItems();
    QCanvasItemList::Iterator it = itemList.begin();

    while ( it != itemList.end() )
    {
	KTreemapTile * tile = dynamic_cast<KTreemapTile *> (*it);

	if ( tile && tile->orig() == node )
	    return tile;

	++it;
    }

    return 0;
}


QSize
KTreemapView::visibleSize()
{
    ScrollBarMode oldHMode = hScrollBarMode();
    ScrollBarMode oldVMode = vScrollBarMode();

    setHScrollBarMode( AlwaysOff );
    setVScrollBarMode( AlwaysOff );

    QSize size = QSize( QCanvasView::visibleWidth(),
			QCanvasView::visibleHeight() );

    setHScrollBarMode( oldHMode );
    setVScrollBarMode( oldVMode );

    return size;
}


QColor
KTreemapView::tileColor( KFileInfo * file )
{
    if ( file )
    {
	if ( file->isFile() )
	{
	    // Find the filename extension: Everything after the first '.'
	    QString ext = file->name().section( '.', 1 );

	    while ( ! ext.isEmpty() )
	    {
		QString lowerExt = ext.lower();

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

		if ( lowerExt == "html"	)	return Qt::blue;
		if ( lowerExt == "htm"	)	return Qt::blue;
		if ( lowerExt == "txt"	)	return Qt::blue;
		if ( lowerExt == "doc"	)	return Qt::blue;

		if ( lowerExt == "png"	)	return Qt::cyan;
		if ( lowerExt == "jpg"	)	return Qt::cyan;
		if ( lowerExt == "jpeg"	)	return Qt::cyan;
		if ( lowerExt == "gif"	)	return Qt::cyan;
		if ( lowerExt == "tif"	)	return Qt::cyan;
		if ( lowerExt == "tiff"	)	return Qt::cyan;
		if ( lowerExt == "bmp"	)	return Qt::cyan;
		if ( lowerExt == "xpm"	)	return Qt::cyan;
		if ( lowerExt == "tga"	)	return Qt::cyan;

		if ( lowerExt == "wav"	)	return Qt::yellow;
		if ( lowerExt == "mp3"	)	return Qt::yellow;

		if ( lowerExt == "avi"	)	return QColor( 0xa0, 0xff, 0x00 );
		if ( lowerExt == "mov"	)	return QColor( 0xa0, 0xff, 0x00 );
		if ( lowerExt == "mpg"	)	return QColor( 0xa0, 0xff, 0x00 );
		if ( lowerExt == "mpeg"	)	return QColor( 0xa0, 0xff, 0x00 );

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






KTreemapSelectionRect::KTreemapSelectionRect( QCanvas * canvas, const QColor & color )
    : QCanvasRectangle( canvas )
{
    setPen( QPen( color, 2 ) );
    setZ( 1e10 );		// Higher than everything else
}



void
KTreemapSelectionRect::highlight( KTreemapTile * tile )
{
    if ( tile )
    {
	QRect tileRect = tile->rect();

	move( tileRect.x(), tileRect.y() );
	setSize( tileRect.width(), tileRect.height() );

	if ( ! isVisible() )
	    show();
    }
    else
    {
	if ( isVisible() )
	    hide();
    }
}



// EOF
