/*
 *   File name: PkgManager.cpp
 *   Summary:	Simple package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <unistd.h>	// access()
#include <QProcess>

#include "PkgManager.h"
#include "Logger.h"
#include "Exception.h"


#define COMMAND_TIMEOUT_SEC	5
#define CACHE_SIZE		500
#define CACHE_COST		1

using namespace QDirStat;


PkgQuery * PkgQuery::_instance = 0;


PkgQuery * PkgQuery::instance()
{
    if ( ! _instance )
    {
	_instance = new PkgQuery();
	CHECK_PTR( _instance );
    }

    return _instance;
}


PkgQuery::PkgQuery()
{
    _cache.setMaxCost( CACHE_SIZE );

    tryAdd( new DpkgPkgManager() );
    tryAdd( new RpmPkgManager()	 );
}


PkgQuery::~PkgQuery()
{
    qDeleteAll( _pkgManagers );
}


void PkgQuery::tryAdd( PkgManager * pkgManager )
{
    CHECK_PTR( pkgManager );

    if ( pkgManager->isActiveOnSystem() )
    {
	logInfo() << "Found package manager " << pkgManager->name() << endl;
	_pkgManagers << pkgManager;
    }
    else
    {
	delete pkgManager;
    }
}


QString PkgQuery::owningPkg( const QString & path )
{
    return instance()->getOwningPackage( path );
}


QString PkgQuery::getOwningPackage( const QString & path )
{
    QString pkg = "";
    QString foundBy;
    bool haveResult = false;

    if ( _cache.contains( path ) )
    {
	haveResult = true;
	foundBy	   = "Cache";
	pkg	   = *( _cache[ path ] );
    }


    if ( ! haveResult )
    {
	foreach ( PkgManager * pkgManager, _pkgManagers )
	{
	    pkg	    = pkgManager->owningPkg( path );

	    if ( ! pkg.isEmpty() )
	    {
		haveResult = true;
		foundBy	   = pkgManager->name();
		break;
	    }
	}

	if ( foundBy.isEmpty() )
	    foundBy = "all";

	// Insert package name (even if empty) into the cache
	_cache.insert( path, new QString( pkg ), CACHE_COST );
    }

    if ( pkg.isEmpty() )
	logDebug() << ( ! foundBy.isEmpty() ? ( foundBy + ": " ) : "" )
		   << "No package owns " << path << endl;
    else
	logDebug() << foundBy << ": Package " << pkg << " owns " << path << endl;

    return pkg;
}



bool PkgManager::tryRunCommand( const QString & commandLine,
				const QRegExp & expectedResult ) const
{
    int exitCode = -1;
    QString output = runCommand( commandLine, &exitCode );

    if ( exitCode != 0 )
    {
	logDebug() << "Exit code: " << exitCode << " command line: \"" << commandLine << "\"" << endl;
	return false;
    }

    bool expected = expectedResult.exactMatch( output );
    // logDebug() << "Expected: " << expected << endl;

    return expected;
}


QString PkgManager::runCommand( const QString & commandLine,
				int *		exitCode_ret ) const
{
    if ( exitCode_ret )
	*exitCode_ret = -1;

    QStringList args = commandLine.split( QRegExp( "\\s+" ) );

    if ( args.size() < 1 )
    {
	logError() << "Bad command line: \"" << commandLine << "\"" << endl;
	return "ERROR: Bad command line";
    }

    QString command = args.takeFirst();

    return runCommand( command, args, exitCode_ret );
}


QString PkgManager::runCommand( const QString &	    command,
				const QStringList & args,
				int *		    exitCode_ret ) const
{
    if ( exitCode_ret )
	*exitCode_ret = -1;

    if ( access( command.toUtf8(), X_OK ) != 0 )
    {
	logInfo() << "Command not found: \"" << command << "\"" << endl;
	return "ERROR: Command not found";
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert( "LANG", "C" ); // Prevent output in translated languages

    QProcess process;
    process.setProgram( command );
    process.setArguments( args );
    process.setProcessEnvironment( env );
    process.setProcessChannelMode( QProcess::MergedChannels ); // combine stdout and stderr
    logDebug() << "Running \"" << command << "\" args: " << args << endl;
    process.start();
    bool success = process.waitForFinished( COMMAND_TIMEOUT_SEC * 1000 );
    QString output = QString::fromUtf8( process.readAll() );

    if ( success )
    {
	if ( process.exitStatus() == QProcess::NormalExit )
	{
	    if ( exitCode_ret )
		*exitCode_ret = process.exitCode();
	}
	else
	{
	    logError() << "Command crashed: \"" << command << "\" args: " << args << endl;
	    output = "ERROR: Command crashed\n\n" + output;
	}
    }
    else
    {
	logError() << "Timeout or crash: \"" << command << "\" args: " << args << endl;
	output = "ERROR: Timeout or crash\n\n" + output;
    }

    // logDebug() << "Output: \n" << output << endl;

    return output;
}




bool DpkgPkgManager::isActiveOnSystem()
{
    return tryRunCommand( "/usr/bin/dpkg -S /usr/bin/dpkg", QRegExp( "^dpkg:.*" ) );
}


QString DpkgPkgManager::owningPkg( const QString & path )
{
    int exitCode = -1;
    QString output = runCommand( "/usr/bin/dpkg", QStringList() << "-S" << path, &exitCode );

    if ( exitCode != 0 || output.contains( "no path found matching pattern" ) )
	return "";

    QString pkg = output.section( ":", 0, 0 );
    // logDebug() << path << " is owned by pkg " << pkg << endl;

    return pkg;
}




bool RpmPkgManager::isActiveOnSystem()
{
    return tryRunCommand( "/usr/bin/rpm -qf /usr/bin/rpm", QRegExp( "^rpm.*" ) );
}


QString RpmPkgManager::owningPkg( const QString & path )
{
    int exitCode = -1;
    QString output = runCommand( "/usr/bin/rpm",
				 QStringList() << "-qf" << "--queryformat" << "%{NAME}" << path,
				 &exitCode );

    if ( exitCode != 0 || output.contains( "not owned by any package" ) )
	return "";

    QString pkg = output;
    // logDebug() << path << " is owned by pkg " << pkg << endl;

    return pkg;
}
