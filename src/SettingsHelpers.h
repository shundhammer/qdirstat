/*
 *   File name: SettingsHelpers.cpp
 *   Summary:	Helper functions for QSettings for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef SettingsHelpers_h
#define SettingsHelpers_h


namespace QDirStat
{
    /**
     * Read a color in RGB format (#RRGGBB) from the settings.
     **/
    QColor readColorEntry( const QSettings & settings,
			   const char	   * entryName,
			   const QColor	   & defaultColor );

    /**
     * Write a color in RGB format (#RRGGBB) to the settings.
     **/
    void writeColorEntry( QSettings    & settings,
			  const char   * entryName,
			  const QColor & color );


}	// namespace QDirStat

#endif	// SettingsHelpers_h
