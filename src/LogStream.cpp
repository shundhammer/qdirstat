/*
 *   File name: LogStream.cpp
 *   Summary:   Logger helper class for QDirStat
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:    Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "LogStream.h"


LogStream::LogStream():
    _str( stderr, QIODevice::WriteOnly )
{

}


LogStream::~LogStream()
{
    close();
}


bool LogStream::open( const QString & filename )
{
    bool success = true;

    if ( ! isOpen() || logFileName() != filename )
    {
        QIODevice::OpenMode openMode = QIODevice::WriteOnly | QIODevice::Text;

        if ( filename != "/dev/null" && filename != "/dev/stderr" )
            openMode |= QIODevice::Append;

        _logFile.setFileName( filename );

        if ( _logFile.open( openMode ) )
        {
            _str.setDevice( &_logFile );
        }
        else
        {
            success = false;
        }
    }

    return success;
}


void LogStream::close()
{
    if ( _logFile.isOpen() )
        _logFile.close();
}


LogStream & LogStream::operator<<( LogStream & (*func)( LogStream & str ) )
{
    func( *this );

    return *this;
}


namespace LogStr
{
    LogStream & endl( LogStream & str )
    {
        str.str() << '\n';
        str.str().flush();

        return str;
    }
}


LogStream & operator<<( LogStream & str, const QString & text )
{
    return logStreamOut<const QString &>( str, text );
}


LogStream & operator<<( LogStream & str, const char * text )
{
    str.str() << QString( text );
    return str;
}


LogStream & operator<<( LogStream & str, const std::string & text )
{
    str.str() << QString::fromUtf8( text.c_str() );
    return str;
}


LogStream & operator<<( LogStream & str, const QStringList & stringList )
{
    str.str() << stringList.join( ", " );
    return str;
}


LogStream & operator<<( LogStream & str, int val )
{
    return logStreamOut<int>( str, val );
}


LogStream & operator<<( LogStream & str, unsigned val )
{
    return logStreamOut<unsigned>( str, val );
}


LogStream & operator<<( LogStream & str, long val )
{
    return logStreamOut<long>( str, val );
}


LogStream & operator<<( LogStream & str, unsigned long val )
{
    return logStreamOut<unsigned long>( str, val );
}


LogStream & operator<<( LogStream & str, double val )
{
    return logStreamOut<double>( str, val );
}


LogStream & operator<<( LogStream & str, bool val )
{
    str.str() << ( val ? "true" : "false" );
    return str;
}


LogStream & operator<<( LogStream & str, char val )
{
    return logStreamOut<char>( str, val );
}

