/*
 *   File name: FileSizeLabel.cpp
 *   Summary:	Specialized QLabel for a file size for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMenu>
#include <QMouseEvent>

#include "FileSizeLabel.h"
#include "Logger.h"

using namespace QDirStat;


FileSizeLabel::FileSizeLabel( QWidget * parent ):
    QLabel( parent ),
    _value( -1 )
{

}


FileSizeLabel::~FileSizeLabel()
{
    // NOP
}


void FileSizeLabel::clear()
{
    _value = -1;
    _prefix.clear();
    _contextText.clear();
    setBold( false );
    QLabel::clear();
}


void FileSizeLabel::setValue( FileSize val, const QString & prefix )
{
    _value  = val;
    _prefix = prefix;
    _contextText.clear();

    if ( _value < 0 )
        QLabel::setText( "" );
    else
        QLabel::setText( _prefix + formatSize( _value ) );
}


void FileSizeLabel::setText( const QString & newText,
                             FileSize        newValue,
                             const QString & newPrefix )
{
    _value  = newValue;
    _prefix = newPrefix;
    _contextText.clear();

    QLabel::setText( newText );
}


void FileSizeLabel::setBold( bool bold )
{
    QFont textFont = font();
    textFont.setBold( bold );
    setFont( textFont );
}


bool FileSizeLabel::haveContextMenu() const
{
    if ( ! _contextText.isEmpty() )
        return true;

    return _value >= 1024; // Doesn't make sense below 1 kB
}


void FileSizeLabel::mousePressEvent( QMouseEvent * event )
{
    if ( ! haveContextMenu() )
        return;

    if ( event->buttons() == Qt::LeftButton ||
         event->buttons() == Qt::RightButton  )
    {
        QString text = _contextText.isEmpty() ?
            _prefix + formatByteSize( _value ) : _contextText;

        QMenu menu;
        menu.addAction( text );
        menu.exec( event->globalPos() );
    }
}


void FileSizeLabel::enterEvent( QEvent * event )
{
    Q_UNUSED( event );

    if ( haveContextMenu() )
    {
        QFont f = font();
        f.setUnderline( true );
        setFont( f );
    }
}


void FileSizeLabel::leaveEvent( QEvent * event )
{
    Q_UNUSED( event );

    QFont f = font();
    f.setUnderline( false );
    setFont( f );
}
