/*
 *   File name: Qt4Compat.h
 *   Summary:	Helpers for Qt4 compatibility
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef Qt4Compat_h
#define Qt4Compat_h

#include <QString>


#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
#  define setSectionResizeMode setResizeMode
#  define sectionResizeMode    resizeMode

inline QString qHtmlEscape( const QString & text )
{
    return Qt::escape( text );
}

#else // Qt 5.x

inline QString qHtmlEscape( const QString & text )
{
    return text.toHtmlEscaped();
}

#endif


#endif // Qt4Compat_h
