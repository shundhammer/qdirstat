/*
 *   File name: PkgFileListCache.cpp
 *   Summary:	Package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "PkgFileListCache.h"
#include "Logger.h"


using namespace QDirStat;


PkgFileListCache::PkgFileListCache( PkgManager * pkgManager ):
    _pkgManager( pkgManager )
{

}


PkgFileListCache::~PkgFileListCache()
{
    logDebug() << "Destroying the file list cache." << endl;
}


QStringList PkgFileListCache::fileList( const QString & pkgName )
{
    QStringList fileList( _fileNames.values( pkgName ) );
    fileList.sort();

    return fileList;
}


bool PkgFileListCache::containsPkg( const QString & pkgName ) const
{
    return _fileNames.contains( pkgName );
}


void PkgFileListCache::remove( const QString & pkgName )
{
    _fileNames.remove( pkgName );
}


void PkgFileListCache::clear()
{
    _fileNames.clear();
}


void PkgFileListCache::add( const QString & pkgName,
                            const QString & fileName )
{
    _fileNames.insert( pkgName, fileName );
}
