/*
 *   File name:	DirSaver.cpp
 *   Summary:	Utility object to save current working directory
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <unistd.h>
#include "Logger.h"
#include "DirSaver.h"


DirSaver::DirSaver( const QString & newPath )
{
    _oldWorkingDir = QDir::currentPath();
    cd( newPath );
}


DirSaver::~DirSaver()
{
    restore();
}


void DirSaver::cd( const QString & newPath )
{
    if ( newPath.isEmpty() )
    {
	logWarning() << "Empty path" << Qt::endl;
	return;
    }

    // logDebug() << "cd " << newPath << Qt::endl;
    int result = chdir( newPath.toUtf8() );

    if ( result != 0 )
        logWarning() << "chdir( " << newPath << " ) failed" << Qt::endl;
}


void DirSaver::restore()
{
    cd( _oldWorkingDir );
}

