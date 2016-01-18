/*
 *   File name: OutputWindow.h
 *   Summary:	Terminal-like window to watch output of an external process
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef OutputWindow_h
#define OutputWindow_h

#include <QDialog>
#include <QList>
#include <QTextStream>
#include <QStringList>

#include "ui_output-window.h"
#include "Process.h"

class QCloseEvent;
using QDirStat::Process;


/**
 * Terminal-like window to watch output of external processes started via
 * QProcess. The command invoked by the process, its stdout and stderr output
 * are displayed in different colors.
 *
 * This class can watch more than one process: It can watch a sequence of
 * processes, such as QDirStat cleanup actions as they are invoked for each
 * selected item one after another, or even multiple processes running in
 * parallel (which may make the output a bit messy, of course).
 *
 * If this dialog is created, but now shown, it will (by default) show itself
 * as soon as there is any output on stderr.
 **/
class OutputWindow: public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    OutputWindow( QWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~OutputWindow();

    /**
     * Add a process to watch. Ownership of the process is transferred to this
     * object. If the process is not started yet, it will be started as soon as
     * there is no other one running.
     **/
    void addProcess( Process * process );

    /**
     * Tell this dialog that no more processes will be added, so when the last
     * one is finished and the "auto close" checkbox is checked, it may close
     * itself.
     **/
    void noMoreProcesses();

    /**
     * Return the number of errors the processes reported.
     **/
    int errorCount() const { return _errorCount; }

    /**
     * Return 'true' if this dialog closes itself automatically after the last
     * process finished successfully.
     **/
    bool autoClose() const;

    /**
     * Set if this dialog should close itself automatically after the last
     * process finished successfully.
     **/
    void setAutoClose( bool autoClose );

    /**
     * Set if this dialog should show itself if there is any output on
     * stderr. The default is 'true'.
     *
     * This means an application can create the dialog and leave it hidden, and
     * if there is any error output, it will automatically show itself -- with
     * all previous output of the watched processes on stdout and stderr.
     * If the user closes the dialog, however, it will remain closed.
     **/
    void setShowOnStderr( bool show ) { _showOnStderr = show; }

    /**
     * Return 'true' if this dialog shows itself if there is any output on
     * stderr.
     **/
    bool showOnStderr() const { return _showOnStderr; }

    /**
     * Show window (if not already shown) after the specified timeout has
     * elapsed. This is useful for operations that might be very short, so no
     * output window is desired, but that sometimes might also take a while.
     *
     * If 'timeoutMillisec' is 0, defaultShowTimeout() is used.
     **/
    void showAfterTimeout( int timeoutMillisec = 0 );

    /**
     * Return the default show timeout in milliseconds.
     **/
    int defaultShowTimeout() const { return _defaultShowTimeout; }

    /**
     * set the default show timeout (in milliseconds).
     **/
    void setDefaultShowTimeout( int newTimeoutMillisec )
	{ _defaultShowTimeout = newTimeoutMillisec; }

    /**
     * Return the text color for commands in the terminal area.
     **/
    QColor commandTextColor() const { return _commandTextColor; }

    /**
     * Set the text color for commands in the terminal area.
     **/
    void setCommandTextColor( const QColor & newColor )
	{ _commandTextColor = newColor; }

    /**
     * Return the text color for stdout output in the terminal area.
     **/
    QColor stdoutColor() const { return _stdoutColor; }

    /**
     * Set the text color for stdout output in the terminal area.
     **/
    void setStdoutColor( const QColor & newColor )
	{ _stdoutColor = newColor; }

    /**
     * Return the text color for stderr output in the terminal area.
     **/
    QColor stderrColor() const { return _stderrColor; }

    /**
     * Set the text color for stderr output in the terminal area.
     **/
    void setStderrColor( const QColor & newColor )
	{ _stderrColor = newColor; }

#if 0
    /**
     * Return the background color of the terminal area.
     **/
    QColor terminalBackground() const { return _terminalBackground; }

    /**
     * Set the background color of the terminal area.
     **/
    void setTerminalBackground( const QColor & newColor );
#endif

    /**
     * Return the internal process list.
     **/
    const QList<Process *> & processList() const { return _processList; }

    /**
     * Return 'true' if any process in the internal process is still active.
     **/
    bool hasActiveProcess() const;

    /**
     * Get the command of the process. Since usually processes are started via
     * a shell ("/bin/sh -c theRealCommand arg1 arg2 ..."), this is typically
     * not QProcess::program(), but the arguments minus the "-c".
     **/
    static QString command( Process * process );


public slots:

    /**
     * Add a command line to show in the output area.
     * This is typically displayed in white.
     **/
    void addCommandLine( const QString commandline );

    /**
     * Add one or more lines of stdout to show in the output area.
     * This is typically displayed in amber.
     **/
    void addStdout( const QString output );

    /**
     * Add one or more lines of stderr to show in the output area.
     * This is typically displayed in red.
     **/
    void addStderr( const QString output );

    /**
     * Kill all processes this class watches.
     **/
    void killAll();

    /**
     * Clear the output area, i.e. remove all previous output and commands.
     **/
    void clearOutput();

    /**
     * Enable or disable actions based on the internal status of this object.
     **/
    void updateActions();

    /**
     * Read parameters from the settings.
     **/
    void readSettings();

    /**
     * Write parameters to the settings.
     **/
    void writeSettings();


protected slots:

    /**
     * Read output on one of the watched process's stdout channel.
     **/
    void readStdout();

    /**
     * Read output on one of the watched process's stderr channel.
     **/
    void readStderr();

    /**
     * One of the watched processes finished.
     **/
    void processFinished( int exitCode, QProcess::ExitStatus exitStatus );

    /**
     * One of the watched processes terminated with an error.
     **/
    void processError( QProcess::ProcessError error );

    /**
     * Zoom the output area in, i.e. make its font larger.
     **/
    void zoomIn();

    /**
     * Zoom the output area out, i.e. make its font smaller.
     **/
    void zoomOut();

    /**
     * Reset the output area zoom, i.e. restore its default font.
     **/
    void resetZoom();

    /**
     * Show after timeout has elapsed (unless the user closed this dialog
     * before)
     **/
    void timeoutShow();


signals:

    /**
     * Emitted when the last process finished, no matter if that was successful
     * or with an error. 'totalErrorCount' is the accumulated error count of
     * all processes this OutputWindow watched.
     **/
    void lastProcessFinished( int totalErrorCount );


protected:

    /**
     * Close event: Invoked upon QDialog::close(), i.e. the "Close" button, the
     * window manager close button (the [x] at the top right), or when this
     * dialog decides to auto-close itself after the last process finishes
     * successfully.
     *
     * This object will delete itself in this event if there are no more
     * processes to watch.
     *
     * Reimplemented from QDialog / QWidget.
     **/
    void closeEvent( QCloseEvent * event ) Q_DECL_OVERRIDE;

    /**
     * Close if there are no more processes and there is no error to show.
     **/
    void closeIfDone();

    /**
     * Add one or more lines of text in text color 'textColor' to the output
     * area.
     **/
    void addText( const QString & text, const QColor & textColor );

    /**
     * Obtain the process to use from sender(). Return 0 if this is not a
     * QProcess.
     **/
    Process * senderProcess( const char * callingFunctionName ) const;

    /**
     * Pick the next inactive process that can be started. Return 0 if there is
     * none.
     **/
    Process * pickQueuedProcess();

    /**
     * Try to start the next inactive process, if there is any. Return that
     * process or 0 if there is none.
     **/
    Process * startNextProcess();

    /**
     * Zoom the terminal font by the specified factor.
     **/
    void zoom( double factor = 1.0 );


    //
    // Data members
    //

    Ui::OutputWindow  * _ui;
    QList<Process *>	_processList;
    bool		_showOnStderr;
    bool		_noMoreProcesses;
    bool		_closed;
    bool		_killedAll;
    int			_errorCount;
    QString		_lastWorkingDir;
    QColor		_terminalBackground;
    QColor		_commandTextColor;
    QColor		_stdoutColor;
    QColor		_stderrColor;
    QFont		_terminalDefaultFont;
    int			_defaultShowTimeout;

};	// class OutputWindow


inline QTextStream & operator<< ( QTextStream & stream, Process * process )
{
    if ( process )
	stream << OutputWindow::command( process );
    else
	stream << "<NULL QProcess>";

    return stream;
}



#endif	// OutputWindow_h
