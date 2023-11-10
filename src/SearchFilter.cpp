/*
 *   File name: SearchFilter.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "SearchFilter.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


SearchFilter::SearchFilter( const QString & pattern,
                            FilterMode      filterMode,
                            FilterMode      defaultFilterMode ):
    _pattern( pattern ),
    _regexp( pattern ),
    _filterMode( filterMode ),
    _defaultFilterMode( defaultFilterMode )
{
    if ( _filterMode == Auto )
        guessFilterMode();

    if ( _filterMode == Wildcard )
        _regexp.setPatternSyntax( QRegExp::Wildcard );

    _regexp.setCaseSensitivity( Qt::CaseInsensitive );
}


void SearchFilter::guessFilterMode()
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
    else if ( _pattern.contains( "*.*" ) )
    {
        _filterMode = Wildcard;
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
        if ( _defaultFilterMode == Auto )
            _filterMode = StartsWith;
        else
            _filterMode = _defaultFilterMode;
    }

#if 0
    logDebug() << "using filter mode " << toString( _filterMode )
               << " from \"" << _pattern << "\""
               << endl;
#endif
}


bool SearchFilter::matches( const QString & str ) const
{
    Qt::CaseSensitivity caseSensitivity = _regexp.caseSensitivity();

    switch ( _filterMode )
    {
        case Contains:   return str.contains  ( _pattern, caseSensitivity );
        case StartsWith: return str.startsWith( _pattern, caseSensitivity );
        case ExactMatch: return QString::compare( str, _pattern, caseSensitivity ) == 0;
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


void SearchFilter::setCaseSensitive( bool sensitive )
{
    _regexp.setCaseSensitivity( sensitive ?
                                Qt::CaseSensitive : Qt::CaseInsensitive );
}


QString SearchFilter::toString( FilterMode filterMode )
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
