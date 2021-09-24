/*
 *   File name: ProcessStarter.h
 *   Summary:	Utilities for external processes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef ProcessStarter_h
#define ProcessStarter_h

#include <QObject>
#include <QList>

#include "Process.h"


namespace QDirStat
{
    /**
     * Class to manage starting a number of external processes, but limiting
     * the number of processes running in parallel. Whenever a process
     * finishes, the next one from the list is started.
     *
     * When all processes are started and the 'autoDelete' flag is set, this
     * class will delete itself.
     **/
    class ProcessStarter: public QObject
    {
        Q_OBJECT

    public:

        /**
         * Constructor.
         **/
        ProcessStarter( QObject * parent = 0 );

        /**
         * Add another process. This class does not take over ownership of the
         * process objects.
         **/
        void add( Process * process );

        /**
         * Begin starting processes.
         *
         * If 'autoDelete' is set, make sure to call this AFTER all processes
         * are added; otherwise this object might be deleted already when the
         * first processes finish very quickly.
         **/
        void start();

        /**
         * Return the maximum number of processes running in parallel.
         **/
        int maxParallel() const { return _maxParallel; }

        /**
         * Set the maximum number of processes running in parallel.
         **/
        void setMaxParallel( int newVal ) { _maxParallel = newVal; }

        /**
         * Return 'true' if this object will automatically delete itself when
         * the last process is started, 'false' otherwise.
         **/
        bool autoDelete() const { return _autoDelete; }

        /**
         * Set the autoDelete flag: If set, this object will automatically
         * delete itself when the last process is started. The default is
         * 'false'.
         **/
        void setAutoDelete( bool newVal ) { _autoDelete = newVal; }


    protected slots:

        /**
         * Notification that a process has finished.
         **/
        void processFinished( int                  exitCode,
                              QProcess::ExitStatus exitStatus );

    protected:

        /**
         * Start more processes until the limit (_maxParallel) is reached.
         **/
        void startProcesses();

        // Data members

        int              _maxParallel;
        bool             _autoDelete;
        bool             _started;
        QList<Process *> _running;
        QList<Process *> _waiting;
    };
}

#endif  // ProcessStarter_h
