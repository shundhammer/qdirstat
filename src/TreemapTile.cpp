/*
 *   File name: TreemapTile.cpp
 *   Summary:	Treemap rendering for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <math.h>

#include <QImage>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

#include "TreemapTile.h"
#include "TreemapView.h"
#include "SelectionModel.h"
#include "ActionManager.h"
#include "CleanupCollection.h"
#include "Exception.h"
#include "Logger.h"

using namespace QDirStat;


TreemapTile::TreemapTile( TreemapView *	 parentView,
			  TreemapTile *	 parentTile,
			  FileInfo *	 orig,
			  const QRectF & rect,
			  Orientation	 orientation ):
    QGraphicsRectItem( rect, parentTile ),
    _parentView( parentView ),
    _parentTile( parentTile ),
    _orig( orig )
{
    // logDebug() << "Creating tile without cushion for " << orig << "	" << rect << endl;
    init();

    if ( parentTile )
	_cushionSurface = parentTile->cushionSurface();

    createChildren( rect, orientation );
}


TreemapTile::TreemapTile( TreemapView *		 parentView,
			  TreemapTile *		 parentTile,
			  FileInfo    *		 orig,
			  const QRectF &	 rect,
			  const CushionSurface & cushionSurface,
			  Orientation		 orientation ):
    QGraphicsRectItem( rect, parentTile ),
    _parentView( parentView ),
    _parentTile( parentTile ),
    _orig( orig ),
    _cushionSurface( cushionSurface )
{
    // logDebug() << "Creating cushioned tile for " << orig << "  " << rect << endl;
    init();

    // Intentionally not copying the parent's cushion surface!

    createChildren( rect, orientation );
}


TreemapTile::~TreemapTile()
{
    // DO NOT try to delete the _highlighter: It is owned by the TreemapView /
    // QGraphicsScene and deleted together with all other QGraphicsItems
    // in the TreemapView destructor.
}


void TreemapTile::init()
{
    // Set up height (z coordinate) - one level higher than the parent so this
    // will be closer to the foreground.

    setZValue( _parentTile ? ( _parentTile->zValue() + 1.0 ) : 0.0 );

    setBrush( QColor( 0x60, 0x60, 0x60 ) );
    setPen( Qt::NoPen );

    if ( _orig->isDir() || _orig->isDotEntry() )
    {
        if ( _parentView->useDirGradient() )
        {
            if ( qMax( rect().width(), rect().height() ) < _parentView->minTileSize() )
            {
                setBrush( Qt::NoBrush );
            }
            else
            {
                QLinearGradient gradient( rect().topLeft(), rect().bottomRight() );
                gradient.setColorAt( 0.0, _parentView->dirGradientStart() );
                gradient.setColorAt( 1.0, _parentView->dirGradientEnd()   );
                setBrush( gradient );
            }
        }
    }

    setFlags( ItemIsSelectable );
    _highlighter = 0;

    if ( ( _orig->isDir() && _orig->totalSubDirs() == 0 ) || _orig->isDotEntry() )
        setAcceptHoverEvents( true );

    if ( ! _parentTile )
	_parentView->scene()->addItem( this );

    // logDebug() << "Creating treemap tile for " << this
    //		  << " size " << formatSize( _orig->totalAllocatedSize() ) << endl;
}


void TreemapTile::createChildren( const QRectF & rect,
				  Orientation	 orientation )
{
    if ( _orig->totalAllocatedSize() == 0 )	// Prevent division by zero
	return;

    if ( _parentView->squarify() )
	createSquarifiedChildren( rect );
    else
	createChildrenSimple( rect, orientation );
}


void TreemapTile::createChildrenSimple( const QRectF & rect,
					Orientation    orientation )
{
    Orientation dir	 = orientation;
    Orientation childDir = orientation;

    if ( dir == TreemapAuto )
	dir = rect.width() > rect.height() ? TreemapHorizontal : TreemapVertical;

    if ( orientation == TreemapHorizontal )  childDir = TreemapVertical;
    if ( orientation == TreemapVertical	  )  childDir = TreemapHorizontal;

    int offset	 = 0;
    int size	 = dir == TreemapHorizontal ? rect.width() : rect.height();
    int count	 = 0;
    double scale = (double) size / (double) _orig->totalAllocatedSize();

    _cushionSurface.addRidge( childDir, _cushionSurface.height(), rect );
    FileSize minSize = (FileSize) ( _parentView->minTileSize() / scale );
    FileInfoSortedBySizeIterator it( _orig, minSize );

    while ( *it )
    {
	int childSize = 0;

	childSize = (int) ( scale * (*it)->totalAllocatedSize() );

	if ( childSize >= _parentView->minTileSize() )
	{
	    QRectF childRect;

	    if ( dir == TreemapHorizontal )
		childRect = QRectF( rect.x() + offset, rect.y(), childSize, rect.height() );
	    else
		childRect = QRectF( rect.x(), rect.y() + offset, rect.width(), childSize );

	    TreemapTile * tile = new TreemapTile( _parentView, this, *it, childRect, childDir );
	    CHECK_NEW( tile );

	    tile->cushionSurface().addRidge( dir,
					     _cushionSurface.height() * _parentView->heightScaleFactor(),
					     childRect );

	    offset += childSize;
	}

	++count;
	++it;
    }
}


void TreemapTile::createSquarifiedChildren( const QRectF & rect )
{
    if ( _orig->totalAllocatedSize() == 0 )
    {
	logError()  << "Zero totalAllocatedSize()" << endl;
	return;
    }

    double scale	= rect.width() * (double) rect.height() / _orig->totalAllocatedSize();
    FileSize minSize	= (FileSize) ( _parentView->minTileSize() / scale );

    FileInfoSortedBySizeIterator it( _orig, minSize );
    QRectF childrenRect = rect;

    while ( *it )
    {
	FileInfoList row = squarify( childrenRect, scale, it );
	childrenRect = layoutRow( childrenRect, scale, row );
    }
}


FileInfoList TreemapTile::squarify( const QRectF & rect,
				    double	  scale,
				    FileInfoSortedBySizeIterator & it )
{
    // logDebug() << "squarify() " << this << " " << rect << endl;

    FileInfoList row;
    int length = qMax( rect.width(), rect.height() );

    if ( length == 0 )	// Sanity check
    {
	logWarning()  << "Zero length" << endl;

	if ( *it )	// Prevent endless loop in case of error:
	    ++it;	// Advance iterator.

	return row;
    }


    bool   improvingAspectRatio = true;
    double lastWorstAspectRatio = -1.0;
    double sum			= 0;

    // This is a bit ugly, but doing all calculations in the 'size' dimension
    // is more efficient here since that requires only one scaling before
    // doing all other calculations in the loop.
    const double scaledLengthSquare = length * (double) length / scale;

    while ( *it && improvingAspectRatio )
    {
	sum += (*it)->totalAllocatedSize();

	if ( ! row.isEmpty() && sum != 0 && (*it)->totalAllocatedSize() != 0 )
	{
	    double sumSquare	    = sum * sum;
	    double worstAspectRatio = qMax( scaledLengthSquare * row.first()->totalAllocatedSize() / sumSquare,
                                            sumSquare / ( scaledLengthSquare * (*it)->totalAllocatedSize() ) );

	    if ( lastWorstAspectRatio >= 0.0 &&
		 worstAspectRatio > lastWorstAspectRatio )
	    {
		improvingAspectRatio = false;
	    }

	    lastWorstAspectRatio = worstAspectRatio;
	}

	if ( improvingAspectRatio )
	{
	    // logDebug() << "Adding " << *it << " size " << (*it)->totalAllocatedSize() << endl;
	    row.append( *it );
	    ++it;
	}
	else
	{
	    // logDebug() << "Getting worse after adding " << *it << " size " << (*it)->totalAllocatedSize() << endl;
	}
    }

    return row;
}


QRectF TreemapTile::layoutRow( const QRectF & rect,
			       double	      scale,
			       FileInfoList & row )
{
    if ( row.isEmpty() )
	return rect;

    // Determine the direction in which to subdivide.
    // We always use the longer side of the rectangle.
    Orientation dir = rect.width() > rect.height() ? TreemapHorizontal : TreemapVertical;

    // This row's primary length is the longer one.
    int primary = qMax( rect.width(), rect.height() );

    // This row's secondary length is determined by the area (the number of
    // pixels) to be allocated for all of the row's items.

    FileSize sum = 0;

    foreach ( FileInfo * item, row )
	sum += item->totalAllocatedSize();

    int secondary = (int) ( sum * scale / primary );

    if ( sum == 0 )	// Prevent division by zero.
	return rect;

    if ( secondary < _parentView->minTileSize() )	// We don't want tiles that small.
	return rect;


    // Set up a cushion surface for this layout row:
    // Add another ridge perpendicular to the row's direction
    // that optically groups this row's tiles together.

    CushionSurface rowCushionSurface = _cushionSurface;

    rowCushionSurface.addRidge( dir == TreemapHorizontal ? TreemapVertical : TreemapHorizontal,
				_cushionSurface.height() * _parentView->heightScaleFactor(),
				rect );

    int offset = 0;
    int remaining = primary;
    FileInfoList::const_iterator it  = row.constBegin();
    FileInfoList::const_iterator end = row.constEnd();

    while ( it != end )
    {
	int childSize = (int) ( (*it)->totalAllocatedSize() / (double) sum * primary + 0.5 );

	if ( childSize > remaining )	// Prevent overflow because of accumulated rounding errors
	    childSize = remaining;

	remaining -= childSize;

	if ( childSize >= _parentView->minTileSize() )
	{
	    QRectF childRect;

	    if ( dir == TreemapHorizontal )
		childRect = QRectF( rect.x() + offset, rect.y(), childSize, secondary );
	    else
		childRect = QRectF( rect.x(), rect.y() + offset, secondary, childSize );

	    TreemapTile * tile = new TreemapTile( _parentView, this, *it, childRect, rowCushionSurface );
	    CHECK_NEW( tile );

	    tile->cushionSurface().addRidge( dir,
					     rowCushionSurface.height() * _parentView->heightScaleFactor(),
					     childRect );
	    offset += childSize;
	}

	++it;
    }


    // Subtract the layouted area from the rectangle.

    QRectF newRect;

    if ( dir == TreemapHorizontal )
	newRect = QRectF( rect.x(), rect.y() + secondary, rect.width(), rect.height() - secondary );
    else
	newRect = QRectF( rect.x() + secondary, rect.y(), rect.width() - secondary, rect.height() );

    // logDebug() << "Left over:" << " " << newRect << " " << this << endl;

    return newRect;
}


void TreemapTile::paint( QPainter			* painter,
			 const QStyleOptionGraphicsItem * option,
			 QWidget			* widget )
{
    // logDebug() << this << "	 " << rect() << endl;

    QSizeF size = rect().size();

    if ( size.height() < 1.0 || size.width() < 1.0 )
	return;

    if ( _parentView->doCushionShading() )
    {
	if ( _orig->isDir() || _orig->isDotEntry() )
	{
	    QGraphicsRectItem::paint( painter, option, widget );
	}
	else
	{
	    if ( _cushion.isNull() )
		_cushion = renderCushion();

	    QRectF rect = QGraphicsRectItem::rect();

	    if ( ! _cushion.isNull() )
		painter->drawPixmap( rect.topLeft(), _cushion );

	    if ( isSelected() && ! _orig->hasChildren() )
	    {
		// Highlight this tile. This makes only sense if this is a leaf
		// tile (i.e., if the corresponding FileInfo doesn't have any
		// children), because otherwise the children will obscure this
		// tile anyway. In that case, we have to rely on a
		// HighlightRect to be created. But we can save some memory if
		// we don't do that for every tile, so we draw that highlight
		// frame manually if this is a leaf tile.

		QRectF selectionRect = rect;
		selectionRect.setSize( rect.size() - QSize( 1.0, 1.0 ) );
		painter->setBrush( Qt::NoBrush );
		painter->setPen( QPen( _parentView->selectedItemsColor(), 1 ) );
		painter->drawRect( selectionRect );
	    }

	    if ( _parentView->forceCushionGrid() )
	    {
		// Draw a clearly visible boundary

		painter->setPen( QPen( _parentView->cushionGridColor(), 1 ) );

		if ( rect.x() > 0 )
		    painter->drawLine( rect.topLeft(), rect.bottomLeft() );

		if ( rect.y() > 0 )
		    painter->drawLine( rect.topLeft(), rect.topRight() );
	    }
	}
    }
    else	// No cushion shading, use plain tiles
    {
	painter->setPen( QPen( _parentView->outlineColor(), 1 ) );

	if ( _orig->isDir() || _orig->isDotEntry() )
	{
            if ( ! _parentView->useDirGradient() )
                setBrush( _parentView->dirFillColor() );
	}
	else
	{
	    setBrush( _parentView->tileColor( _orig ) );
	}

	QGraphicsRectItem::paint( painter, option, widget );
    }
}


QPixmap TreemapTile::renderCushion()
{
    QRectF rect = QGraphicsRectItem::rect();

    if ( rect.width() < 1.0 || rect.height() < 1.0 )
	return QPixmap();

    // logDebug() << endl;

    double nx;
    double ny;
    double cosa;
    int	   x, y;
    int	   red, green, blue;


    // Cache some values. They are used for each loop iteration, so let's try
    // to keep multiple indirect references down.

    int		ambientLight = parentView()->ambientLight();
    double	lightX	     = parentView()->lightX();
    double	lightY	     = parentView()->lightY();
    double	lightZ	     = parentView()->lightZ();

    double	xx2	     = cushionSurface().xx2();
    double	xx1	     = cushionSurface().xx1();
    double	yy2	     = cushionSurface().yy2();
    double	yy1	     = cushionSurface().yy1();

    int		x0	     = rect.x();
    int		y0	     = rect.y();

    QColor	color	     = parentView()->tileColor( _orig );
    int		maxRed	     = qMax( 0, color.red()   - ambientLight );
    int		maxGreen     = qMax( 0, color.green() - ambientLight );
    int		maxBlue	     = qMax( 0, color.blue()  - ambientLight );

    QImage image( qRound( rect.width() ), qRound( rect.height() ), QImage::Format_RGB32 );

    for ( y = 0; y < rect.height(); y++ )
    {
	for ( x = 0; x < rect.width(); x++ )
	{
	    nx = 2.0 * xx2 * (x+x0) + xx1;
	    ny = 2.0 * yy2 * (y+y0) + yy1;
	    cosa  = ( nx * lightX + ny * lightY + lightZ ) / sqrt( nx*nx + ny*ny + 1.0 );

	    red	  = (int) ( maxRed   * cosa + 0.5 );
	    green = (int) ( maxGreen * cosa + 0.5 );
	    blue  = (int) ( maxBlue  * cosa + 0.5 );

	    if ( red   < 0 )	red   = 0;
	    if ( green < 0 )	green = 0;
	    if ( blue  < 0 )	blue  = 0;

	    red	  += ambientLight;
	    green += ambientLight;
	    blue  += ambientLight;

	    image.setPixel( x, y, qRgb( red, green, blue) );
	}
    }

    if ( _parentView->enforceContrast() )
	enforceContrast( image );

    return QPixmap::fromImage( image );
}


void TreemapTile::enforceContrast( QImage & image )
{
    if ( image.width() > 5 )
    {
	// Check contrast along the right image boundary:
	//
	// Compare samples from the outmost boundary to samples a few pixels to
	// the inside and count identical pixel values. A number of identical
	// pixels are tolerated, but not too many.

	int x1 = image.width() - 6;
	int x2 = image.width() - 1;
	int interval = qMax( image.height() / 10, 5 );
	int sameColorCount = 0;


	// Take samples

	for ( int y = interval; y < image.height(); y+= interval )
	{
	    if ( image.pixel( x1, y ) == image.pixel( x2, y ) )
		sameColorCount++;
	}

	if ( sameColorCount * 10 > image.height() )
	{
	    // Add a line at the right boundary

	    QRgb val = contrastingColor( image.pixel( x2, image.height() / 2 ) );

	    for ( int y = 0; y < image.height(); y++ )
		image.setPixel( x2, y, val );
	}
    }


    if ( image.height() > 5 )
    {
	// Check contrast along the bottom boundary

	int y1 = image.height() - 6;
	int y2 = image.height() - 1;
	int interval = qMax( image.width() / 10, 5 );
	int sameColorCount = 0;

	for ( int x = interval; x < image.width(); x += interval )
	{
	    if ( image.pixel( x, y1 ) == image.pixel( x, y2 ) )
		sameColorCount++;
	}

	if ( sameColorCount * 10 > image.height() )
	{
	    // Add a grey line at the bottom boundary

	    QRgb val = contrastingColor( image.pixel( image.width() / 2, y2 ) );

	    for ( int x = 0; x < image.width(); x++ )
		image.setPixel( x, y2, val );
	}
    }
}


QRgb TreemapTile::contrastingColor( QRgb col )
{
    if ( qGray( col ) < 128 )
	return qRgb( qRed( col ) * 2, qGreen( col ) * 2, qBlue( col ) * 2 );
    else
	return qRgb( qRed( col ) / 2, qGreen( col ) / 2, qBlue( col ) / 2 );
}


QVariant TreemapTile::itemChange( GraphicsItemChange   change,
				  const QVariant     & value)
{
    // logDebug() << this << endl;

    if ( change == ItemSelectedChange )
    {
	bool selected = value.toBool();
	// logDebug() << this << ( selected ? " is selected" : " is deselected" ) << endl;

	if ( _orig->hasChildren() )
	{
	    if ( ! selected && _highlighter )
		_highlighter->hide();

	    if ( selected && this != _parentView->rootTile() ) // don't highlight the root tile
	    {
		if ( ! _highlighter )
		{
		    // logDebug() << "Creating highlighter for " << this << endl;
		    _highlighter = new SelectedItemHighlighter( this, _parentView->selectedItemsColor() );
		    CHECK_NEW( _highlighter );
		}

		if ( ! _highlighter->isVisible() )
		    _highlighter->show();
	    }
	}
    }

    return QGraphicsRectItem::itemChange( change, value );
}


void TreemapTile::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
    switch ( event->button() )
    {
	case Qt::LeftButton:
	    // isSelected() is unreliable here since in QGraphicsItem some
	    // stuff is done in the mousePressEvent, while some other stuff is
	    // done in the mouseReleaseEvent. Just setting the current item
	    // here to avoid having a yellow highlighter rectangle upon mouse
	    // press and then a red one upon mouse release. No matter if the
	    // item ends up selected or not, the mouse press makes it the
	    // current item, so let's update the red highlighter rectangle
	    // here.

	    QGraphicsRectItem::mousePressEvent( event );
	    // logDebug() << this << " mouse pressed" << endl;
	    _parentView->setCurrentItem( this );
	    break;

	case Qt::MidButton:
            // logDebug() << "Middle click on " << _orig << endl;

            // Handle item selection (with or without Ctrl) ourselves here;
            // unlike for a left click, the QGraphicsItem base class does
            // not do this for us already.

            if ( ( event->modifiers() & Qt::ControlModifier ) == 0 )
                scene()->clearSelection();

            setSelected( ! isSelected() );

            _parentView->setCurrentItem( this );
            _parentView->toggleParentsHighlight( this );
            break;


	case Qt::RightButton:
	    // logDebug() << this << " right mouse pressed" << endl;
	    _parentView->setCurrentItem( this );
	    break;

	default:
	    QGraphicsRectItem::mousePressEvent( event );
	    break;
    }
}


void TreemapTile::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
    switch ( event->button() )
    {
	case Qt::LeftButton:
	    {
		// The current item was already set in the mouse press event,
		// but it might have changed its 'selected' status right now,
		// so let the view update it.

		QGraphicsRectItem::mouseReleaseEvent( event );
		_parentView->setCurrentItem( this );
		// logDebug() << this << " clicked; selected: " << isSelected() << endl;
	    }
	    break;

	default:
	    QGraphicsRectItem::mouseReleaseEvent( event );
	    break;
    }

    _parentView->sendSelection();
}


void TreemapTile::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * event )
{
    switch ( event->button() )
    {
	case Qt::LeftButton:
	    logDebug() << "Zooming treemap in" << endl;
	    _parentView->zoomIn();
	    break;

	case Qt::MidButton:
	    logDebug() << "Zooming treemap out" << endl;
	    _parentView->zoomOut();
	    break;

	case Qt::RightButton:
	    // This doesn't work at all since the first click already opens the
	    // context menu which grabs the focus to that pop-up menu.
	    break;

	default:
	    break;
    }
}


void TreemapTile::wheelEvent( QGraphicsSceneWheelEvent * event )
{
    if ( event->delta() > 0 )
    {
	if ( ! _parentView->currentItem() )  // can only zoom in with a current item
	    _parentView->setCurrentItem( this );

	_parentView->zoomIn();
    }
    else if ( event->delta() < 0 )
    {
	_parentView->zoomOut();
    }
}


void TreemapTile::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if ( ! _parentView->selectionModel() )
	return;

    FileInfoSet selectedItems = _parentView->selectionModel()->selectedItems();

    if ( ! selectedItems.contains( _orig ) )
    {
	logDebug() << "Abandoning old selection" << endl;
	_parentView->selectionModel()->setCurrentItem( _orig, true );
	selectedItems = _parentView->selectionModel()->selectedItems();
    }

    if ( _parentView->selectionModel()->verbose() )
	_parentView->selectionModel()->dumpSelectedItems();

    logDebug() << "Context menu for " << this << endl;

    QMenu menu;
    QStringList actions;
    actions << "actionGoUp"
	    << "actionCopyPathToClipboard"
	    << "---"
	    << "actionTreemapZoomIn"
	    << "actionTreemapZoomOut"
	    << "actionResetTreemapZoom"
	    << "---"
	    << "actionMoveToTrash"
	;

    ActionManager::instance()->addActions( &menu, actions );

    if ( _parentView->cleanupCollection() &&
	 ! _parentView->cleanupCollection()->isEmpty() )
    {
	menu.addSeparator();
	_parentView->cleanupCollection()->addToMenu( &menu );
    }

    menu.exec( event->screenPos() );
}


void TreemapTile::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
    Q_UNUSED( event );

    // logDebug() << "Hovering over " << this << endl;
    _parentView->sendHoverEnter( _orig );
}


void TreemapTile::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
    Q_UNUSED( event );

    // logDebug() << "  Leaving " << this << endl;
    _parentView->sendHoverLeave( _orig );
}


//
//---------------------------------------------------------------------------
//


CushionSurface::CushionSurface()
{
    _xx2    = 0.0;
    _xx1    = 0.0;
    _yy2    = 0.0;
    _yy1    = 0.0;
    _height = CushionHeight;
}


void CushionSurface::addRidge( Orientation dim, double height, const QRectF & rect )
{
    _height = height;

    if ( dim == TreemapHorizontal )
    {
	_xx2 = squareRidge( _xx2, _height, rect.left(), rect.right() );
	_xx1 = linearRidge( _xx1, _height, rect.left(), rect.right() );
    }
    else
    {
	_yy2 = squareRidge( _yy2, _height, rect.top(), rect.bottom() );
	_yy1 = linearRidge( _yy1, _height, rect.top(), rect.bottom() );
    }
}


double CushionSurface::squareRidge( double squareCoefficient, double height, int x1, int x2 )
{
    if ( x2 != x1 ) // Avoid division by zero
	squareCoefficient -= 4.0 * height / ( x2 - x1 );

    return squareCoefficient;
}


double CushionSurface::linearRidge( double linearCoefficient, double height, int x1, int x2 )
{
    if ( x2 != x1 ) // Avoid division by zero
	linearCoefficient += 4.0 * height * ( x2 + x1 ) / ( x2 - x1 );

    return linearCoefficient;
}


