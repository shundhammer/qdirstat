/*
 *   File name: PkgManager.cpp
 *   Summary:	Simple package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "PkgQuery.h"
#include "PkgManager.h"
#include "DpkgPkgManager.h"
#include "RpmPkgManager.h"
#include "PacManPkgManager.h"
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
    checkPkgManagers();
}


PkgQuery::~PkgQuery()
{
    qDeleteAll( _pkgManagers );
}


void PkgQuery::checkPkgManagers()
{
    logInfo() << "Checking available supported package managers..." << endl;

    checkPkgManager( new DpkgPkgManager()   );
    checkPkgManager( new RpmPkgManager()    );
    checkPkgManager( new PacManPkgManager() );

    _pkgManagers += _secondaryPkgManagers;
    _secondaryPkgManagers.clear();

    if ( _pkgManagers.isEmpty() )
        logInfo() << "No supported package manager found." << endl;
    else
    {
        QStringList available;

        foreach ( PkgManager * pkgManager, _pkgManagers )
            available << pkgManager->name();

        logInfo() << "Found " << available.join( ", " )  << endl;
    }
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


bool PkgQuery::foundSupportedPkgManager()
{
    return ! instance()->_pkgManagers.isEmpty();
}


PkgManager * PkgQuery::primaryPkgManager()
{
    return instance()->_pkgManagers.isEmpty() ?
        0 : instance()->_pkgManagers.first();
}


QString PkgQuery::owningPkg( const QString & path )
{
    return instance()->getOwningPackage( path );
}


PkgInfoList PkgQuery::installedPkg()
{
    return instance()->getInstalledPkg();
}


QStringList PkgQuery::fileList( PkgInfo * pkg )
{
    return instance()->getFileList( pkg );
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


PkgInfoList PkgQuery::getInstalledPkg()
{
    PkgInfoList pkgList;

    foreach ( PkgManager * pkgManager, _pkgManagers )
    {
        pkgList.append( pkgManager->installedPkg() );
    }

    return pkgList;
}


QStringList PkgQuery::getFileList( PkgInfo * pkg )
{
    foreach ( PkgManager * pkgManager, _pkgManagers )
    {
        QStringList fileList = pkgManager->fileList( pkg );

        if ( ! fileList.isEmpty() )
            return fileList;
    }

    return QStringList();
}


bool PkgQuery::haveGetInstalledPkgSupport()
{
    return instance()->checkGetInstalledPkgSupport();
}


bool PkgQuery::haveFileListSupport()
{
    return instance()->checkFileListSupport();
}


bool PkgQuery::checkGetInstalledPkgSupport()
{
    foreach ( PkgManager * pkgManager, _pkgManagers )
    {
        if ( pkgManager->supportsGetInstalledPkg() )
            return true;
    }

    return false;
}


bool PkgQuery::checkFileListSupport()
{
    foreach ( PkgManager * pkgManager, _pkgManagers )
    {
        if ( pkgManager->supportsFileList() )
            return true;
    }

    return false;
}
