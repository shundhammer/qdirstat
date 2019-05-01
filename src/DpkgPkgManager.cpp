/*
 *   File name: PkgManager.cpp
 *   Summary:	Dpkg package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DpkgPkgManager.h"
#include "Logger.h"
#include "Exception.h"

#define LOG_COMMANDS	true
#define LOG_OUTPUT	false
#include "SysUtil.h"


using namespace QDirStat;

using SysUtil::runCommand;
using SysUtil::tryRunCommand;
using SysUtil::haveCommand;


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


PkgInfoList DpkgPkgManager::installedPkg()
{
    int exitCode = -1;
    QString output = runCommand( "/usr/bin/dpkg-query",
                                 QStringList()
                                 << "--show"
                                 << "--showformat=${Package} | ${Version} | ${Architecture} | ${Status}\n",
                                 &exitCode );

    PkgInfoList pkgList;

    if ( exitCode == 0 )
        pkgList = parsePkgList( output );

    return pkgList;
}


PkgInfoList DpkgPkgManager::parsePkgList( const QString & output )
{
    PkgInfoList pkgList;

    foreach ( const QString & line, output.split( "\n" ) )
    {
        if ( ! line.isEmpty() )
        {
            QStringList fields = line.split( " | ", QString::KeepEmptyParts );

            if ( fields.size() != 4 )
                logError() << "Invalid dpkg-query output: \"" << line << "\n" << endl;
            else
            {
                QString name    = fields.takeFirst();
                QString version = fields.takeFirst();
                QString arch    = fields.takeFirst();
                QString status  = fields.takeFirst();

                if ( status == "install ok installed" )
                {
                    PkgInfo * pkg = new PkgInfo( name, version, arch, this );
                    CHECK_NEW( pkg );

                    pkgList << pkg;
                }
                else
                {
                    // logDebug() << "Ignoring " << line << endl;
                }
            }
        }
    }

    return pkgList;
}


QString DpkgPkgManager::fileListCommand( PkgInfo * pkg )
{
    return QString( "/usr/bin/dpkg-query --listfiles %1" ).arg( queryName( pkg ) );
}


QStringList DpkgPkgManager::parseFileList( const QString & output )
{
    QStringList fileList = output.split( "\n" );
    fileList.removeAll( "/." );     // Remove cruft

    return fileList;
}


QString DpkgPkgManager::queryName( PkgInfo * pkg )
{
    CHECK_PTR( pkg );

    QString name = pkg->baseName();

    if ( pkg->isMultiVersion() )
        name += "_" + pkg->version();

    if ( pkg->isMultiArch() )
        name += ":" + pkg->arch();

    return name;
}
