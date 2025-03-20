/*
 *   File name: Logger.cpp
 *   Summary:   Logger class for QDirstat
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:    Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QRectF>
#include <QPointF>
#include <QSizeF>
#include <QSize>

#include <string.h>     // strlen()
#include <stdlib.h>     // abort(), mkdtemp()
#include <unistd.h>     // getpid()
#include <errno.h>
#include <pwd.h>        // getpwuid()
#include <sys/types.h>  // pid_t, getpwuid()

#include "Logger.h"


#define VERBOSE_ROTATE  0

using std::endl;
using std::cerr;

static LogSeverity toLogSeverity( QtMsgType msgType );

static void qt_logger( QtMsgType                  msgType,
                       const QMessageLogContext & context,
                       const QString &            msg );


Logger * Logger::_defaultLogger = 0;
QString  Logger::_lastLogDir;


Logger::Logger( const QString & filename )
{
    init();
    createNullStream();
    openLogFile( filename );
}


Logger::Logger( const QString & rawLogDir,
                const QString & rawFilename,
                bool            doRotate,
                int             logRotateCount )
{
    init();
    createNullStream();

    QString logDir   = expandVariables( rawLogDir   );
    QString filename = expandVariables( rawFilename );

    logDir = createLogDir( logDir );
    _lastLogDir = logDir;

    if ( doRotate )
        logRotate( logDir, filename, logRotateCount );

    openLogFile( logDir + "/" + filename );
}


Logger::~Logger()
{
    if ( _logStream.is_open() )
    {
        // logInfo() << "-- Log End --\n" << endl;
        _logStream.close();
    }

    if ( this == _defaultLogger )
    {
        _defaultLogger = 0;
        qInstallMessageHandler(0); // Restore default message handler
    }
}


void Logger::init()
{
    _logLevel = LogSeverityVerbose;
}


void Logger::createNullStream()
{
    // Open the null device to suppress output below the log level: This is
    // necessary because each call to operator<<() for LogStream returns the
    // LogStream, so we really need to return _nullStream (connected with
    // /dev/null) to actually suppress anything; otherwise, it's just the
    // logger time stamp etc. that gets suppressed, not the real logging
    // output.

    _nullStream.open( "/dev/null" );

    if ( _nullStream.fail() )
        cerr << "ERROR: Can't open /dev/null to suppress log output" << endl;
}


void Logger::openLogFile( const QString & filename )
{
    if ( ! _logStream.is_open() || _logFilename != filename )
    {
        _logFilename = filename;
        _logStream.open( filename.toUtf8(), std::ofstream::out | std::ofstream::app );

        if ( _logStream.good() )
        {
            if ( ! _defaultLogger )
                setDefaultLogger();

            cerr << "Logging to " << qPrintable( filename ) << endl;
            _logStream << "\n\n";
            log( __FILE__, __LINE__, __FUNCTION__, LogSeverityInfo )
                << "-- Log Start --" << endl;
        }
        else
        {
            cerr << "ERROR: Can't open log file " << qPrintable( filename ) << endl;
        }
    }
}


void Logger::setDefaultLogger()
{
    _defaultLogger = this;
    qInstallMessageHandler( qt_logger );
}


LogStream & Logger::log( Logger *        logger,
                         const QString & srcFile,
                         int             srcLine,
                         const QString & srcFunction,
                         LogSeverity     severity )
{
    static LogStream stderrStream;

    if ( ! logger )
        logger = Logger::defaultLogger();

    if ( logger )
        return logger->log( srcFile, srcLine, srcFunction, severity );
    else
    {
        if ( ! stderrStream.is_open() )
            stderrStream.open( "/dev/stderr" );

        return stderrStream;
    }
}


LogStream & Logger::log( const QString & srcFile,
                         int             srcLine,
                         const QString & srcFunction,
                         LogSeverity     severity )
{
    if ( severity < _logLevel )
        return _nullStream;

    std::string sev;

    switch ( severity )
    {
        case LogSeverityVerbose:   sev = "<Verbose>"; break;
        case LogSeverityDebug:     sev = "<Debug>  "; break;
        case LogSeverityInfo:      sev = "<Info>   "; break;
        case LogSeverityWarning:   sev = "<WARNING>"; break;
        case LogSeverityError:     sev = "<ERROR>  "; break;
            // Intentionally omitting 'default' branch so the compiler can
            // complain about unhandled enum values
    }

    _logStream << Logger::timeStamp() << " "
               << "[" << (int) getpid() << "] "
               << sev << " ";

    if ( ! srcFile.isEmpty() )
    {
        if ( srcFile.contains( "/") )
        {
            // CMake just dumps the whole path wholesale to the compiler
            // command line which gcc merrily uses as __FILE__;
            // which results in an abysmal-looking log line.
            // So let's cut off the path: Use only the last (No. -1)
            // section delimited with '/'.

            QString basename = srcFile.section( '/', -1 );
            _logStream << basename;

            // I hate CMake. Seriously, WTF?!
        }
        else
        {
            _logStream << srcFile;
        }

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


QString Logger::prefixLines( const QString & prefix,
                             const QString & multiLineText )
{
    QStringList lines = multiLineText.split( "\n" );
    QString result = lines.isEmpty() ? QString() : prefix;
    result += lines.join( QString( "\n" ) + prefix );

    return result;
}


QString Logger::indentLines( int             indentWidth,
                             const QString & multiLineText )
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
        case QtInfoMsg:     severity = LogSeverityInfo;    break;
    }

    return severity;
}


static void qt_logger( QtMsgType                  msgType,
                       const QMessageLogContext & context,
                       const QString &            msg )
{
    const QStringList & lines = msg.split( "\n" );

    for ( QString line: lines )
    {
        // Remove utterly misleading message that will just dump a ton of bug
        // reports on the application maintainers just because some clueless
        // moron put this message into the Qt libs

        line.remove( "Reinstalling the application may fix this problem." );

        if ( ! line.trimmed().isEmpty() )
        {
            Logger::log( 0, // use default logger
                         context.file, context.line, context.function,
                         toLogSeverity( msgType ) )
                << "[Qt] " << line << endl;
        }
    }

    if ( msgType == QtFatalMsg )
    {
        if ( msg.contains( "Could not connect to display" ) ||
             msg.contains( "failed to start because no Qt platform plugin" ) )
        {
            if ( msg.contains( "Reinstalling the application may fix this problem" ) )
            {
                // Suppress this new message which is complete and utter garbage:
                //
                // "This application failed to start because no Qt platform
                // plugin could be initialized. Reinstalling the application
                // may fix this problem.
                //
                // Available platform plugins are: wayland-org.kde.kwin.qpa,
                // eglfs, linuxfb, minimal, minimalegl, offscreen, vnc,
                // wayland-egl, wayland, wayland-xcomposite-egl,
                // wayland-xcomposite-glx, xcb."
                //
                // Seriously, who comes up with bullshit like this?
                //
                // Anybody who thinks that this might help a user does not have
                // the first clue about user interaction. This only confuses
                // and frightens most users. Telling users to reinstall the
                // application (!) is no more than computer voodoo and cargo
                // cult. There is not even the slightest shred of evidence that
                // this might help, yet it is the most destructive approach.
                //
                // Even a simple "I don't know what the problem is" is more
                // helpful than this.
                //
                // Worse, the plethora of plug-ins and methods how to start a
                // Qt application and thus how to connect to a display only
                // multiplies the confusion. WTF is a normal user expected to
                // do? What user even knows which of those technologies his
                // system uses? And WTF should a user do with that
                // non-information about the various plug-ins? This is a
                // perfect example of completely overengineered bullshit.
                //
                // I with my 35+ years as a Unix/Linux/X11 software developer
                // have no clue what to do when they dump this message on
                // me. WTF do they expect from a normal user?
                //
                // Get it right or get out of this business. Seriously. The IT
                // world already has enough morons with enough useless messages
                // designed by a committee of clueless product managers and
                // marketing people.

                std::string text = "FATAL: Could not connect to the display.";
                cerr << "\n" << text << endl;
                logError() << text << endl;
            }
            else
            {
                cerr << "FATAL: " << qPrintable( msg ) << endl;
            }

            logInfo() << "-- Exiting --\n" << endl;
            exit( 1 ); // Don't dump core, just exit
        }
        else
        {
            cerr << "FATAL: " << qPrintable( msg ) << endl;
            logInfo() << "-- Aborting with core dump --\n" << endl;
            abort(); // Exit with core dump (it might contain a useful backtrace)
        }
    }

    if ( msgType == QtWarningMsg &&
         ( msg.contains( "QObject::connect"    ) ||
           msg.contains( "QObject::disconnect" )    ) )
    {
        // Duplicate this on stderr
        cerr << "Qt Warning: " << qPrintable( msg ) << endl;
    }
}


QString Logger::userName()
{
    // Not using getuid() because that function relies on the user owning the
    // controlling terminal which is wrong in many aspects:
    //
    // - There might not be a controlling terminal at all.
    // - The user owning the controlling terminal may or may not be the one
    //   starting this program.

    struct passwd * pw = getpwuid( getuid() );

    if ( pw )
        return pw->pw_name;
    else
        return QString::number( getuid() );
}


QString Logger::createLogDir( const QString & rawLogDir )
{
    QString logDir( rawLogDir );
    QDir rootDir( "/" );
    bool created = false;

    if ( ! rootDir.exists( logDir ) )
    {
        rootDir.mkpath( logDir );
        created = true;
    }

    QFileInfo dirInfo( logDir );

    if ( (uid_t) dirInfo.ownerId()  != getuid() )
    {
        logError() << "ERROR: Directory " << logDir
                   << " is not owned by " << userName() << endl;

        QByteArray nameTemplate( QString( logDir + "-XXXXXX" ).toUtf8() );
        char * result = mkdtemp( nameTemplate.data() );

        if ( result )
        {
            created = true;
            logDir = QString::fromUtf8( result );
        }
        else
        {
            logError() << "Could not create log dir " << QString( nameTemplate )
                       << ": " << formatErrno() << endl;

            logDir = "/";
            // No permissions to write to /,
            // i.e. the log will go to stderr instead
        }
    }

    if ( created )
    {
        QFile dir( logDir );
        dir.setPermissions( QFile::ReadOwner  |
                            QFile::WriteOwner |
                            QFile::ExeOwner    );
    }

    return logDir;
}


QString Logger::oldName( const QString & filename, int no )
{
    QString oldName = filename;

    if ( oldName.endsWith( ".log" ) )
        oldName.chop( sizeof( ".log" ) - 1 );

    oldName += QString( "-%1.old" ).arg( no, 2, 10, QChar( '0' ) );

    return oldName;
}


QString Logger::oldNamePattern( const QString & filename )
{
    QString pattern = filename;

    if ( pattern.endsWith( ".log" ) )
        pattern.chop( sizeof( ".log" ) - 1 );

    pattern += "-??.old";

    return pattern;
}


void Logger::logRotate( const QString & logDir,
                        const QString & filename,
                        int             logRotateCount )
{
    QDir dir( logDir );
    QStringList keepers;
    keepers << filename;

    for ( int i = logRotateCount - 1; i >= 0; --i )
    {
        QString currentName = i > 0 ? oldName( filename, i-1 ) : filename;
        QString newName     = oldName( filename, i );

        if ( dir.exists( newName ) )
        {
            bool success = dir.remove( newName );
#if VERBOSE_ROTATE
            logDebug() << "Removing " << newName << ( success ? "" : " FAILED" ) << endl;
#else
            Q_UNUSED( success );
#endif
        }

        if ( dir.exists( currentName ) )
        {
            bool success = dir.rename( currentName, newName );
#if VERBOSE_ROTATE
            logDebug() << "Renaming " << currentName << " to " << newName
                       << ( success ? "" : " FAILED" )
                       << endl;
#else
            Q_UNUSED( success );
#endif

            keepers << newName;
        }
    }

    const QStringList & matches = dir.entryList( QStringList() << oldNamePattern( filename ),
                                                 QDir::Files );

    for ( const QString & match: matches )
    {
        if ( ! keepers.contains( match ) )
        {
            bool success = dir.remove( match );
#if VERBOSE_ROTATE
            logDebug() << "Removing leftover " << match << ( success ? "" : " FAILED" ) << endl;
#else
            Q_UNUSED( success );
#endif
        }
    }
}


QString Logger::expandVariables( const QString & unexpanded )
{
    QString expanded = unexpanded;
    expanded.replace( "$USER", userName() );
    expanded.replace( "$UID" , QString::number( getuid() ) );

    return expanded;
}




LogStream & operator<<( LogStream & str, const char * text )
{
    // Need to resort to ugly low-level ostream::write()
    // to prevent an endless recursion
    str.write( text, strlen( text ) );

    return str;
}


LogStream & operator<<( LogStream & str, const QString & text )
{
    str << text.toUtf8().constData();
    return str;
}


LogStream & operator<<( LogStream & str, const QStringList & stringList )
{
    str << stringList.join( ", " );
    return str;
}



LogStream & operator<<( LogStream & str, const QRectF & rect )
{
    str << "QRectF("
        << " x: " << rect.x()
        << " y: " << rect.y()
        << " width: " << rect.width()
        << " height: " << rect.height()
        << " )";

    return str;
}


LogStream & operator<<( LogStream & str, const QPointF & point )
{
    str << "QPointF("
        << " x: " << point.x()
        << " y: " << point.y()
        << " )";

    return str;
}


LogStream & operator<<( LogStream & str, const QSizeF & size )
{
    str << "QSizeF("
        << " width: " << size.width()
        << " height: " << size.height()
        << " )";

    return str;
}


LogStream & operator<<( LogStream & str, const QSize & size )
{
    str << "QSize("
        << " width: " << size.width()
        << " height: " << size.height()
        << " )";

    return str;
}


QString formatErrno()
{
    return QString::number( errno );
}
