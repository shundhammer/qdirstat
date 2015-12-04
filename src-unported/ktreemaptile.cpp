/*
 *   File name:	ktreemaptile.cpp
 *   Summary:	High level classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <math.h>
#include <algorithm>

#include <kapp.h>
#include <klocale.h>
#include <kglobal.h>
#include <qimage.h>
#include <qpainter.h>

#include "ktreemaptile.h"
#include "ktreemapview.h"
#include "kdirtreeiterators.h"
#include "kdirtreeview.h"


using namespace KDirStat;
using std::max;
using std::min;


KTreemapTile::KTreemapTile( KTreemapView *	parentView,
			    KTreemapTile *	parentTile,
			    KFileInfo *		orig,
			    const QRect &	rect,
			    KOrientation	orientation )
    : QCanvasRectangle( rect, parentView->canvas() )
    , _parentView( parentView )
    , _parentTile( parentTile )
    , _orig( orig )
{
    init();

    if ( parentTile )
	_cushionSurface = parentTile->cushionSurface();

    createChildren( rect, orientation );
}


KTreemapTile::KTreemapTile( KTreemapView *		parentView,
			    KTreemapTile *		parentTile,
			    KFileInfo *			orig,
			    const QRect &		rect,
			    const KCushionSurface &	cushionSurface,
			    KOrientation		orientation )
    : QCanvasRectangle( rect, parentView->canvas() )
    , _parentView( parentView )
    , _parentTile( parentTile )
    , _orig( orig )
    , _cushionSurface( cushionSurface )
{
    init();

    // Intentionally not copying the parent's cushion surface!

    createChildren( rect, orientation );
}


KTreemapTile::~KTreemapTile()
{
    // NOP
}


void
KTreemapTile::init()
{
    // Set up height (z coordinate) - one level higher than the parent so this
    // will be closer to the foreground.
    //
    // Note that this must happen before any children are created.
    // I found that out the hard way. ;-)

    setZ( _parentTile ? ( _parentTile->z() + 1.0 ) : 0.0 );

    setBrush( QColor( 0x60, 0x60, 0x60 ) );
    setPen( NoPen );

    show();	// QCanvasItems are invisible by default!

    // kdDebug() << "Creating treemap tile for " << _orig
    //           << " size " << formatSize( _orig->totalSize() ) << endl;
}


void
KTreemapTile::createChildren( const QRect &	rect,
			      KOrientation	orientation )
{
    if ( _orig->totalSize() == 0 )	// Prevent division by zero
	return;

    if ( _parentView->squarify() )
	createSquarifiedChildren( rect );
    else
	createChildrenSimple( rect, orientation );
}


void
KTreemapTile::createChildrenSimple( const QRect &	rect,
				    KOrientation	orientation )
{

    KOrientation dir      = orientation;
    KOrientation childDir = orientation;

    if ( dir == KTreemapAuto )
	dir = rect.width() > rect.height() ? KTreemapHorizontal : KTreemapVertical;

    if ( orientation == KTreemapHorizontal	)	childDir = KTreemapVertical;
    if ( orientation == KTreemapVertical	)	childDir = KTreemapHorizontal;

    int offset	 = 0;
    int size	 = dir == KTreemapHorizontal ? rect.width() : rect.height();
    int count 	 = 0;
    double scale = (double) size / (double) _orig->totalSize();

    _cushionSurface.addRidge( childDir, _cushionSurface.height(), rect );

    KFileInfoSortedBySizeIterator it( _orig,
				      (KFileSize) ( _parentView->minTileSize() / scale ),
				      KDotEntryAsSubDir );

    while ( *it )
    {
	int childSize = 0;

	childSize = (int) ( scale * (*it)->totalSize() );

	if ( childSize >= _parentView->minTileSize() )
	{
	    QRect childRect;

	    if ( dir == KTreemapHorizontal )
		childRect = QRect( rect.x() + offset, rect.y(), childSize, rect.height() );
	    else
		childRect = QRect( rect.x(), rect.y() + offset, rect.width(), childSize );

	    KTreemapTile * tile = new KTreemapTile( _parentView, this, *it, childRect, childDir );
	    CHECK_PTR( tile );

	    tile->cushionSurface().addRidge( dir,
					     _cushionSurface.height() * _parentView->heightScaleFactor(),
					     childRect );

	    offset += childSize;
	}

	++count;
	++it;
    }
}


void
KTreemapTile::createSquarifiedChildren( const QRect & rect )
{
    if ( _orig->totalSize() == 0 )
    {
	kdError() << k_funcinfo << "Zero totalSize()" << endl;
	return;
    }

    double scale	= rect.width() * (double) rect.height() / _orig->totalSize();
    KFileSize minSize	= (KFileSize) ( _parentView->minTileSize() / scale );

#if 0
    if ( _orig->hasChildren() )
    {
	_cushionSurface.addRidge( KTreemapHorizontal, _cushionSurface.height(), rect );
	_cushionSurface.addRidge( KTreemapVertical,   _cushionSurface.height(), rect );
    }
#endif

    KFileInfoSortedBySizeIterator it( _orig, minSize, KDotEntryAsSubDir );
    QRect childrenRect = rect;

    while ( *it )
    {
	KFileInfoList row = squarify( childrenRect, scale, it );
	childrenRect = layoutRow( childrenRect, scale, row );
    }
}


KFileInfoList
KTreemapTile::squarify( const QRect & 			rect,
			double				scale,
			KFileInfoSortedBySizeIterator & it   )
{
    // kdDebug() << "squarify() " << _orig << " " << rect << endl;

    KFileInfoList row;
    int length = max( rect.width(), rect.height() );

    if ( length == 0 )	// Sanity check
    {
	kdWarning() << k_funcinfo << "Zero length" << endl;

	if ( *it )	// Prevent endless loop in case of error:
	    ++it;	// Advance iterator.

	return row;
    }


    bool	improvingAspectRatio 	= true;
    double	lastWorstAspectRatio	= -1.0;
    double	sum 			= 0;

    // This is a bit ugly, but doing all calculations in the 'size' dimension
    // is more efficient here since that requires only one scaling before
    // doing all other calculations in the loop.
    const double scaledLengthSquare = length * (double) length / scale;

    while ( *it && improvingAspectRatio )
    {
	sum += (*it)->totalSize();

	if ( ! row.isEmpty() && sum != 0 && (*it)->totalSize() != 0 )
	{
	    double sumSquare        = sum * sum;
	    double worstAspectRatio = max( scaledLengthSquare * row.first()->totalSize() / sumSquare,
					   sumSquare / ( scaledLengthSquare * (*it)->totalSize() ) );

	    if ( lastWorstAspectRatio >= 0.0 &&
		worstAspectRatio > lastWorstAspectRatio )
	    {
		improvingAspectRatio = false;
	    }

	    lastWorstAspectRatio = worstAspectRatio;
	}

	if ( improvingAspectRatio )
	{
	    // kdDebug() << "Adding " << *it << " size " << (*it)->totalSize() << endl;
	    row.append( *it );
	    ++it;
	}
	else
	{
	    // kdDebug() << "Getting worse after adding " << *it << " size " << (*it)->totalSize() << endl;
	}
    }

    return row;
}



QRect
KTreemapTile::layoutRow( const QRect &		rect,
			 double			scale,
			 KFileInfoList & 	row )
{
    if ( row.isEmpty() )
	return rect;

    // Determine the direction in which to subdivide.
    // We always use the longer side of the rectangle.
    KOrientation dir = rect.width() > rect.height() ? KTreemapHorizontal : KTreemapVertical;

    // This row's primary length is the longer one.
    int primary = max( rect.width(), rect.height() );

    // This row's secondary length is determined by the area (the number of
    // pixels) to be allocated for all of the row's items.
    KFileSize sum = row.sumTotalSizes();
    int secondary = (int) ( sum * scale / primary );

    if ( sum == 0 )	// Prevent division by zero.
	return rect;

    if ( secondary < _parentView->minTileSize() )	// We don't want tiles that small.
	return rect;


    // Set up a cushion surface for this layout row:
    // Add another ridge perpendicular to the row's direction
    // that optically groups this row's tiles together.

    KCushionSurface rowCushionSurface = _cushionSurface;

    rowCushionSurface.addRidge( dir == KTreemapHorizontal ? KTreemapVertical : KTreemapHorizontal,
				_cushionSurface.height() * _parentView->heightScaleFactor(),
				rect );

    int offset = 0;
    int remaining = primary;
    KFileInfoListIterator it( row );

    while ( *it )
    {
	int childSize = (int) ( (*it)->totalSize() / (double) sum * primary + 0.5 );

	if ( childSize > remaining )	// Prevent overflow because of accumulated rounding errors
	    childSize = remaining;

	remaining -= childSize;

	if ( childSize >= _parentView->minTileSize() )
	{
	    QRect childRect;

	    if ( dir == KTreemapHorizontal )
		childRect = QRect( rect.x() + offset, rect.y(), childSize, secondary );
	    else
		childRect = QRect( rect.x(), rect.y() + offset, secondary, childSize );

	    KTreemapTile * tile = new KTreemapTile( _parentView, this, *it, childRect, rowCushionSurface );
	    CHECK_PTR( tile );

	    tile->cushionSurface().addRidge( dir,
					     rowCushionSurface.height() * _parentView->heightScaleFactor(),
					     childRect );
	    offset += childSize;
	}

	++it;
    }


    // Subtract the layouted area from the rectangle.

    QRect newRect;

    if ( dir == KTreemapHorizontal )
	newRect = QRect( rect.x(), rect.y() + secondary, rect.width(), rect.height() - secondary );
    else
	newRect = QRect( rect.x() + secondary, rect.y(), rect.width() - secondary, rect.height() );

    // kdDebug() << "Left over:" << " " << newRect << " " << _orig << endl;

    return newRect;
}


void
KTreemapTile::drawShape( QPainter & painter )
{
    // kdDebug() << "drawShape() " << _orig << endl;

    QSize size = rect().size();

    if ( size.height() < 1 || size.width() < 1 )
	return;

    if ( _parentView->doCushionShading() )
    {
	if ( _orig->isDir() || _orig->isDotEntry() )
	{
	    QCanvasRectangle::drawShape( painter );
	}
	else
	{
	    if ( _cushion.isNull() )
		_cushion = renderCushion();

	    QRect rect = QCanvasRectangle::rect();

	    if ( ! _cushion.isNull() )
		painter.drawPixmap( rect, _cushion );

	    if ( _parentView->forceCushionGrid() )
	    {
		// Draw a clearly visible boundary

		painter.setPen( QPen( _parentView->cushionGridColor(), 1 ) );

		if ( rect.x() > 0 )
		    painter.drawLine( rect.topLeft(), rect.bottomLeft() + QPoint( 0, 1 ) );

		if ( rect.y() > 0 )
		    painter.drawLine( rect.topLeft(), rect.topRight() + QPoint( 1, 0 ) );
	    }
	}
    }
    else	// No cushion shading, use plain tiles
    {
	painter.setPen( QPen( _parentView->outlineColor(), 1 ) );

	if ( _orig->isDir() || _orig->isDotEntry() )
	    painter.setBrush( _parentView->dirFillColor() );
	else
	{
	    painter.setBrush( _parentView->tileColor( _orig ) );
#if 0
	    painter.setBrush( _parentView->fileFillColor() );
#endif
	}

	QCanvasRectangle::drawShape( painter );
    }
}


QPixmap
KTreemapTile::renderCushion()
{
    QRect rect = QCanvasRectangle::rect();

    if ( rect.width() < 1 || rect.height() < 1 )
	return QPixmap();

    // kdDebug() << k_funcinfo << endl;

    double 	nx;
    double 	ny;
    double 	cosa;
    int		x, y;
    int		red, green, blue;


    // Cache some values. They are used for each loop iteration, so let's try
    // to keep multiple indirect references down.

    int		ambientLight	= parentView()->ambientLight();
    double 	lightX		= parentView()->lightX();
    double 	lightY		= parentView()->lightY();
    double 	lightZ		= parentView()->lightZ();

    double	xx2		= cushionSurface().xx2();
    double	xx1		= cushionSurface().xx1();
    double	yy2		= cushionSurface().yy2();
    double	yy1		= cushionSurface().yy1();

    int		x0 		= rect.x();
    int		y0 		= rect.y();

    QColor	color		= parentView()->tileColor( _orig );
    int		maxRed		= max( 0, color.red()   - ambientLight );
    int		maxGreen	= max( 0, color.green() - ambientLight );
    int		maxBlue		= max( 0, color.blue()  - ambientLight );

    QImage image( rect.width(), rect.height(), 32 );

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

	    red   += ambientLight;
	    green += ambientLight;
	    blue  += ambientLight;

	    image.setPixel( x, y, qRgb( red, green, blue) );
	}
    }

    if ( _parentView->ensureContrast() )
	ensureContrast( image );

    return QPixmap( image );
}


void
KTreemapTile::ensureContrast( QImage & image )
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
	int interval = max( image.height() / 10, 5 );
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
	int interval = max( image.width() / 10, 5 );
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


QRgb
KTreemapTile::contrastingColor( QRgb col )
{
    if ( qGray( col ) < 128 )
	return qRgb( qRed( col ) * 2, qGreen( col ) * 2, qBlue( col ) * 2 );
    else
	return qRgb( qRed( col ) / 2, qGreen( col ) / 2, qBlue( col ) / 2 );
}




KCushionSurface::KCushionSurface()
{
    _xx2 	= 0.0;
    _xx1 	= 0.0;
    _yy2 	= 0.0;
    _yy1 	= 0.0;
    _height	= CushionHeight;
}


void
KCushionSurface::addRidge( KOrientation dim, double height, const QRect & rect )
{
    _height = height;

    if ( dim == KTreemapHorizontal )
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


double
KCushionSurface::squareRidge( double squareCoefficient, double height, int x1, int x2 )
{
    if ( x2 != x1 ) // Avoid division by zero
	squareCoefficient -= 4.0 * height / ( x2 - x1 );

    return squareCoefficient;
}


double
KCushionSurface::linearRidge( double linearCoefficient, double height, int x1, int x2 )
{
    if ( x2 != x1 ) // Avoid division by zero
	linearCoefficient += 4.0 * height * ( x2 + x1 ) / ( x2 - x1 );

    return linearCoefficient;
}




// EOF
