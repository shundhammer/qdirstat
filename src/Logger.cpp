/*
 *   File name: Logger.cpp
 *   Summary:	Logger class for QDirstat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Logger.h"

#include <QFile>
#include <QDateTime>
#include <QString>
#include <QStringList>

#include <stdio.h>	// stderr, fprintf()
#include <stdlib.h>	// abort()
#include <unistd.h>	// getpid()
#include <sys/types.h>	// pid_t


static LogSeverity toLogSeverity( QtMsgType msgType );

#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
static void qt_logger( QtMsgType msgType, const char *msg);
#else
static void qt_logger( QtMsgType msgType,
		       const QMessageLogContext & context,
		       const QString & msg );
#endif

Logger * Logger::_defaultLogger = 0;


Logger::Logger( const QString &filename ):
    _logStream( stderr, QIODevice::WriteOnly ),
    _nullDevice( "/dev/null" ),
    _nullStream( stderr, QIODevice::WriteOnly ),
    _logLevel( LogSeverityVerbose )
{
    // Open the null device to suppress output below the log level: This is
    // necessary because each call to operator<<() for QTextStream returns the
    // QTextStream, so we really need to return _nullStream (connected with
    // /dev/null) to actually suppress anything; otherwise, it's just the
    // logger time stamp etc. that gets suppressed, not the real logging
    // output.

    if ( _nullDevice.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
	_nullStream.setDevice( &_nullDevice );
    }
    else
    {
	fprintf( stderr, "ERROR: Can't open /dev/null to suppress log output\n" );
    }


    //
    // Open the real log file
    //

    if ( ! _logFile.isOpen() || _logFile.fileName() != filename )
    {
	_logFile.setFileName( filename );

	if ( _logFile.open( QIODevice::WriteOnly |
			    QIODevice::Text	 |
			    QIODevice::Append	   ) )
	{
	    if ( ! _defaultLogger )
		setDefaultLogger();

	    fprintf( stderr, "Logging to %s\n", qPrintable( filename ) );
	    _logStream.setDevice( &_logFile );
	    _logStream << "\n\n";
	    log( __FILE__, __LINE__, __FUNCTION__, LogSeverityInfo )
		<< "-- Log Start --" << endl;
	}
	else
	{
	    fprintf( stderr, "ERROR: Can't open log file %s\n", qPrintable( filename ) );
	}
    }
}


Logger::~Logger()
{
    if ( _logFile.isOpen() )
    {
	logInfo() << "-- Log End --\n" << endl;
	_logFile.close();
    }

    if ( this == _defaultLogger )
    {
	_defaultLogger = 0;
#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
	qInstallMsgHandler(0);
#else
	qInstallMessageHandler(0); // Restore default message handler
#endif
    }
}


void Logger::setDefaultLogger()
{
    _defaultLogger = this;
#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
    qInstallMsgHandler( qt_logger );
#else
    qInstallMessageHandler( qt_logger );
#endif
}


QTextStream & Logger::log( Logger *	  logger,
			   const QString &srcFile,
			   int		  srcLine,
			   const QString &srcFunction,
			   LogSeverity	  severity )
{
    static QTextStream stderrStream( stderr, QIODevice::WriteOnly );

    if ( ! logger )
	logger = Logger::defaultLogger();

    if ( logger )
	return logger->log( srcFile, srcLine, srcFunction, severity );
    else
	return stderrStream;
}


QTextStream & Logger::log( const QString &srcFile,
			   int		  srcLine,
			   const QString &srcFunction,
			   LogSeverity	  severity )
{
    if ( severity < _logLevel )
	return _nullStream;

    QString sev;

    switch ( severity )
    {
	case LogSeverityVerbose:   sev = "<Verbose>"; break;
	case LogSeverityDebug:	   sev = "<Debug>  "; break;
	case LogSeverityInfo:	   sev = "<Info>   "; break;
	case LogSeverityWarning:   sev = "<WARNING>"; break;
	case LogSeverityError:	   sev = "<ERROR>  "; break;
	    // Intentionally omitting 'default' branch so the compiler can
	    // complain about unhandled enum values
    }

    _logStream << Logger::timeStamp() << " "
	       << "[" << (int) getpid() << "] "
	       << sev << " ";

    if ( ! srcFile.isEmpty() )
    {
	_logStream << srcFile;

	if ( srcLine > 0 )
	    _logStream << ":" << srcLine;

	_logStream << " ";

	if ( ! srcFunction.isEmpty() )
	_logStream << srcFunction << "():  ";
    }

    return _logStream;
}


void Logger::newline( Logger *logger )
{
    if ( ! logger )
	logger = Logger::defaultLogger();

    if ( logger )
	logger->newline();
}


LogSeverity Logger::logLevel( Logger *logger )
{
    if ( ! logger )
	logger = Logger::defaultLogger();

    if ( logger )
	return logger->logLevel();
    else
	return LogSeverityVerbose;
}


void Logger::setLogLevel( Logger *logger, LogSeverity newLevel )
{
    if ( ! logger )
	logger = Logger::defaultLogger();

    if ( logger )
	logger->setLogLevel( newLevel );
}


void Logger::newline()
{
    _logStream << endl;
}


QString Logger::timeStamp()
{
    return QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss.zzz" );
}


QString Logger::prefixLines( const QString &prefix,
			     const QString &multiLineText )
{
    QStringList lines = multiLineText.split( "\n" );
    QString result = lines.isEmpty() ? QString() : prefix;
    result += lines.join( QString( "\n" ) + prefix );

    return result;
}


QString Logger::indentLines( int indentWidth,
			     const QString &multiLineText )
{
    QString prefix( indentWidth, ' ' );

    return prefixLines( prefix, multiLineText );
}



static LogSeverity toLogSeverity( QtMsgType msgType )
{
    LogSeverity severity = LogSeverityVerbose;

    switch ( msgType )
    {
	case QtDebugMsg:    severity = LogSeverityVerbose; break;
	case QtWarningMsg:  severity = LogSeverityWarning; break;
	case QtCriticalMsg: severity = LogSeverityError;   break;
	case QtFatalMsg:    severity = LogSeverityError;   break;
#if QT_VERSION >= 0x050500
	case QtInfoMsg:	    severity = LogSeverityInfo;	   break;
#endif
    }

    return severity;
}


#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 )) // Qt 4.x

static void qt_logger( QtMsgType msgType, const char *msg)
{
    Logger::log( 0, // use default logger
		 "[Qt]", 0, "", // file, line, function
		 toLogSeverity( msgType ) )
	<< msg << endl;

    if ( msgType == QtFatalMsg )
    {
	fprintf( stderr, "FATAL: %s\n", qPrintable( msg ) );
	abort();
    }

    if ( msgType == QtWarningMsg &&
	 QString( msg ).contains( "cannot connect to X server" ) )
    {
	fprintf( stderr, "FATAL: %s\n", qPrintable( msg ) );
	exit( 1 );
    }
}

#else // Qt 5.x

static void qt_logger( QtMsgType msgType,
		       const QMessageLogContext & context,
		       const QString & msg )
{
    Logger::log( 0, // use default logger
		 context.file, context.line, context.function,
		 toLogSeverity( msgType ) )
	<< msg << endl;

    if ( msgType == QtFatalMsg )
    {
	fprintf( stderr, "FATAL: %s\n", qPrintable( msg ) );

	if ( msg.contains( "Could not connect to display" ) )
	    exit( 1 );
	else
	    abort();
    }
}

#endif // Qt 5.x



QTextStream & operator<<( QTextStream &str, const QStringList &stringList )
{
    str << stringList.join( ", " );
    return str;
}


QTextStream & operator<<( QTextStream &str, bool val )
{
    str << ( val ? "true" : "false" );
    return str;
}
