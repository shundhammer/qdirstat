/*
 *   File name:	DirSaver.cpp
 *   Summary:	Utility object to save current working directory
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <unistd.h>
#include "Logger.h"
#include "DirSaver.h"


DirSaver::KDirSaver( const QString & newPath )
{
    /*
     * No need to actually save the current working directory: This object
     * includes a QDir whose default constructor constructs a directory object
     * that contains the current working directory. Just what is needed here.
     */

    cd( newPath );
}


DirSaver::KDirSaver( const KURL & url )
{
    if ( url.isLocalFile() )
    {
	cd( url.path() );
    }
    else
    {
	logError() << k_funcinfo << "Can't change dir to remote location " << url.url() << endl;
    }
}


DirSaver::~KDirSaver()
{
    restore();
}


void
DirSaver::cd( const QString & newPath )
{
    if ( ! newPath.isEmpty() )
    {
	chdir( newPath );
    }
}


QString
DirSaver::currentDirPath() const
{
    return QDir::currentDirPath();
}


void
DirSaver::restore()
{
    chdir( oldWorkingDir.path() );
}


// EOF
