/*
 *   File name: SizeColDelegate.cpp
 *   Summary:	DirTreeView delegate for the size column
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include <QPainter>
#include <QTreeView>
#include <QApplication>

#include "SizeColDelegate.h"
#include "DirTreeModel.h"
#include "Qt4Compat.h"
#include "Exception.h"
#include "FileInfo.h"
#include "Logger.h"

#define ALLOC_COLOR_NORMAL      "#6666FF"
#define ALLOC_COLOR_DARK_THEME  "#CCCCFF"

#define MARGIN_RIGHT    2
#define MARGIN_LEFT     8
#define MARGIN_TOP      2
#define MARGIN_BOTTOM   2

using namespace QDirStat;


SizeColDelegate::SizeColDelegate( QTreeView * treeView ):
    QStyledItemDelegate( treeView ),
    _treeView( treeView ),
    _model( 0 )
{
    QColor background = qAppPalette().color( QPalette::Active, QPalette::Base );
    _usingDarkTheme = background.lightness() < 128; // 0 (black) .. 255 (white)
}


SizeColDelegate::~SizeColDelegate()
{
    // NOP
}


void SizeColDelegate::paint( QPainter		        * painter,
                             const QStyleOptionViewItem & option,
                             const QModelIndex	        & index ) const
{
    if ( ! index.isValid() || index.column() != SizeCol )
	return QStyledItemDelegate::paint( painter, option, index );

    ensureModel( index );

    if ( _model )
    {
        FileInfo * item = _model->itemFromIndex( index );

        if ( item )
        {
            if ( DirTreeModel::isSmallFileOrSymLink( item ) &&
                 item->links() == 1 )
            {
                // logDebug() << "Small file " << item << endl;

                QString text = _model->data( index, Qt::DisplayRole ).toString();
                QStringList fields = text.split( " (" );  //  "137 B (4k)"

                if ( fields.size() == 2 )
                {
                    QRect rect           = option.rect;
                    const QPalette & pal = option.palette;
                    QColor textColor     = pal.color( item->isIgnored() ?
                                                      QPalette::Disabled : QPalette::Normal,
                                                      QPalette::Text );

                    int alignment        = Qt::AlignRight | Qt::AlignVCenter;
                    bool invertColor     = _usingDarkTheme;

                    if ( option.state & QStyle::State_Selected )
                    {
                        invertColor = ! invertColor;
                        painter->fillRect( rect, pal.highlight() );
                    }

                    QString sizeText  = fields.takeFirst();             // "137 B"
                    QString allocText = " (" + fields.takeFirst();      // " (4k)"

                    // Draw the size ("137 B").
                    //
                    // Since we align right, we need to move the rectangle to the left
                    // to reserve some space for the allocated size.

                    QFontMetrics fontMetrics( option.font );
                    int allocWidth = fontMetrics.width( allocText );
                    rect.setWidth( rect.width() - allocWidth );

                    painter->setPen( textColor );
                    painter->drawText( rect, alignment, sizeText );


                    // Draw the allocated size (" (4k)").

                    QColor allocColor( invertColor ?
                                       ALLOC_COLOR_DARK_THEME :
                                       ALLOC_COLOR_NORMAL );

                    rect = option.rect;
                    rect.setWidth( rect.width() );

                    painter->setPen( allocColor );
                    painter->drawText( rect, alignment, allocText );

                    return;
                }
            }
        }
    }

    // logDebug() << "Using fallback" << endl;

    // Fallback: Use the standard delegate.
    return QStyledItemDelegate::paint( painter, option, index );
}


QSize SizeColDelegate::sizeHint( const QStyleOptionViewItem & option,
                                 const QModelIndex	    & index) const
{
    if ( index.isValid() && index.column() == SizeCol )
    {
        ensureModel( index );

        if ( _model )
        {
            FileInfo * item = _model->itemFromIndex( index );

            if ( DirTreeModel::isSmallFileOrSymLink( item ) &&
                 item->links() == 1                            )
            {
                QString text = _model->data( index, Qt::DisplayRole ).toString();
                QFontMetrics fontMetrics( option.font );
                int width  = fontMetrics.width( text );
                int height = fontMetrics.height();
                QSize size( width  + MARGIN_RIGHT + MARGIN_LEFT,
                            height + MARGIN_TOP   + MARGIN_BOTTOM );
#if 0
                logDebug() << "size hint for \"" << text << "\": "
                           << size.width() << ", " << size.height() << endl;
#endif
                return size;
            }
        }
    }

    // logDebug() << "Using fallback" << endl;

#if 0
    QSize size = QStyledItemDelegate::sizeHint( option, index );
    return QSize( size.width() + MARGIN_RIGHT + MARGIN_LEFT, size.height() );
#endif

    return QStyledItemDelegate::sizeHint( option, index );
}


void SizeColDelegate::ensureModel( const QModelIndex & index ) const
{
    if ( ! _model )
    {
        const DirTreeModel * constModel =
            dynamic_cast<const DirTreeModel *>( index.model() );

        // This mess with const_cast and mutable DirTreeModel * is only
        // necessary because the Trolls in their infinite wisdom saw fit to
        // make this whole item/view stuff as inaccessible as they possibly
        // could: The QModelIndex only stores a CONST pointer to the model, and
        // the paint() method is const for whatever reason.
        //
        // Adding insult to injury, the creation order is view, delegate,
        // model, and then the model is put into the view; so we can't simply
        // put the model into the delegate in the constructor; we have to get
        // it out somewhere, and what better place is there than from a
        // QModelIndex? But no, they nail everything down with this "const"
        // insanity. There is nothing wrong with data encapsulation, but there
        // is such a thing as making classes pretty much unusable; Qt's model /
        // view classes are a classic example.
        //
        // I wish some day they might come out of their ivory tower and meet
        // the real life. Seriously: WTF?!

        if ( constModel )
            _model = const_cast<DirTreeModel *>( constModel );

        if ( ! _model )
            logError() << "WRONG_MODEL TYPE" << endl;
    }
}

