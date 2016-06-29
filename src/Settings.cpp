/*
 *   File name: Settings.cpp
 *   Summary:	Specialized settings classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "Settings.h"
#include "SettingsHelpers.h"

#include <QCoreApplication>


using namespace QDirStat;


Settings::Settings( const QString & name ):
    QSettings( QCoreApplication::organizationName(),
	       name.isEmpty()? QCoreApplication::applicationName() : name ),
    _name( name )
{

}


Settings::~Settings()
{

}


void Settings::ensureToplevel()
{
    while ( ! group().isEmpty() )	// ensure using toplevel settings
	endGroup();
}


QStringList Settings::findGroups( const QString & groupPrefix )
{
    QStringList result;;
    ensureToplevel();

    foreach ( const QString & group, childGroups() )
    {
	if ( group.startsWith( groupPrefix ) )
	    result << group;
    }

    return result;
}


bool Settings::hasGroup( const QString & groupPrefix )
{
    ensureToplevel();

    foreach ( const QString & group, childGroups() )
    {
	if ( group.startsWith( groupPrefix ) )
	    return true;
    }

    return false;
}


void Settings::removeGroups( const QString & groupPrefix )
{
    ensureToplevel();

    foreach ( const QString & group, childGroups() )
    {
	if ( group.startsWith( groupPrefix ) )
	    remove( group );
    }
}


void Settings::migrate( const QString & groupPrefix,
			Settings & from,
			Settings & to )
{
    Q_UNUSED( groupPrefix );
    Q_UNUSED( from );
    Q_UNUSED( to );
}




CleanupSettings::CleanupSettings():
    Settings( QCoreApplication::applicationName() + "-cleanup" )
{
    _groupPrefix = "Cleanup_";
}


CleanupSettings::~CleanupSettings()
{

}


void CleanupSettings::migrate()
{

}




MimeCategorySettings::MimeCategorySettings():
    Settings( QCoreApplication::applicationName() + "-mime" )
{
    _groupPrefix = "MimeCategory_";
}


MimeCategorySettings::~MimeCategorySettings()
{

}


void MimeCategorySettings::migrate()
{

}
