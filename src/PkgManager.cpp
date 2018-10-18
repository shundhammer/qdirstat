/*
 *   File name: PkgManager.cpp
 *   Summary:	Simple package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "PkgManager.h"
#include "Logger.h"
#include "Exception.h"

#define LOG_COMMANDS	true
#define LOG_OUTPUT	false
#include "SysUtil.h"


#define CACHE_SIZE		500
#define CACHE_COST		1

#define VERBOSE_PKG_QUERY	1


using namespace QDirStat;

using SysUtil::runCommand;
using SysUtil::tryRunCommand;
using SysUtil::haveCommand;


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

    checkPkgManager( new DpkgPkgManager()   );
    checkPkgManager( new RpmPkgManager()    );
    checkPkgManager( new PacManPkgManager() );

    _pkgManagers += _secondaryPkgManagers;
    _secondaryPkgManagers.clear();
}


PkgQuery::~PkgQuery()
{
    qDeleteAll( _pkgManagers );
}


void PkgQuery::checkPkgManager( PkgManager * pkgManager )
{
    CHECK_PTR( pkgManager );

    if ( pkgManager->isPrimaryPkgManager() )
    {
	logInfo() << "Found primary package manager " << pkgManager->name() << endl;
	_pkgManagers << pkgManager;
    }
    else if ( pkgManager->isAvailable() )
    {
	logInfo() << "Found secondary package manager " << pkgManager->name() << endl;
	_secondaryPkgManagers << pkgManager;
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
	    pkg = pkgManager->owningPkg( path );

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

#if VERBOSE_PKG_QUERY
    if ( pkg.isEmpty() )
	logDebug() << foundBy << ": No package owns " << path << endl;
    else
	logDebug() << foundBy << ": Package " << pkg << " owns " << path << endl;
#endif

    return pkg;
}




bool DpkgPkgManager::isPrimaryPkgManager()
{
    return tryRunCommand( "/usr/bin/dpkg -S /usr/bin/dpkg", QRegExp( "^dpkg:.*" ) );
}


bool DpkgPkgManager::isAvailable()
{
    return haveCommand( "/usr/bin/dpkg" );
}


QString DpkgPkgManager::owningPkg( const QString & path )
{
    int exitCode = -1;
    QString output = runCommand( "/usr/bin/dpkg", QStringList() << "-S" << path, &exitCode );

    if ( exitCode != 0 || output.contains( "no path found matching pattern" ) )
	return "";

    QString pkg = output.section( ":", 0, 0 );

    return pkg;
}




RpmPkgManager::RpmPkgManager()
{
    if ( haveCommand( "/usr/bin/rpm" ) )
	_rpmCommand = "/usr/bin/rpm";
    else
	_rpmCommand = "/bin/rpm"; // for old SUSE / Red Hat distros

    // Notice that it is not enough to rely on a symlink /bin/rpm ->
    // /usr/bin/rpm: While recent SUSE distros have that symlink (and maybe Red
    // Hat and Fedora as well?), rpm as a secondary package manager on Ubuntu
    // does not have such a link; they only have /usr/bin/rpm.
    //
    // Also intentionally never leaving _rpmCommand empty if it is not
    // available to avoid unpleasant surprises if a caller tries to use any
    // other method of this class that refers to it.
}


bool RpmPkgManager::isPrimaryPkgManager()
{
    return tryRunCommand( QString( "%1 -qf %1" ).arg( _rpmCommand ),
			  QRegExp( "^rpm.*" ) );
}


bool RpmPkgManager::isAvailable()
{
    return haveCommand( _rpmCommand );
}


QString RpmPkgManager::owningPkg( const QString & path )
{
    int exitCode = -1;
    QString output = runCommand( _rpmCommand,
				 QStringList() << "-qf" << "--queryformat" << "%{NAME}" << path,
				 &exitCode );

    if ( exitCode != 0 || output.contains( "not owned by any package" ) )
	return "";

    QString pkg = output;

    return pkg;
}



bool PacManPkgManager::isPrimaryPkgManager()
{
    return tryRunCommand( "/usr/bin/pacman -Qo /usr/bin/pacman",
                          QRegExp( ".*is owned by pacman.*" ) );
}


bool PacManPkgManager::isAvailable()
{
    return haveCommand( "/usr/bin/pacman" );
}


QString PacManPkgManager::owningPkg( const QString & path )
{
    int exitCode = -1;
    QString output = runCommand( "/usr/bin/pacman", QStringList() << "-Qo" << path, &exitCode );

    if ( exitCode != 0 || output.contains( "No package owns" ) )
	return "";

    // Sample output:
    //
    //   /usr/bin/pacman is owned by pacman 5.1.1-3
    //
    // The path might contain blanks, so it might not be safe to just use
    // blank-separated section #4; let's remove the part before the package
    // name.

    output.remove( QRegExp( "^.*is owned by " ) );
    QString pkg = output.section( " ", 0, 0 );

    return pkg;
}
