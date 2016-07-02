/*
 *   File name: Exception.cpp
 *   Summary:	Exception classes for QDirstat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <sys/errno.h>
#include <string.h>	// strerror()

#include "Exception.h"


void Exception::setSrcLocation( const QString &srcFile,
				int	       srcLine,
				const QString &srcFunction ) const
{
    // This is why those member variables are 'mutable':
    // We need to be able to set the source location from RETHROW even after
    // the exception was caught as const reference.
    //
    // This is not 100% elegant, but it keeps in line with usual conventions -
    // conventions like "catch exception objects as const reference".

    _srcFile	 = srcFile;
    _srcLine	 = srcLine;
    _srcFunction = srcFunction;
}


QString SysCallFailedException::errMsg( const QString & sysCall,
					const QString & resourceName ) const
{
    QString msg;

    if ( errno != 0 )
    {
	msg = QObject::tr( "%1( %2 ) failed: %3" )
	    .arg( sysCall )
	    .arg( resourceName )
	    .arg( strerror( errno ) );
    }
    else
    {
	msg = QObject::tr( "%1( %2 ) failed" )
	    .arg( sysCall )
	    .arg( resourceName );
    }

    return msg;
}
