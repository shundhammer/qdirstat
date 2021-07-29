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
#include "FormatUtil.h"
#include "Logger.h"

using namespace QDirStat;


FileSizeLabel::FileSizeLabel( QWidget * parent ):
    PopupLabel( parent ),
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
    PopupLabel::clear();
}


void FileSizeLabel::setValue( FileSize val, const QString & prefix )
{
    _value  = val;
    _prefix = prefix;
    setContextText( "" );

    if ( _value < 0 )
	QLabel::setText( "" );
    else
	QLabel::setText( _prefix + formatSize( _value ) );
}


void FileSizeLabel::setText( const QString & newText,
			     FileSize	     newValue,
			     const QString & newPrefix )
{
    _value  = newValue;
    _prefix = newPrefix;
    setContextText( "" );

    QLabel::setText( newText );
}


bool FileSizeLabel::haveContextMenu() const
{
    if ( ! _contextText.isEmpty() )
	return true;

    return _value >= 1024; // Doesn't make sense below 1 kB
}


QString FileSizeLabel::contextText() const
{
    return _contextText.isEmpty() ?
	_prefix + formatByteSize( _value ) : _contextText;
}
