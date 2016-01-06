/*
 *   File name: ProcessOutput.cpp
 *   Summary:	Terminal-like window to watch output of an external process
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QApplication>
#include <QCloseEvent>

#include "ProcessOutput.h"
#include "Logger.h"
#include "Exception.h"


#define CONNECT_ACTION(ACTION, RECEIVER, RCVR_SLOT) \
    connect( (ACTION), SIGNAL( triggered() ), (RECEIVER), SLOT( RCVR_SLOT ) )


ProcessOutput::ProcessOutput( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::ProcessOutputDialog ),
    _showOnStderr( true ),
    _noMoreProcesses( false ),
    _hadError( false ),
    _closed( false )
{
    _ui->setupUi( this );
    logDebug() << "Creating" << endl;

    _terminalBackground	 = Qt::black;
    _commandTextColor	 = Qt::white;
    _stdoutColor	 = QColor( 0xff, 0xaa, 0x00 );
    _stderrColor	 = Qt::red;
    _terminalDefaultFont = _ui->terminal->font();

    _ui->terminal->clear();

    CONNECT_ACTION( _ui->actionZoomIn,	    this, zoomIn()    );
    CONNECT_ACTION( _ui->actionZoomOut,	    this, zoomOut()   );
    CONNECT_ACTION( _ui->actionResetZoom,   this, resetZoom() );
    CONNECT_ACTION( _ui->actionKillProcess, this, killAll()   );

    updateActions();
}


ProcessOutput::~ProcessOutput()
{
    logDebug() << "Destructor" << endl;

    if ( ! _processList.isEmpty() )
    {
	logWarning() << _processList.size() << " processes left over" << endl;

	foreach ( QProcess * process, _processList )
	    logWarning() << "Left over: " << process << endl;

	qDeleteAll( _processList );
    }
}


void ProcessOutput::addProcess( QProcess * process )
{
    CHECK_PTR( process );
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


void ProcessOutput::addCommandLine( const QString commandline )
{
    addText( commandline, _commandTextColor );
}


void ProcessOutput::addStdout( const QString output )
{
    addText( output, _stdoutColor );
}


void ProcessOutput::addStderr( const QString output )
{
    _hadError = true;
    addText( output, _stderrColor );

    if ( _showOnStderr && ! isVisible() && ! _closed )
	show();
}


void ProcessOutput::addText( const QString & rawText, const QColor & textColor )
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


void ProcessOutput::clearOutput()
{
    _ui->terminal->clear();
}


QProcess * ProcessOutput::senderProcess( const char * function ) const
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


void ProcessOutput::readStdout()
{
    QProcess * process = senderProcess( __FUNCTION__ );

    if ( process )
	addStdout( QString::fromUtf8( process->readAllStandardOutput() ) );
}


void ProcessOutput::readStderr()
{
    QProcess * process = senderProcess( __FUNCTION__ );

    if ( process )
	addStderr( QString::fromUtf8( process->readAllStandardError() ) );
}


void ProcessOutput::processFinished( int exitCode, QProcess::ExitStatus exitStatus )
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
	    _hadError = true;
	    addStderr( tr( "Process crashed." ) );
	    break;
    }

    QProcess * process = senderProcess( __FUNCTION__ );

    if ( process )
    {
	_processList.removeAll( process );

	if ( _processList.isEmpty() && _noMoreProcesses )
	    emit lastProcessFinished();

	process->deleteLater();
	closeIfDone();
    }

    startNextProcess(); // this also calls updateActions()
}


void ProcessOutput::processError( QProcess::ProcessError error )
{
    _hadError = true;
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
	    emit lastProcessFinished();

	process->deleteLater();
    }

    logError() << msg << endl;
    addStderr( msg );
    startNextProcess(); // this also calls updateActions()

    if ( ! _showOnStderr && ! isVisible() )
	closeIfDone();
}


void ProcessOutput::closeIfDone()
{
    if ( _processList.isEmpty() && _noMoreProcesses )
    {
	if ( ( autoClose() && ! _hadError ) ||
	     _closed || ! isVisible() )
	{
	    logDebug() << "No more processes to watch. Auto-closing." << endl;
	    this->deleteLater(); // It is safe to call this multiple times
	}
    }
}


void ProcessOutput::noMoreProcesses()
{
    _noMoreProcesses = true;
    closeIfDone();
}


void ProcessOutput::zoom( double factor )
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


void ProcessOutput::zoomIn()
{
    zoom( 1.1 );
}


void ProcessOutput::zoomOut()
{
    zoom( 1.0/1.1 );
}


void ProcessOutput::resetZoom()
{
    logDebug() << "Resetting font to normal" << endl;
    _ui->terminal->setFont( _terminalDefaultFont );
}


void ProcessOutput::killAll()
{
    foreach ( QProcess * process, _processList )
    {
	logDebug() << "Killing process " << process << endl;
	process->kill();
	_processList.removeAll( process );
	process->deleteLater();
    }
}


#if 0
void ProcessOutput::setTerminalBackground( const QColor & newColor )
{
    // TO DO
    // TO DO
    // TO DO
}
#endif


bool ProcessOutput::hasActiveProcess() const
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


QProcess * ProcessOutput::pickQueuedProcess()
{
    foreach ( QProcess * process, _processList )
    {
	if ( process->state() == QProcess::NotRunning )
	    return process;
    }

    return 0;
}


QProcess * ProcessOutput::startNextProcess()
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


QString ProcessOutput::command( QProcess * process )
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


bool ProcessOutput::autoClose() const
{
    return _ui->autoCloseCheckBox->isChecked();
}


void ProcessOutput::setAutoClose( bool autoClose )
{
    _ui->autoCloseCheckBox->setChecked( autoClose );
}


void ProcessOutput::closeEvent( QCloseEvent * event )
{
    _closed = true;

    if ( _processList.isEmpty() && _noMoreProcesses )
	this->deleteLater();

    // If there are any more processes, wait until the last one is finished and
    // then deleteLater().

    event->accept();
}


void ProcessOutput::updateActions()
{
    _ui->killButton->setEnabled( hasActiveProcess() );
}
