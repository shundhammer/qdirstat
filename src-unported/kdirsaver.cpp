/*
 *   File name:	kdirsaver.cpp
 *   Summary:	Utility object to save current working directory
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <unistd.h>
#include <kdebug.h>
#include "kdirsaver.h"


KDirSaver::KDirSaver( const QString & newPath )
{
    /*
     * No need to actually save the current working directory: This object
     * includes a QDir whose default constructor constructs a directory object
     * that contains the current working directory. Just what is needed here.
     */

    cd( newPath );
}


KDirSaver::KDirSaver( const KURL & url )
{
    if ( url.isLocalFile() )
    {
	cd( url.path() );
    }
    else
    {
	kdError() << k_funcinfo << "Can't change dir to remote location " << url.url() << endl;
    }
}


KDirSaver::~KDirSaver()
{
    restore();
}


void
KDirSaver::cd( const QString & newPath )
{
    if ( ! newPath.isEmpty() )
    {
	chdir( newPath );
    }
}


QString
KDirSaver::currentDirPath() const
{
    return QDir::currentDirPath();
}


void
KDirSaver::restore()
{
    chdir( oldWorkingDir.path() );
}


// EOF
