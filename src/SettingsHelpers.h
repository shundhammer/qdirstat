/*
 *   File name: SettingsHelpers.cpp
 *   Summary:	Helper functions for QSettings for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef SettingsHelpers_h
#define SettingsHelpers_h

#include <QColor>
#include <QList>

class QSettings;


namespace QDirStat
{
    /**
     * Read a color in RGB format (#RRGGBB) from the settings.
     **/
    QColor readColorEntry( const QSettings & settings,
			   const char	   * entryName,
			   const QColor	   & fallback );

    /**
     * Write a color in RGB format (#RRGGBB) to the settings.
     **/
    void writeColorEntry( QSettings    & settings,
			  const char   * entryName,
			  const QColor & color );

    /**
     * Read a list of colors in RGB format (#RRGGBB, #RRGGBB, ...) from the
     * settings.
     **/
    QList<QColor> readColorListEntry( const QSettings	  & settings,
				      const char	  * entryName,
				      const QList<QColor> & fallback );

    /**
     * Write a list of colors in RGB format (#RRGGBB, #RRGGBB, ...) to the
     * settings.
     **/
    void writeColorListEntry( QSettings		  & settings,
			      const char	  * entryName,
			      const QList<QColor> & colors );

}	// namespace QDirStat

#endif	// SettingsHelpers_h
