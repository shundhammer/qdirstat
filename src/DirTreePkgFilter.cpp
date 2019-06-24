/*
 *   File name: DirTreePkgFilter.cpp
 *   Summary:	Package manager support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DirTreePkgFilter.h"
#include "PkgManager.h"
#include "PkgFileListCache.h"
#include "Exception.h"
#include "Logger.h"


using namespace QDirStat;


DirTreePkgFilter::DirTreePkgFilter( PkgManager * pkgManager )
{
    CHECK_PTR( pkgManager );

    logInfo() << "Creating file list cache for " << pkgManager->name() << endl;
    _fileListCache = pkgManager->createFileListCache( PkgFileListCache::LookupGlobal );
    logInfo() << "Done." << endl;
}


DirTreePkgFilter::~DirTreePkgFilter()
{
    delete _fileListCache;
}


bool DirTreePkgFilter::ignore( const QString & path ) const
{
    if ( ! _fileListCache )
	return false;

    return _fileListCache->containsFile( path );
}
