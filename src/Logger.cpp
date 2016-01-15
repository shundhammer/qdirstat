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
#include <unistd.h>	// getpid()
#include <sys/types.h>	// pid_t


static void qt_logger( QtMsgType msgType,
		       const QMessageLogContext &context,
		       const QString & msg );

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
	qInstallMessageHandler(0); // Restore default message handler
    }
}


void Logger::setDefaultLogger()
{
    _defaultLogger = this;
    qInstallMessageHandler( qt_logger );
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
	       << sev << " "
	       << srcFile << ":" << srcLine << " "
	       << srcFunction << "():  ";

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


static void qt_logger( QtMsgType msgType,
		       const QMessageLogContext &context,
		       const QString & msg )
{
    LogSeverity severity;

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

    Logger::log( 0, // use default logger
		 context.file, context.line, context.function,
		 severity )
	<< msg << endl;

    if ( msgType == QtFatalMsg )
	abort();
}


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
