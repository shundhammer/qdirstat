/*
 *   File name: DirTreePatternFilter.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DirTreePatternFilter.h"
#include "Exception.h"
#include "Logger.h"

#define VERBOSE_MATCH	0


using namespace QDirStat;


DirTreeFilter * DirTreePatternFilter::create( const QString & pattern )
{
    if ( pattern.isEmpty() )
	return 0;

    DirTreeFilter * filter = 0;

    if ( pattern.startsWith( "*." ) )
    {
	QString suffix = pattern;
	suffix.remove( 0, 1 ); // Remove the leading "*"

	if ( QRegExp( "^\\.[a-zA-Z0-9]+" ).exactMatch( suffix ) )
	    filter = new DirTreeSuffixFilter( suffix );
    }

    if ( ! filter )
	filter = new DirTreePatternFilter( pattern );

    CHECK_NEW( filter );
    return filter;
}


DirTreePatternFilter::DirTreePatternFilter( const QString & pattern ):
    _pattern( pattern )
{
    QString pat = _pattern.contains( "/" ) ?
	_pattern :
	QString( "*/" ) + _pattern;

    logDebug() << "Creating pattern filter matching against " << pat << endl;
    _regExp = QRegExp( pat, Qt::CaseSensitive, QRegExp::Wildcard );
}


DirTreePatternFilter::~DirTreePatternFilter()
{

}


bool DirTreePatternFilter::ignore( const QString & path ) const
{
    bool match = _regExp.exactMatch( path );

#if VERBOSE_MATCH
    if ( match )
    {
	logDebug() << "Ignoring " << path << " by pattern filter *" << _pattern << endl;
    }
#endif

    return match;
}





DirTreeSuffixFilter::DirTreeSuffixFilter( const QString & suffix ):
    _suffix( suffix )
{
    logDebug() << "Creating suffix filter matching *" << suffix << endl;
}


DirTreeSuffixFilter::~DirTreeSuffixFilter()
{

}


bool DirTreeSuffixFilter::ignore( const QString & path ) const
{
    bool match = path.endsWith( _suffix );

#if VERBOSE_MATCH
    if ( match )
    {
	logDebug() << "Ignoring " << path << " by suffix filter *" << _suffix << endl;
    }
#endif

    return match;
}
