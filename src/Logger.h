/*
 *   File name: Logger.h
 *   Summary:	Logger class for QDirstat
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef Logger_h
#define Logger_h

#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>


// Intentionally not using LogDebug, LogMilestone etc. to avoid confusion
// because of simple typos: logDebug() vs. LogDebug()
//
// ...well knowing that C++ compilers will throw all kinds of crazy and
// impossible to understand error messages if somebody confuses very different
// types like a function (logDebug()) vs. an enum value.

enum LogSeverity
{
    LogSeverityVerbose,
    LogSeverityDebug,
    LogSeverityWarning,
    LogSeverityError
};


// Log macros for stream (QTextStream) output.
//
// Unlike qDebug() etc., they also record the location in the source code that
// wrote the log entry.
//
// These macros all use the default logger. Create similar macros to use your
// own class-specific logger.

#define logVerbose()    Logger::log( 0, __FILE__, __LINE__, __FUNCTION__, LogSeverityVerbose   )
#define logDebug()      Logger::log( 0, __FILE__, __LINE__, __FUNCTION__, LogSeverityDebug     )
#define logWarning()    Logger::log( 0, __FILE__, __LINE__, __FUNCTION__, LogSeverityWarning   )
#define logError()      Logger::log( 0, __FILE__, __LINE__, __FUNCTION__, LogSeverityError     )
#define logNewline()    Logger::newline( 0 )


/**
 * Logging class. Use one of the macros above for stream output:
 *
 *     logDebug() << "Debug logging demo " << myString << ": " << 42 << endl;
 *     logError() << "Can't open file " << filename << ": " << errno << endl;
 *
 * Remember to terminate each log line with 'endl'.
 * Unlike qDebug() etc., this class does NOT add spaces or quotes.
 * There are overloads for most common types (QString, const char *,
 * QByteArray, int).
 *
 * This class also redirects Qt logging (qDebug() etc.) to the same log file.
 */
class Logger
{
public:
    /**
     * Constructor. Create a logger that logs to the specified file.
     * The first logger created is also implicitly used as the default
     * logger. This can be changed later with setDefaultLogger().
     */
    Logger( const QString &filename );

    /**
     * Destructor.
     */
    ~Logger();


    /**
     * Internal logging function. In most cases, better use the logDebug(),
     * logWarning() etc. macros instead.
     */
    QTextStream & log( const QString &srcFile,
                       int            srcLine,
                       const QString &srcFunction,
                       LogSeverity    severity );

    /**
     * Static version of the internal logging function.
     * Use the logDebug(), logWarning() etc. macros instead.
     *
     * If 'logger' is 0, the default logger is used.
     */
    static QTextStream & log( Logger        *logger,
                              const QString &srcFile,
                              int            srcLine,
                              const QString &srcFunction,
                              LogSeverity    severity );

    /**
     * Log a plain newline without any prefix (timestamp, source file name, line number).
     */
    void newline();
    static void newline( Logger *logger );

    /**
     * Return a timestamp string in the format used in the log file:
     * "yyyy-MM-dd hh:mm:ss.zzz"
     */
    static QString timeStamp();

    /**
     * Prefix each line of a multi-line text with 'prefix'.
     */
    static QString prefixLines( const QString &prefix, const QString &multiLineText );

    /**
     * Indent each line of a multi-line text with 'indentWith' blanks.
     */
    static QString indentLines( int indentWidth, const QString &multiLineText );

    /**
     * Set this as the default logger. That one will be used whenever log() is
     * called with a 0 'logger' argument.
     *
     * The first logger created will implicitly be the default logger until
     * another one is set with setDefaultLogger.
     */
    void setDefaultLogger();

    /**
     * Return the default logger.
     *
     * If no default logger is explicitly set, this is the first logger that
     * was created.
     */
    static Logger * defaultLogger() { return _defaultLogger; }

    /**
     * Return the QTextStream associated with this logger. Not for general use.
     */
    QTextStream & logStream() { return _logStream; }

    /**
     * Return the current log level, i.e. the severity that will actually be
     * logged. Any lower severity will be suppressed.
     *
     * Notice that due to the way C++ evaluates expressions, the runtime cost
     * will not change significantly, only the log file size:
     *
     *     logDebug() << "Result: " << myObj->result() << endl;
     *
     * Even if the log level is higher than logDebug(), this will still call
     * myObj->result() and its operator<<(). If you want to avoid that, use
     * your own 'if' around the log output:
     *
     * if ( logLevel() >= LogSeverityDebug )
     *     logDebug() ...
     */
    LogSeverity logLevel() const { return _logLevel; }

    /**
     * Set the log level.
     */
    void setLogLevel( LogSeverity newLevel ) { _logLevel = newLevel; }

    /**
     * Return the log level of the specified logger.
     *
     * If 'logger' is 0, the default logger is used.
     */
    static LogSeverity logLevel( Logger *logger );

    /**
     * Set the log level of the specified logger.
     *
     * If 'logger' is 0, the default logger is used.
     */
    static void setLogLevel( Logger *logger, LogSeverity newLevel );


private:

    static Logger * _defaultLogger;
    QFile           _logFile;
    QTextStream     _logStream;
    QFile           _nullDevice;
    QTextStream     _nullStream;
    LogSeverity     _logLevel;
};


QTextStream & operator<<( QTextStream &str, const QStringList &stringList );
QTextStream & operator<<( QTextStream &str, bool val );


#endif // Logger_h
