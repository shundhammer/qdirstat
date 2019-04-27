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
