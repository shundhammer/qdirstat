/*
 *   File name: OutputWindow.cpp
 *   Summary:	Terminal-like window to watch output of an external process
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QApplication>
#include <QCloseEvent>
#include <QTimer>

#include "OutputWindow.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"

using QDirStat::readColorEntry;
using QDirStat::writeColorEntry;
using QDirStat::readFontEntry;
using QDirStat::writeFontEntry;


#define CONNECT_ACTION(ACTION, RECEIVER, RCVR_SLOT) \
    connect( (ACTION), SIGNAL( triggered() ), (RECEIVER), SLOT( RCVR_SLOT ) )


OutputWindow::OutputWindow( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::OutputWindow ),
    _showOnStderr( true ),
    _noMoreProcesses( false ),
    _closed( false ),
    _killedAll( false ),
    _errorCount( 0 )
{
    _ui->setupUi( this );
    logDebug() << "Creating" << endl;
    readSettings();

    _ui->terminal->clear();
    setAutoClose( false );

    CONNECT_ACTION( _ui->actionZoomIn,	    this, zoomIn()    );
    CONNECT_ACTION( _ui->actionZoomOut,	    this, zoomOut()   );
    CONNECT_ACTION( _ui->actionResetZoom,   this, resetZoom() );
    CONNECT_ACTION( _ui->actionKillProcess, this, killAll()   );

    updateActions();
}


OutputWindow::~OutputWindow()
{
    logDebug() << "Destructor" << endl;

    if ( ! _processList.isEmpty() )
    {
	logWarning() << _processList.size() << " processes left over" << endl;

	foreach ( Process * process, _processList )
	    logWarning() << "Left over: " << process << endl;

	qDeleteAll( _processList );
    }

    writeSettings();
    delete _ui;
}


void OutputWindow::addProcess( Process * process )
{
    CHECK_PTR( process );

    if ( _killedAll )
    {
	logInfo() << "User killed all processes - "
                  << "no longer accepting new processes" << endl;
	process->kill();
	process->deleteLater();
    }

    _processList << process;
    // logDebug() << "Adding " << process << endl;

    connect( process, SIGNAL( readyReadStandardOutput() ),
	     this,    SLOT  ( readStdout()		) );

    connect( process, SIGNAL( readyReadStandardError() ),
	     this,    SLOT  ( readStderr()	       ) );

    connect( process, SIGNAL( error	  ( QProcess::ProcessError ) ),
	     this,    SLOT  ( processError( QProcess::ProcessError ) ) );

    connect( process, SIGNAL( finished	     ( int, QProcess::ExitStatus ) ),
	     this,    SLOT  ( processFinished( int, QProcess::ExitStatus ) ) );

    if ( ! hasActiveProcess() )
	startNextProcess();
}


void OutputWindow::addCommandLine( const QString commandline )
{
    addText( commandline, _commandTextColor );
}


void OutputWindow::addStdout( const QString output )
{
    addText( output, _stdoutColor );
}


void OutputWindow::addStderr( const QString output )
{
    _errorCount++;
    addText( output, _stderrColor );
    logWarning() << output << ( output.endsWith( "\n" ) ? "" : "\n" );

    if ( _showOnStderr && ! isVisible() && ! _closed )
	show();
}


void OutputWindow::addText( const QString & rawText, const QColor & textColor )
{
    if ( rawText.isEmpty() )
	return;

    QString text = rawText;

    if ( ! text.endsWith( "\n" ) )
	text += "\n";

    _ui->terminal->moveCursor( QTextCursor::End );
    QTextCursor cursor( _ui->terminal->textCursor() );

    QTextCharFormat format;
    format.setForeground( QBrush( textColor ) );
    cursor.setCharFormat( format );
    cursor.insertText( text );
}


void OutputWindow::clearOutput()
{
    _ui->terminal->clear();
}


Process * OutputWindow::senderProcess( const char * function ) const
{
    Process * process = qobject_cast<Process *>( sender() );

    if ( ! process )
    {
	if ( sender() )
	{
	    logError() << "Expecting QProcess as sender() in " << function
		       <<" , got "
		       << sender()->metaObject()->className() << endl;
	}
	else
	{
	    logError() << "NULL sender() in " << function << endl;
	}
    }

    return process;
}


void OutputWindow::readStdout()
{
    Process * process = senderProcess( __FUNCTION__ );

    if ( process )
	addStdout( QString::fromUtf8( process->readAllStandardOutput() ) );
}


void OutputWindow::readStderr()
{
    Process * process = senderProcess( __FUNCTION__ );

    if ( process )
	addStderr( QString::fromUtf8( process->readAllStandardError() ) );
}


void OutputWindow::processFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    switch ( exitStatus )
    {
	case QProcess::NormalExit:
	    logDebug() << "Process finished normally." << endl;
	    addCommandLine( tr( "Process finished." ) );
	    break;

	case QProcess::CrashExit:

	    if ( exitCode == 0 )
	    {
		// Don't report an exit code of 0: Since we are starting all
		// processes with a shell, that exit code would be the exit
		// code of the shell; that would only be useful if the shell
		// crashed or could not be started.

		logError() << "Process crashed." << endl;
		addStderr( tr( "Process crashed." ) );
	    }
	    else
	    {
		logError() << "Process crashed. Exit code: " << exitCode << endl;
		addStderr( tr( "Process crashed. Exit code: %1" ).arg( exitCode ) );
	    }
	    break;
    }

    Process * process = senderProcess( __FUNCTION__ );

    if ( process )
    {
	_processList.removeAll( process );

	if ( _processList.isEmpty() && _noMoreProcesses )
	{
	    logDebug() << "Emitting lastProcessFinished() err: " << _errorCount << endl;
	    emit lastProcessFinished( _errorCount );
	}

	process->deleteLater();
	closeIfDone();
    }

    startNextProcess(); // this also calls updateActions()
}


void OutputWindow::processError( QProcess::ProcessError error )
{
    QString msg;

    switch ( error )
    {
	case QProcess::FailedToStart:
	    msg = tr( "Error: Process failed to start." );
	    break;

	case QProcess::Crashed: // Already reported via processFinished()
	    break;

	case QProcess::Timedout:
	    msg = tr( "Error: Process timed out." );
	    break;

	case QProcess::ReadError:
	    msg = tr( "Error reading output from the process." );
	    break;

	case QProcess::WriteError:
	    msg = tr( "Error writing data to the process." );
	    break;

	case QProcess::UnknownError:
	    msg = tr( "Unknown error." );
	    break;
    }

    if ( ! msg.isEmpty() )
    {
	logError() << msg << endl;
	addStderr( msg );
    }

    Process * process = senderProcess( __FUNCTION__ );

    if ( process )
    {
	_processList.removeAll( process );

	if ( _processList.isEmpty() && _noMoreProcesses )
	{
	    logDebug() << "Emitting lastProcessFinished() err: " << _errorCount << endl;
	    emit lastProcessFinished( _errorCount );
	}

	process->deleteLater();
    }

    startNextProcess(); // this also calls updateActions()

    if ( ! _showOnStderr && ! isVisible() )
	closeIfDone();
}


void OutputWindow::closeIfDone()
{
    if ( _processList.isEmpty() && _noMoreProcesses )
    {
	if ( ( autoClose() && _errorCount == 0 ) ||
	     _closed || ! isVisible() )
	{
	    logDebug() << "No more processes to watch. Auto-closing." << endl;
	    this->deleteLater(); // It is safe to call this multiple times
	}
    }
}


void OutputWindow::noMoreProcesses()
{
    _noMoreProcesses = true;

    if ( _processList.isEmpty() && _noMoreProcesses )
    {
	logDebug() << "Emitting lastProcessFinished() err: " << _errorCount << endl;
	emit lastProcessFinished( _errorCount );
    }

    closeIfDone();
}


void OutputWindow::zoom( double factor )
{
    QFont font = _ui->terminal->font();

    if ( font.pixelSize() != -1 )
    {
	int pixelSize = qRound( font.pixelSize() * factor );
	logDebug() << "New font size: " << pixelSize << " pixels" << endl;
	font.setPixelSize( pixelSize );
    }
    else
    {
	qreal pointSize = font.pointSize() * factor;
	logDebug() << "New font size: " << pointSize << " points" << endl;
	font.setPointSize( pointSize );
    }

    _ui->terminal->setFont( font );
}


void OutputWindow::zoomIn()
{
    zoom( 1.1 );
}


void OutputWindow::zoomOut()
{
    zoom( 1.0/1.1 );
}


void OutputWindow::resetZoom()
{
    logDebug() << "Resetting font to normal" << endl;
    _ui->terminal->setFont( _terminalDefaultFont );
}


void OutputWindow::killAll()
{
    int killCount = 0;

    foreach ( Process * process, _processList )
    {
	logInfo() << "Killing process " << process << endl;
	process->kill();
	_processList.removeAll( process );
	process->deleteLater();
	++killCount;
    }

    _killedAll = true;
    addCommandLine( killCount == 1 ?
		    tr( "Process killed." ) :
		    tr( "Killed %1 processes." ).arg( killCount ) );
}


#if 0
void OutputWindow::setTerminalBackground( const QColor & newColor )
{
    // TO DO
    // TO DO
    // TO DO
}
#endif


bool OutputWindow::hasActiveProcess() const
{
    foreach ( Process * process, _processList )
    {
	if ( process->state() == QProcess::Starting ||
	     process->state() == QProcess::Running )
	{
	    return true;
	}
    }

    return false;
}


Process * OutputWindow::pickQueuedProcess()
{
    foreach ( Process * process, _processList )
    {
	if ( process->state() == QProcess::NotRunning )
	    return process;
    }

    return 0;
}


Process * OutputWindow::startNextProcess()
{
    Process * process = pickQueuedProcess();

    if ( process )
    {
	QString dir = process->workingDirectory();

	if ( dir != _lastWorkingDir )
	{
	    addCommandLine( "cd " + dir );
	    _lastWorkingDir = dir;
	}

	addCommandLine( command( process ) );
	logInfo() << "Starting: " << process << endl;

	process->start();
	qApp->processEvents(); // Keep GUI responsive
    }

    updateActions();

    return process;
}


QString OutputWindow::command( Process * process )
{
    // The common case is to start an external command with
    //	  /bin/sh -c theRealCommand arg1 arg2 arg3 ...
    QStringList args = process->arguments();

    if ( ! args.isEmpty() )
	args.removeFirst();		// Remove the "-c"

    if ( args.isEmpty() )		// Nothing left?
	return process->program();	// Ok, use the program name
    else
	return args.join( " " );	// output only the real command and its args
}


bool OutputWindow::autoClose() const
{
    return _ui->autoCloseCheckBox->isChecked();
}


void OutputWindow::setAutoClose( bool autoClose )
{
    _ui->autoCloseCheckBox->setChecked( autoClose );
}


void OutputWindow::closeEvent( QCloseEvent * event )
{
    _closed = true;

    if ( _processList.isEmpty() && _noMoreProcesses )
	this->deleteLater();

    // If there are any more processes, wait until the last one is finished and
    // then deleteLater().

    event->accept();
}


void OutputWindow::updateActions()
{
    _ui->killButton->setEnabled( hasActiveProcess() );
}


void OutputWindow::showAfterTimeout( int timeoutMillisec )
{
    if ( timeoutMillisec <= 0 )
	timeoutMillisec = _defaultShowTimeout;

    QTimer::singleShot( timeoutMillisec, this, SLOT( timeoutShow() ) );
}


void OutputWindow::timeoutShow()
{
    if ( ! isVisible() && ! _closed )
	show();
}


void OutputWindow::readSettings()
{
    QDirStat::Settings settings;
    settings.beginGroup( "OutputWindow" );

    _terminalBackground	 = readColorEntry( settings, "TerminalBackground", QColor( Qt::black  ) );
    _commandTextColor	 = readColorEntry( settings, "CommandTextColor"	 , QColor( Qt::white  ) );
    _stdoutColor	 = readColorEntry( settings, "StdoutTextColor"	 , QColor( 0xff, 0xaa, 0x00 ) );
    _stderrColor	 = readColorEntry( settings, "StdErrTextColor"	 , QColor( Qt::red    ) );
    _terminalDefaultFont = readFontEntry ( settings, "TerminalFont"	 , _ui->terminal->font() );
    _defaultShowTimeout	 = settings.value( "DefaultShowTimeoutMillisec", 500 ).toInt();

    settings.endGroup();

    _ui->terminal->setFont( _terminalDefaultFont );
}


void OutputWindow::writeSettings()
{
    QDirStat::Settings settings;
    settings.beginGroup( "OutputWindow" );

    writeColorEntry( settings, "TerminalBackground", _terminalBackground  );
    writeColorEntry( settings, "CommandTextColor"  , _commandTextColor	  );
    writeColorEntry( settings, "StdoutTextColor"   , _stdoutColor	  );
    writeColorEntry( settings, "StdErrTextColor"   , _stderrColor	  );
    writeFontEntry ( settings, "TerminalFont"	   , _terminalDefaultFont );
    settings.setValue( "DefaultShowTimeoutMillisec", _defaultShowTimeout  );

    settings.endGroup();
}
