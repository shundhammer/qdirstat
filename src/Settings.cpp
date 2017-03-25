/*
 *   File name: Settings.cpp
 *   Summary:	Specialized settings classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QCoreApplication>

#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"

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


void Settings::moveGroups( const QString & groupPrefix,
			   Settings * from,
			   Settings * to )
{
    CHECK_PTR( from );
    CHECK_PTR( to   );


    if ( ! hasGroup( groupPrefix ) )
    {
	logInfo() << "Migrating " << groupPrefix << "* to " << to->name() << endl;
	QStringList groups = from->findGroups( groupPrefix );

	foreach ( const QString & group, groups )
	{
	    // logVerbose() << "  Migrating " << group << endl;

	    from->beginGroup( group );
	    to->beginGroup( group );

	    QStringList keys = from->allKeys();

	    foreach( const QString & key, keys )
	    {
		// logVerbose() << "	Copying " << key << endl;
		to->setValue( key, from->value( key ) );
	    }

	    to->endGroup();
	    from->endGroup();
	}
    }
    else
    {
#if 0
	logVerbose() << "Target settings " << to->name()
		     << " have group " << groupPrefix
		     << " - nothing to migrate"
		     << endl;
#endif
    }

    from->removeGroups( groupPrefix );
}




CleanupSettings::CleanupSettings():
    Settings( QCoreApplication::applicationName() + "-cleanup" )
{
    _groupPrefix = "Cleanup_";
    migrate();
}


CleanupSettings::~CleanupSettings()
{

}


void CleanupSettings::migrate()
{
    Settings commonSettings;
    moveGroups( _groupPrefix, &commonSettings, this );
}




MimeCategorySettings::MimeCategorySettings():
    Settings( QCoreApplication::applicationName() + "-mime" )
{
    _groupPrefix = "MimeCategory_";
    migrate();
}


MimeCategorySettings::~MimeCategorySettings()
{

}


void MimeCategorySettings::migrate()
{
    Settings commonSettings;
    moveGroups( _groupPrefix, &commonSettings, this );
}




ExcludeRuleSettings::ExcludeRuleSettings():
    Settings( QCoreApplication::applicationName() + "-exclude" )
{
    _groupPrefix = "ExcludeRule_";
    migrate();
}


ExcludeRuleSettings::~ExcludeRuleSettings()
{

}


void ExcludeRuleSettings::migrate()
{
    Settings commonSettings;
    moveGroups( _groupPrefix, &commonSettings, this );
}
