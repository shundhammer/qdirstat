/*
 *   File name: ProcessStarter.h
 *   Summary:	Utilities for external processes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "ProcessStarter.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


ProcessStarter::ProcessStarter( QObject * parent ):
    QObject( parent ),
    _maxParallel( 8 ),
    _autoDelete( false ),
    _started( false )
{

}


void ProcessStarter::start()
{
    logDebug() << "Starting. Processes in queue: " << _waiting.count() << endl;
    logDebug() << "Maximum parallel processes: " << _maxParallel << endl;

    _started = true;
    startProcesses();
}


void ProcessStarter::add( Process * process )
{
    _waiting.append( process );

    connect( process, SIGNAL( finished	     ( int, QProcess::ExitStatus ) ),
	     this,    SLOT  ( processFinished( int, QProcess::ExitStatus ) ) );

    if ( _started )
        startProcesses();
}


void ProcessStarter::startProcesses()
{
    while ( _running.size() < _maxParallel )
    {
        if ( _waiting.isEmpty() )
            return;

        Process * process = _waiting.takeFirst();

        if ( process )
        {
            process->start();
            _running.append( process );
        }
    }
}


void ProcessStarter::processFinished( int                  exitCode,
                                      QProcess::ExitStatus exitStatus )
{
    Q_UNUSED( exitCode   );
    Q_UNUSED( exitStatus );

    Process * process = qobject_cast<Process *>( sender() );

    if ( ! process )
    {
        logError() << "Ignoring non-process QObject " << (void *) sender() << endl;
        return;
    }

    _running.removeAll( process );
    _waiting.removeAll( process ); // It shouldn't be in _waiting; just making sure

    if ( _started )
    {
        if ( _waiting.isEmpty() )
        {
            // logDebug() << "All processes started." << endl;

            if ( _autoDelete )
                deleteLater();
        }
        else
        {
            startProcesses();
        }
    }
}
