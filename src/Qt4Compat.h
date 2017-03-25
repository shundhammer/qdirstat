/*
 *   File name: Qt4Compat.h
 *   Summary:	Helpers for Qt4 compatibility
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef Qt4Compat_h
#define Qt4Compat_h


#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
#  define setSectionResizeMode setResizeMode
#  define sectionResizeMode    resizeMode
#endif


#endif // Qt4Compat_h
