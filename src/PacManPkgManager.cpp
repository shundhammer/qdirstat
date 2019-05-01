/*
 *   File name: PacManPkgManager.cpp
 *   Summary:	Simple package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "PacManPkgManager.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


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
    QString output = runCommand( "/usr/bin/pacman",
                                 QStringList() << "-Qo" << path,
                                 &exitCode );

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


PkgInfoList PacManPkgManager::installedPkg()
{
    int exitCode = -1;
    QString output = runCommand( "/usr/bin/pacman",
                                 QStringList() << "-Qn",
                                 &exitCode );

    PkgInfoList pkgList;

    if ( exitCode == 0 )
        pkgList = parsePkgList( output );

    return pkgList;
}


PkgInfoList PacManPkgManager::parsePkgList( const QString & output )
{
    PkgInfoList pkgList;

    foreach ( const QString & line, output.split( "\n" ) )
    {
        if ( ! line.isEmpty() )
        {
            QStringList fields = line.split( " ", QString::KeepEmptyParts );

            if ( fields.size() != 2 )
                logError() << "Invalid pacman -Qn output: \"" << line << "\n" << endl;
            else
            {
                QString name    = fields.takeFirst();
                QString version = fields.takeFirst();
                QString arch    = "";

                PkgInfo * pkg = new PkgInfo( name, version, arch, this );
                CHECK_NEW( pkg );

                pkgList << pkg;
            }
        }
    }

    return pkgList;
}


QString PacManPkgManager::fileListCommand( PkgInfo * pkg )
{
    return QString( "/usr/bin/pacman -Qlq %1" ).arg( pkg->baseName() );
}


QStringList PacManPkgManager::parseFileList( const QString & output )
{
    return output.split( "\n" );
}

