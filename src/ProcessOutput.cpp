/*
 *   File name: ProcessOutput.cpp
 *   Summary:	Terminal-like window to watch output of an external process
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "ProcessOutput.h"
#include "Logger.h"
#include "Exception.h"


#define CONNECT_ACTION(ACTION, RECEIVER, RCVR_SLOT) \
    connect( (ACTION), SIGNAL( triggered() ), (RECEIVER), SLOT( RCVR_SLOT ) )


ProcessOutput::ProcessOutput( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::ProcessOutputDialog )
{
    _ui->setupUi( this );
    logDebug() << "Creating" << endl;
    setAttribute( Qt::WA_DeleteOnClose );

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
}


ProcessOutput::~ProcessOutput()
{
    logDebug() << "Destructor" << endl;
}


void ProcessOutput::addProcess( QProcess * process )
{
    CHECK_PTR( process );
    _processList << QPointer<QProcess>( process );
    logDebug() << "Adding " << process << endl;

    connect( process, SIGNAL( readyReadStandardOutput() ),
	     this,    SLOT  ( readStdout()		) );

    connect( process, SIGNAL( readyReadStandardError() ),
	     this,    SLOT  ( readStderr()	       ) );

    connect( process, SIGNAL( error	  ( QProcess::ProcessError ) ),
	     this,    SLOT  ( processError( QProcess::ProcessError ) ) );

    connect( process, SIGNAL( finished	     ( int, QProcess::ExitStatus ) ),
	     this,    SLOT  ( processFinished( int, QProcess::ExitStatus ) ) );
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
    addText( output, _stderrColor );
}


void ProcessOutput::addText( const QString & rawText, const QColor & textColor )
{
    QString text = rawText;

    if ( text.isEmpty() )
        return;

    if ( _ui->terminal->document()->isEmpty() )
        addHeader();

    QString rgbCode   = textColor.name();

#if 0
    QStringList lines = text.split( "\n" );
    QString spanBegin = QString( "<span style=\" color:%1;\">" ).arg( rgbCode );
    QString spanEnd   = "</span>";

    QString html = spanBegin + "<p>" + lines.join( "</p>\n<p>" ) + "</p>" + spanEnd;
#endif

    if ( ! text.endsWith( "\n" ) )
        text += "\n";

    _ui->terminal->moveCursor( QTextCursor::End );
    QTextCursor cursor( _ui->terminal->textCursor() );

    QTextCharFormat format;
    // format.setFontWeight( QFont::DemiBold );
    format.setForeground( QBrush( textColor ) );
    cursor.setCharFormat( format );
    cursor.insertText( text );
}


void ProcessOutput::addHeader()
{
#if 0
    QString header =
	"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
	"<html><head>\n"
	"<meta name=\"qrichtext\" content=\"1\" />\n"
	"<style type=\"text/css\">\n"
	"p, li { white-space: pre-wrap; }\n"
	"</style>"
	"</head>"
	"<body style=\" font-family:'DejaVu Sans Mono'; font-size:10pt; font-weight:400; font-style:normal;\">\n";

    _ui->terminal->setHtml( header );
#endif
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
	    addStderr( tr( "Process crashed." ) );
	    break;
    }

    cleanupProcessList();
    QProcess * process = senderProcess( __FUNCTION__ );

    if ( process )
    {
	if ( _processList.size() == 1		&&
	     exitStatus == QProcess::NormalExit &&
	     process == _processList.first()	&&
	     _ui->autoCloseCheckBox->isChecked()  )
	{
	    logDebug() << "No more processes to watch. Auto-closing." << endl;
#if 0
            // DEBUG
            // DEBUG
            // DEBUG
	    close();
            // DEBUG
            // DEBUG
            // DEBUG
#endif
	}
    }
}


void ProcessOutput::processError( QProcess::ProcessError error )
{
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

    addStderr( msg );
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
    foreach ( QPointer<QProcess> process, _processList )
    {
	if ( process )
	{
	    logDebug() << "Killing " << process << endl;
	    process->kill();
	}
    }
}


void ProcessOutput::cleanupProcessList()
{
    _processList.removeAll( 0 );
}


void ProcessOutput::setTerminalBackground( const QColor & newColor )
{
    Q_UNUSED( newColor );
    // TO DO
    // TO DO
    // TO DO
}


bool ProcessOutput::hasActiveProcess() const
{
    foreach ( QPointer<QProcess> process, _processList )
    {
	if ( process )
	{
	    if ( process->state() == QProcess::Starting ||
		 process->state() == QProcess::Running )
	    {
		return true;
	    }
	}
    }

    return false;
}


QList<QProcess *> ProcessOutput::processList() const
{
    QList<QProcess *> procList;

    foreach ( QPointer<QProcess> process, _processList )
    {
	if ( process )
	    procList << process.data();
    }

    return procList;
}

