/*
 *   File name: LogStream.h
 *   Summary:   Logger helper class for QDirStat
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:    Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef LogStream_h
#define LogStream_h

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>



/**
 * Stream class for the logger as a base for overloaded stream output operators
 * without predefined operator<<() getting in the way all the time.
 *
 * Neither QTextStream nor std::ofstream are meant to be inherited from, and
 * they both offer a vast number of overloads already that tend to produce
 * "ambiguous overload" compiler errors if you want to overload your own
 * versions.
 *
 * This class uses composition, not inheritance to avoid this problem.
 **/
class LogStream
{
public:

    /**
     * Constructor. All log output will go to stderr until a file is opened.
     **/
    LogStream();

    /**
     * Destructor. This will also close the log if one is open.
     **/
    virtual ~LogStream();

    /**
     * Open 'filename' as the new log. Return 'true' if success, 'false' if error.
     **/
    bool open( const QString & filename );

    /**
     * Return 'true' if the log is open, 'false' if not.
     **/
    bool isOpen() const { return _logFile.isOpen(); }

    /**
     * Close the current log if it is open.
     **/
    void close();

    /**
     * Return the internally used stream.
     * This can be used in overloaded operator<<() methods.
     **/
    QTextStream & str() { return _str; }

    /**
     * Return the file name of the current log file.
     **/
    QString logFileName() const { return _logFile.fileName(); }

    /**
     * operator<<() overload to call a stream manipulator like endl
     *
     * The 'func' parameter is a function that receives a LogStream & and
     * returns a LogStream & like this:
     *
     *   LogStream & manipulator ( LogStream & str );
     **/
    LogStream & operator<<( LogStream & (*func)( LogStream & str ) );


protected:

    QTextStream _str;
    QFile       _logFile;

}; // class LogStream


// namespace for LogStream stream manipulators
namespace LogStr
{
    /**
     * 'endl' stream manipulator: Output a newline and flush the stream.
     **/
    LogStream & endl( LogStream & str );

    /**
     * 'hex' stream manipulator: Switch numeric output to hexadecimal.
     **/
    LogStream & hex( LogStream & str );

    /**
     * 'dec' stream manipulator: Switch numeric output to decimal.
     **/
    LogStream & dec( LogStream & str );
};


//----------------------------------------------------------------------


template<typename T>
LogStream & logStreamOut( LogStream & str, T arg )
{
    str.str() << arg;
    return str;
}


// Bare essential overloads

LogStream & operator<<( LogStream & str, const QString     & text );
LogStream & operator<<( LogStream & str, const char *        text );
LogStream & operator<<( LogStream & str, const std::string & text );
LogStream & operator<<( LogStream & str, const QStringList & stringList );

LogStream & operator<<( LogStream & str, int           val );
LogStream & operator<<( LogStream & str, unsigned      val );
LogStream & operator<<( LogStream & str, long          val );
LogStream & operator<<( LogStream & str, unsigned long val );
LogStream & operator<<( LogStream & str, long long     val );
LogStream & operator<<( LogStream & str, double        val );
LogStream & operator<<( LogStream & str, bool val );
LogStream & operator<<( LogStream & str, char val );



// More Qt types

class QSizeF;

LogStream & operator<<( LogStream & str, const QSizeF & size );


#endif // LogStream_h
