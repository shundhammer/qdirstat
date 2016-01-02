/*
 *   File name: SettingsHelpers.cpp
 *   Summary:	Helper functions for QSettings for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QSettings>
#include <QColor>
#include <QRegExp>

#include "SettingsHelpers.h"
#include "Exception.h"
#include "Logger.h"


namespace QDirStat
{
    QColor readColorEntry( const QSettings & settings,
			   const char	   * entryName,
			   const QColor	   & fallback )
    {
	QString colorName = settings.value( entryName ).toString();
	QColor color( colorName );

	if ( ! color.isValid() )
	{
	    color = fallback;
#if 0
	    logDebug() << "Using fallback for " << entryName
		       << ": " << color.name() << endl;
#endif
	}

	return color;
    }


    void writeColorEntry( QSettings    & settings,
			  const char   * entryName,
			  const QColor & color )
    {
	settings.setValue( entryName, color.name() );
    }


    QList<QColor> readColorListEntry( const QSettings	  & settings,
				      const char	  * entryName,
				      const QList<QColor> & fallback )
    {
	QStringList strList = settings.value( entryName ).toStringList();
	QList<QColor> colorList;

	colorList.clear();

	foreach ( const QString & rgb, strList )
	{
	    QColor color( rgb );

	    if ( color.isValid() )
		colorList << color;
	    else
	    {
		logError() << "ERROR in " << entryName << ": \""
			   << rgb << "\" is not a valid color" << endl;
	    }
	}

	if ( colorList.isEmpty() )
	    colorList = fallback;

	return colorList;
    }


    void writeColorListEntry( QSettings		  & settings,
			      const char	  * entryName,
			      const QList<QColor> & colors )
    {
	QStringList strList;

	foreach ( const QColor & color, colors )
	{
	    strList << color.name();
	}

	settings.setValue( entryName, strList );
    }


    int readEnumEntry( const QSettings & settings,
		       const char      * entryName,
		       int		 fallback,
		       const QMap<int, QString> & enumMapping )
    {
	if ( ! settings.contains( entryName ) )
	    return fallback;

	QString str = settings.value( entryName ).toString();
	QMap<int, QString>::const_iterator it = enumMapping.constBegin();

	while ( it != enumMapping.constEnd() )
	{
	    if ( it.value() == str )
		return it.key();

	    ++it;
	}

	logError() << "Invalid value for " << entryName
		   << ": \"" << str << "\"" << endl;

	return fallback;
    }


    void writeEnumEntry( QSettings  & settings,
			 const char * entryName,
			 int	      enumValue,
			 const QMap<int, QString> & enumMapping )
    {
	if ( ! enumMapping.contains( enumValue ) )
	{
	    logError() << "No string for enum value " << enumValue << endl;
	    return;
	}

	settings.setValue( entryName, enumMapping.value( enumValue ) );
    }


    QMap<int, QString> patternSyntaxMapping()
    {
	static QMap<int, QString> mapping;

	if ( mapping.isEmpty() )
	{
	    mapping[ QRegExp::RegExp	     ] = "RegExp";
	    mapping[ QRegExp::Wildcard	     ] = "Wildcard";
	    mapping[ QRegExp::FixedString    ] = "FixedString";
	    mapping[ QRegExp::RegExp2	     ] = "RegExp2";
	    mapping[ QRegExp::WildcardUnix   ] = "WildcardUnix";
	    mapping[ QRegExp::W3CXmlSchema11 ] = "W3CXmlSchema11";
	}

	return mapping;
    }


} // namespace QDirStat

