/*
 *   File name: SettingsHelpers.h
 *   Summary:	Helper functions for QSettings for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef SettingsHelpers_h
#define SettingsHelpers_h

#include <QColor>
#include <QFont>
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

    /**
     * Read a font in string format  from the settings.
     * Example: "DejaVu Sans Mono,10,-1,5,50,0,0,0,0,0"
     **/
    QFont readFontEntry( const QSettings & settings,
                         const char	 * entryName,
                         const QFont     & fallback );

    /**
     * Write a font in string format to the settings.
     * Example: "DejaVu Sans Mono,10,-1,5,50,0,0,0,0,0"
     **/
    void writeFontEntry( QSettings   & settings,
                         const char  * entryName,
                         const QFont & font );


    /**
     * Read an enum value in string format from the settings.
     * 'enumMapping' maps each valid enum value to the corresponding string.
     **/
    int readEnumEntry( const QSettings & settings,
		       const char      * entryName,
		       int		 fallback,
		       const QMap<int, QString> & enumMapping );

    /**
     * Write an enum value in string format to the settings.
     * 'enumMapping' maps each valid enum value to the corresponding string.
     **/
    void writeEnumEntry( QSettings  & settings,
			 const char * entryName,
			 int	      enumValue,
			 const QMap<int, QString> & enumMapping );

    /**
     * Return the enum mapping for QRegExp::PatternSyntax.
     **/
    QMap<int, QString> patternSyntaxMapping();

    /**
     * Read window settings (size and position) from the settings and apply
     * them.
     **/
    void readWindowSettings( QWidget *       widget,
                             const QString & settingsGroup );

    /**
     * Write window settings (size and position) to the settings.
     **/
    void writeWindowSettings( QWidget *       widget,
                              const QString & settingsGroup );

}	// namespace QDirStat

#endif	// SettingsHelpers_h
