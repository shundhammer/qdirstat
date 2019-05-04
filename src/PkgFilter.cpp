/*
 *   File name: PkgFilter.h
 *   Summary:	Package manager Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "PkgFilter.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


PkgFilter::PkgFilter( const QString & pattern,
                      FilterMode      filterMode ):
    _pattern( pattern ),
    _regexp( pattern ),
    _filterMode( filterMode )
{
    normalizePattern();

    if ( _filterMode == Auto )
        guessFilterMode();

    if ( _filterMode == Wildcard )
        _regexp.setPatternSyntax( QRegExp::Wildcard );

    _regexp.setCaseSensitivity( Qt::CaseInsensitive );
}


PkgFilter::PkgFilter( const QRegExp & regexp ):
    _pattern( regexp.pattern() ),
    _regexp( regexp )
{
    switch ( regexp.patternSyntax() )
    {
        case QRegExp::Wildcard:
        case QRegExp::WildcardUnix:
            _filterMode = Wildcard;
            break;

        default:
            _filterMode = RegExp;
            break;
    }
}


void PkgFilter::normalizePattern()
{
    QString oldPattern = _pattern;
    _pattern.remove( QRegExp( "^Pkg:/?", Qt::CaseInsensitive ) );
    _pattern.remove( QRegExp( "/.*$" ) );

    if ( _pattern != oldPattern )
    {
        logInfo() << "Normalizing pkg pattern to \"" << _pattern << "\"" << endl;
        _regexp.setPattern( _pattern );
    }
}


void PkgFilter::guessFilterMode()
{
    if ( _pattern.isEmpty() )
    {
        _filterMode = SelectAll;
    }
    else if ( _pattern.startsWith( "=" ) )
    {
        _filterMode = ExactMatch;
        _pattern.remove( QRegExp( "^=" ) );
        _regexp.setPattern( _pattern );
    }
    else if ( _pattern.contains( ".*" ) ||
              _pattern.contains( "^"  ) ||
              _pattern.contains( "$"  ) ||
              _pattern.contains( "("  ) ||
              _pattern.contains( "|"  ) ||
              _pattern.contains( "["  )   )
    {
        _filterMode = RegExp;
    }
    else if ( _pattern.contains( "*" ) ||
              _pattern.contains( "?" )   )
    {
        _filterMode = Wildcard;
    }
    else
    {
        _filterMode = StartsWith;
    }

#if 0
    logDebug() << "using filter mode " << toString( _filterMode )
               << " from \"" << _pattern << "\""
               << endl;
#endif
}


bool PkgFilter::matches( const QString & str ) const
{
    switch ( _filterMode )
    {
        case Contains:   return str.contains  ( _pattern, Qt::CaseInsensitive );
        case StartsWith: return str.startsWith( _pattern, Qt::CaseInsensitive );
        case ExactMatch: return QString::compare( str, _pattern, Qt::CaseInsensitive ) == 0;
        case Wildcard:   return _regexp.exactMatch( str );
        case RegExp:     return str.contains( _regexp );
        case SelectAll:  return true;
        case Auto:
            logWarning() << "Unexpected filter mode 'Auto' - assuming 'Contains'" << endl;
            return str.contains( _pattern );
    }

    logError() << "Undefined filter mode " << (int) _filterMode << endl;
    return false;
}


QString PkgFilter::toString( FilterMode filterMode )
{
    switch ( filterMode )
    {
        case Contains:   return "Contains";
        case StartsWith: return "StartsWith";
        case ExactMatch: return "ExactMatch";
        case Wildcard:   return "Wildcard";
        case RegExp:     return "Regexp";
        case SelectAll:  return "SelectAll";
        case Auto:       return "Auto";
    }

    return QString( "<Unknown FilterMode %1" ).arg( (int) filterMode );
}


bool PkgFilter::isPkgUrl( const QString & url )
{
    return url.startsWith( "Pkg:", Qt::CaseInsensitive );
}


QString PkgFilter::url() const
{
    return QString( "Pkg:/%1" ).arg( _pattern );
}
