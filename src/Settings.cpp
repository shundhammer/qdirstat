/*
 *   File name: Settings.cpp
 *   Summary:	Specialized settings classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


// Refusing to use any of those hare-brained incompatible strerror()
// replacements.  Seriously, what are they thinking when they come up with two
// different functions by the same name that behave differently? SORT OUT YOUR
// STUFF before you become a general PITA to the working software developers!
#define DONT_DEPRECATE_STRERROR
#include <unistd.h> // chown()
#include <errno.h>

#include <QCoreApplication>

#include "Settings.h"
#include "SettingsHelpers.h"
#include "SysUtil.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


QSet<QString> Settings::_usedConfigFiles;


Settings::Settings( const QString & name ):
    QSettings( QCoreApplication::organizationName(),
	       name.isEmpty()? QCoreApplication::applicationName() : name ),
    _name( name )
{
    _usedConfigFiles << fileName();
}


Settings::~Settings()
{

}


void Settings::beginGroup( const QString & prefix, int no )
{
    _groupPrefix = prefix;

    QString groupName = QString( "%1_%2" )
	.arg( prefix )
	.arg( no,
	      2,		// fieldWidth
	      10,		// base
	      QChar( '0' ) );	// fillChar

    QSettings::beginGroup( groupName );
}


void Settings::fixFileOwners()
{
    if ( SysUtil::runningWithSudo() )
    {
        foreach ( const QString & filename, _usedConfigFiles )
            fixFileOwner( filename );
    }
}


void Settings::setDefaultValue( const QString & key, bool newValue )
{
    if ( ! contains( key ) )
        setValue( key, newValue );
}


void Settings::setDefaultValue( const QString & key, int newValue )
{
    if ( ! contains( key ) )
        setValue( key, newValue );
}


void Settings::setDefaultValue( const QString & key, const QString & newValue )
{
    if ( ! contains( key ) )
        setValue( key, newValue );
}


void Settings::fixFileOwner( const QString & filename )
{
    QString sudoUid = QString::fromUtf8( qgetenv( "SUDO_UID" ) );
    QString sudoGid = QString::fromUtf8( qgetenv( "SUDO_GID" ) );

    if ( ! sudoUid.isEmpty() && ! sudoGid.isEmpty() )
    {
        uid_t   uid     = sudoUid.toInt();
        gid_t   gid     = sudoGid.toInt();
        QString homeDir = SysUtil::homeDir( uid );

        if ( homeDir.isEmpty() )
        {
            logWarning() << "Can't get home directory for UID " << uid << endl;
            return;
        }

        if ( filename.startsWith( homeDir ) )
        {
            int result = ::chown( filename.toUtf8(), uid, gid );

            if ( result != 0 )
            {
                logError() << "Can't chown " << filename
                           << " to UID "  << uid
                           << " and GID " << gid
                           << ": " << strerror( errno )
                           << endl;
            }
            else
            {
#if 1
                logDebug() << "Success: chown " << filename
                           << " to UID "  << uid
                           << " and GID " << gid
                           << endl;
#endif
            }
        }
        else
        {
            // logInfo() << "Not touching " << filename << endl;
        }
    }
    else
    {
        logWarning() << "$SUDO_UID / $SUDO_GID not set" << endl;
    }
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
#if 0
	logInfo() << "Migrating " << groupPrefix << "* to " << to->name() << endl;
#endif
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
