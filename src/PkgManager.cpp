/*
 *   File name: PkgManager.cpp
 *   Summary:	Package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "PkgManager.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


PkgManager::PkgManager()
{

}


PkgManager::~PkgManager()
{
    // NOP
}


QStringList PkgManager::fileList( PkgInfo * pkg )
{
    QStringList fileList;
    QString     command = fileListCommand( pkg );

    if ( ! command.isEmpty() )
    {
        int exitCode = -1;
        QString output = runCommand( command,
                                     &exitCode,
                                     true,          // logCommand
                                     false );       // logOutput
        if ( exitCode == 0 )
            fileList = parseFileList( output );
    }

    return fileList;
}

