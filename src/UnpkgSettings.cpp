/*
 *   File name: UnpkgSettings.cpp
 *   Summary:	Parameters for "unpackaged files" view
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "UnpkgSettings.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"


using namespace QDirStat;


UnpkgSettings::UnpkgSettings( InitPolicy initPolicy )
{
    switch ( initPolicy )
    {
	case ReadFromConfig:
	    read();
	    break;

	case DefaultValues:
	    startingDir	   = defaultStartingDir();
	    excludeDirs	   = defaultExcludeDirs();
	    ignorePatterns = defaultIgnorePatterns();
	    break;

	case Empty:
	    break;
    }
}


void UnpkgSettings::read()
{
    // logDebug() << endl;

    QDirStat::Settings settings;

    settings.beginGroup( "UnpkgSettings" );

    startingDir	   = settings.value( "StartingDir",    defaultStartingDir()    ).toString();
    excludeDirs	   = settings.value( "ExcludeDirs",    defaultExcludeDirs()    ).toStringList();
    ignorePatterns = settings.value( "IgnorePatterns", defaultIgnorePatterns() ).toStringList();

    settings.endGroup();
}


void UnpkgSettings::write()
{
    // logDebug() << endl;

    QDirStat::Settings settings;

    settings.beginGroup( "UnpkgSettings" );

    settings.setValue( "StartingDir",	 startingDir	);
    settings.setValue( "ExcludeDirs",	 excludeDirs	);
    settings.setValue( "IgnorePatterns", ignorePatterns );

    settings.endGroup();
}


void UnpkgSettings::dump() const
{
    logDebug() << "startingDir:    " << startingDir << endl;
    logDebug() << "excludeDirs:    " << excludeDirs << endl;
    logDebug() << "ignorePatterns: " << ignorePatterns << endl;
}


QString UnpkgSettings::defaultStartingDir()
{
    return "/";
}


QStringList UnpkgSettings::defaultExcludeDirs()
{
    return QStringList()
	<< "/home"
	<< "/root"
	<< "/tmp"
	<< "/var"
        << "/snap"
	<< "/usr/lib/sysimage/rpm"
	<< "/usr/local";
}


QStringList UnpkgSettings::defaultIgnorePatterns()
{
    return QStringList() << "*.pyc";
}

