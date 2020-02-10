/*
 *   File name: FileSizeLabel.cpp
 *   Summary:	Specialized QLabel for a file size for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMenu>
#include <QContextMenuEvent>

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


void FileSizeLabel::setValue( FileSize val )
{
    _value = val;

    if ( _value < 0 )
        QLabel::setText( "" );
    else
        QLabel::setText( formatSize( _value ) );
}


void FileSizeLabel::setText( const QString & newText )
{
    _value = -1;
    QLabel::setText( newText );
}


void FileSizeLabel::contextMenuEvent( QContextMenuEvent * event )
{
    if ( _value < 0 )
        return;
    
    QMenu menu;
    menu.addAction( formatByteSize( _value ) );
    menu.exec( event->globalPos() );
}

