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
    SearchFilter( pattern,
                  filterMode,
                  StartsWith )  // defaultFilterMode
{
    normalizePattern();

    if ( _filterMode == Auto )
        guessFilterMode();

    if ( _filterMode == Wildcard )
        _regexp.setPatternSyntax( QRegExp::Wildcard );

    setCaseSensitive( _filterMode == ExactMatch );
}


void PkgFilter::normalizePattern()
{
    QString oldPattern = _pattern;
    _pattern.remove( QRegExp( "^Pkg:/*", Qt::CaseInsensitive ) );
    _pattern.remove( QRegExp( "/.*$" ) );

    if ( _pattern != oldPattern )
    {
        logInfo() << "Normalizing pkg pattern to \"" << _pattern << "\"" << endl;
        _regexp.setPattern( _pattern );
    }
}


bool PkgFilter::isPkgUrl( const QString & url )
{
    return url.startsWith( "Pkg:", Qt::CaseInsensitive );
}


QString PkgFilter::url() const
{
    return QString( "Pkg:/%1" ).arg( _pattern );
}
