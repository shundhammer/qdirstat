/*
 *   File name: PopupLabel.cpp
 *   Summary:	QLabel with a pop-up
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMenu>
#include <QMouseEvent>

#include "PopupLabel.h"
#include "Logger.h"

using namespace QDirStat;


PopupLabel::PopupLabel( QWidget * parent ):
    QLabel( parent )
{

}


PopupLabel::~PopupLabel()
{
    // NOP
}


void PopupLabel::clear()
{
    _contextText.clear();
    QLabel::clear();
    setBold( false );
}


void PopupLabel::setBold( bool bold )
{
    QFont textFont = font();
    textFont.setBold( bold );
    setFont( textFont );
}


bool PopupLabel::haveContextMenu() const
{
    return ! _contextText.isEmpty();
}


void PopupLabel::showContextMenu( const QPoint & pos )
{
    QString text = contextText();

    if ( ! text.isEmpty() )
    {
	QMenu menu;
	menu.addAction( text );
	menu.exec( pos );
    }
}


void PopupLabel::mousePressEvent( QMouseEvent * event )
{
    if ( ! haveContextMenu() )
	return;

    if ( event->buttons() == Qt::LeftButton ||
	 event->buttons() == Qt::RightButton  )
    {
	showContextMenu( event->globalPos() );
    }
}


void PopupLabel::enterEvent( QEvent * event )
{
    Q_UNUSED( event );

    if ( haveContextMenu() )
    {
	QFont f = font();
	f.setUnderline( true );
	setFont( f );
    }
}


void PopupLabel::leaveEvent( QEvent * event )
{
    Q_UNUSED( event );

    QFont f = font();
    f.setUnderline( false );
    setFont( f );
}
