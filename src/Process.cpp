/*
 *   File name: Process.cpp
 *   Summary:	Qt4/5 compatibility layer for QProcess
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Authors:	 Michael Matz <matz@suse.de>
 *		 Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <Process.h>

using namespace QDirStat;


Process::Process( QObject *parent ):
    QProcess( parent )
{
    // NOP
}


#if (QT_VERSION < QT_VERSION_CHECK( 5, 1, 0))

void Process::start()
{
    QProcess::start( _prog, _arglist );
}

#endif	// Qt < 5.1.0
