/*
 *   File name: SettingsHelpers.cpp
 *   Summary:	Helper functions for QSettings for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QSettings>
#include <QColor>

#include "SettingsHelpers.h"
#include "Exception.h"
#include "Logger.h"


namespace QDirStat
{
    QColor readColorEntry( const QSettings & settings,
			   const char	   * entryName,
			   const QColor	   & defaultColor )
    {
	QString colorName = settings.value( entryName ).toString();
	QColor color( colorName );

	if ( ! color.isValid() )
	{
	    color = defaultColor;

	    logDebug() << "Using fallback for " << entryName
		       << ": " << color.name() << endl;
	}

	return color;
    }


    void writeColorEntry( QSettings    & settings,
			  const char   * entryName,
			  const QColor & color )
    {
	settings.setValue( entryName, color.name() );
    }

} // namespace QDirStat

