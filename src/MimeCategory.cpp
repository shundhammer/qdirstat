/*
 *   File name: MimeCategory.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "MimeCategory.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


MimeCategory::MimeCategory( const QString & name, const QColor & color ):
    _name( name ),
    _color( color )
{
    if ( ! _color.isValid() )
	_color = Qt::white;
}


MimeCategory::~MimeCategory()
{
    // NOP
}


void MimeCategory::addSuffix( const QString &	  rawSuffix,
			      Qt::CaseSensitivity caseSensitivity )
{
    // Normalize suffix: Remove leading "*." or "."

    QString suffix = rawSuffix.trimmed();

    if ( suffix.startsWith( "*." ) )
	suffix.remove( 0, 2 );
    else if ( suffix.startsWith( "." ) )
	suffix.remove( 0, 1 );

    if ( caseSensitivity == Qt::CaseInsensitive )
	suffix = suffix.toLower();

    // Pick the correct suffix list

    QStringList & suffixList = caseSensitivity == Qt::CaseSensitive ?
	_caseSensitiveSuffixList : _caseInsensitiveSuffixList;


    // Append suffix if not empty and not already there

    if ( ! suffix.isEmpty() && ! suffixList.contains( suffix ) )
	suffixList << suffix;
}


bool MimeCategory::isSuffixPattern( const QString & pattern )
{
    if ( ! pattern.startsWith( "*." ) )
	return false;

    QString rest = pattern.mid( 2, -1 ); // Without leading "*."

    if ( rest.contains( '*' ) ||
	 rest.contains( '?' ) ||
	 rest.contains( '[' )	) // No use to check for "]", too, if there is no "["
    {
	return false;
    }
    else
	return true;
}


void MimeCategory::addPattern( const QString &	   rawPattern,
			       Qt::CaseSensitivity caseSensitivity )
{
    QString pattern = rawPattern.trimmed();

    if ( isSuffixPattern( pattern ) )
	addSuffix( pattern, caseSensitivity );
    else
    {
	_patternList << QRegExp ( ( caseSensitivity == Qt::CaseInsensitive ?
				    pattern.toLower() : pattern ),
				  caseSensitivity, QRegExp::Wildcard );
    }
}


void MimeCategory::addPatterns( const QStringList & patterns,
				Qt::CaseSensitivity caseSensitivity )
{
    foreach ( const QString & rawPattern, patterns )
    {
	QString pattern = rawPattern.trimmed();

	if ( ! pattern.isEmpty() )
	    addPattern( pattern, caseSensitivity );
    }
}


void MimeCategory::addSuffixes( const QStringList & suffixes,
				Qt::CaseSensitivity caseSensitivity )
{
    foreach ( const QString & rawSuffix, suffixes )
    {
	QString suffix = rawSuffix.trimmed();

	if ( ! suffix.isEmpty() )
	    addSuffix( suffix, caseSensitivity );
    }
}


void MimeCategory::clear()
{
    _caseInsensitiveSuffixList.clear();
    _caseSensitiveSuffixList.clear();
    _patternList.clear();
}


QStringList MimeCategory::humanReadablePatternList( Qt::CaseSensitivity caseSensitivity )
{
    QStringList result = caseSensitivity == Qt::CaseSensitive ?
	_caseSensitiveSuffixList : _caseInsensitiveSuffixList;

    result = humanReadableSuffixList( result );
    result << humanReadablePatternList( _patternList, caseSensitivity );
#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
    result.sort();
#else
    result.sort( caseSensitivity );
#endif

    return result;
}


QStringList MimeCategory::humanReadableSuffixList( const QStringList & suffixList )
{
    QStringList result;

    foreach ( const QString & suffix, suffixList )
	result << ( "*." + suffix );

    return result;
}


QStringList MimeCategory::humanReadablePatternList( const QRegExpList & patternList,
						    Qt::CaseSensitivity caseSensitivity )
{
    QStringList result;

    foreach ( const QRegExp & pattern, patternList )
    {
	if ( pattern.caseSensitivity() == caseSensitivity )
	    result << pattern.pattern();
    }

    return result;
}
