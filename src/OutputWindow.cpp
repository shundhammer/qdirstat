/*
 *   File name: OutputWindow.cpp
 *   Summary:	Terminal-like window to watch output of an external process
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QApplication>
#include <QCloseEvent>
#include <QSettings>
#include <QTimer>

#include "OutputWindow.h"
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

	foreach ( QProcess * process, _processList )
	    logWarning() << "Left over: " << process << endl;

	qDeleteAll( _processList );
    }

    writeSettings();
}


void OutputWindow::addProcess( QProcess * process )
{
    CHECK_PTR( process );

    if ( _killedAll )
    {
	logDebug() << "User killed all processes - "
		   << "no longer accepting new processes" << endl;
	process->kill();
	process->deleteLater();
    }

    _processList << process;
    logDebug() << "Adding " << process << endl;

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


QProcess * OutputWindow::senderProcess( const char * function ) const
{
    QProcess * process = qobject_cast<QProcess *>( sender() );

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
    QProcess * process = senderProcess( __FUNCTION__ );

    if ( process )
	addStdout( QString::fromUtf8( process->readAllStandardOutput() ) );
}


void OutputWindow::readStderr()
{
    QProcess * process = senderProcess( __FUNCTION__ );

    if ( process )
	addStderr( QString::fromUtf8( process->readAllStandardError() ) );
}


void OutputWindow::processFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
    logDebug() << "exitCode: " << exitCode
	       << " exitStatus: "
	       << ( exitStatus == QProcess::NormalExit ? "NormalExit" : "CrashExit" )
	       << endl;

    switch ( exitStatus )
    {
	case QProcess::NormalExit:
	    addCommandLine( tr( "Process finished." ) );
	    break;

	case QProcess::CrashExit:
	    _errorCount++;
	    addStderr( tr( "Process crashed." ) );
	    break;
    }

    QProcess * process = senderProcess( __FUNCTION__ );

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
    _errorCount++;
    QString msg = tr( "Unknown error." );

    switch ( error )
    {
	case QProcess::FailedToStart:
	    msg = tr( "Error: Process failed to start." );
	    break;

	case QProcess::Crashed:
	    msg = tr( "Error: Process crashed." );
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

    QProcess * process = senderProcess( __FUNCTION__ );

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

    logError() << msg << endl;
    addStderr( msg );
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

    foreach ( QProcess * process, _processList )
    {
	logDebug() << "Killing process " << process << endl;
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
    foreach ( QProcess * process, _processList )
    {
	if ( process->state() == QProcess::Starting ||
	     process->state() == QProcess::Running )
	{
	    return true;
	}
    }

    return false;
}


QProcess * OutputWindow::pickQueuedProcess()
{
    foreach ( QProcess * process, _processList )
    {
	if ( process->state() == QProcess::NotRunning )
	    return process;
    }

    return 0;
}


QProcess * OutputWindow::startNextProcess()
{
    QProcess * process = pickQueuedProcess();

    if ( process )
    {
	QString dir = process->workingDirectory();

	if ( dir != _lastWorkingDir )
	{
	    addCommandLine( "cd " + dir );
	    _lastWorkingDir = dir;
	}

	addCommandLine( command( process ) );
	logDebug() << "Starting " << process << endl;

	process->start();
	qApp->processEvents(); // Keep GUI responsive
    }

    updateActions();

    return process;
}


QString OutputWindow::command( QProcess * process )
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
    QSettings settings;
    settings.beginGroup( "OutputWindow" );

    _terminalBackground	 = readColorEntry( settings, "TerminalBackground", QColor( Qt::black  ) );
    _commandTextColor	 = readColorEntry( settings, "CommandTextColor"	 , QColor( Qt::white  ) );
    _stdoutColor	 = readColorEntry( settings, "StdoutTextColor"	 , QColor( 0xff, 0xaa, 0x00 ) );
    _stderrColor	 = readColorEntry( settings, "StdErrTextColor"	 , QColor( Qt::red    ) );
    _terminalDefaultFont = readFontEntry ( settings, "TerminalFont"	 , _ui->terminal->font() );
    _defaultShowTimeout	 = settings.value( "DefaultShowTimeoutMillisec", 3000 ).toInt();

    settings.endGroup();

    _ui->terminal->setFont( _terminalDefaultFont );
}


void OutputWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup( "OutputWindow" );

    writeColorEntry( settings, "TerminalBackground", _terminalBackground  );
    writeColorEntry( settings, "CommandTextColor"  , _commandTextColor	  );
    writeColorEntry( settings, "StdoutTextColor"   , _stdoutColor	  );
    writeColorEntry( settings, "StdErrTextColor"   , _stderrColor	  );
    writeFontEntry ( settings, "TerminalFont"	   , _terminalDefaultFont );
    settings.setValue( "DefaultShowTimeoutMillisec", _defaultShowTimeout  );

    settings.endGroup();
}
