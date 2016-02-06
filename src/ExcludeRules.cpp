/*
 *   File name:	ExcludeRules.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QSettings>

#include "SettingsHelpers.h"
#include "Logger.h"
#include "ExcludeRules.h"


#define VERBOSE_EXCLUDE_MATCHES	1

using namespace QDirStat;


ExcludeRule::ExcludeRule( const QRegExp & regexp, bool useFullPath ):
    _regexp( regexp ),
    _useFullPath( useFullPath )
{
    // NOP
}


ExcludeRule::ExcludeRule( const QString & regexp, bool useFullPath ):
    _regexp( QRegExp( regexp ) ),
    _useFullPath( useFullPath )
{
    // NOP
}


ExcludeRule::~ExcludeRule()
{
    // NOP
}


bool ExcludeRule::match( const QString & fullPath, const QString & fileName )
{
    QString matchText = _useFullPath ? fullPath : fileName;

    if ( matchText.isEmpty() )
	return false;

    return _regexp.exactMatch( matchText );
}



ExcludeRules::ExcludeRules()
{
    _lastMatchingRule = 0;
}


ExcludeRules::~ExcludeRules()
{
    clear();
}



ExcludeRules * ExcludeRules::instance()
{
    static ExcludeRules * singleton = 0;

    if ( ! singleton )
    {
	singleton = new ExcludeRules();
    }

    return singleton;
}


void ExcludeRules::clear()
{
    qDeleteAll( _rules );
    _rules.clear();
    _lastMatchingRule = 0;
}


void ExcludeRules::add( ExcludeRule * rule )
{
    if ( rule )
	_rules << rule;
}


void ExcludeRules::add( const QRegExp & regexp, bool useFullPath )
{
    ExcludeRule * rule = new ExcludeRule( regexp, useFullPath );
    instance()->add( rule );
}


void ExcludeRules::add( const QString & regexp, bool useFullPath )
{
    add( QRegExp( regexp ), useFullPath );
}


bool ExcludeRules::match( const QString & fullPath, const QString & fileName )
{
    _lastMatchingRule = 0;
    if ( fullPath.isEmpty() || fileName.isEmpty() )
	return false;

    foreach ( ExcludeRule * rule, _rules )
    {
	if ( rule->match( fullPath, fileName ) )
	{
	    _lastMatchingRule = rule;
#if VERBOSE_EXCLUDE_MATCHES

	    logDebug() << fullPath << " matches " << rule << endl;

#endif
	    return true;
	}
    }

    return false;
}


const ExcludeRule * ExcludeRules::matchingRule( const QString & fullPath,
						const QString & fileName )
{
    if ( fullPath.isEmpty() || fileName.isEmpty() )
	return 0;

    foreach ( ExcludeRule * rule, _rules )
    {
	if ( rule->match( fullPath, fileName ) )
	    return rule;
    }

    return 0;
}


void ExcludeRules::readSettings()
{
    QSettings settings;
    settings.beginGroup( "ExcludeRules" );
    int size = settings.beginReadArray( "Rules" );

    if ( size > 0 )
    {
	clear();

	for ( int i=0; i < size; ++i )
	{
	    settings.setArrayIndex( i );
	    QString pattern    = settings.value( "Pattern"	       ).toString();
	    bool caseSensitive = settings.value( "CaseSensitive", true ).toBool();
	    bool useFullPath   = settings.value( "UseFullPath",	 false ).toBool();
	    int syntax = readEnumEntry( settings, "Syntax",
					QRegExp::RegExp,
					patternSyntaxMapping() );

	    QRegExp regexp( pattern,
			    caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive,
			    static_cast<QRegExp::PatternSyntax>( syntax ) );

	    if ( ! pattern.isEmpty() && regexp.isValid() )
		add( regexp, useFullPath );
	    else
	    {
		logError() << "Invalid regexp: \"" << regexp.pattern()
			   << "\": " << regexp.errorString()
			   << endl;
	    }
	}
    }

    settings.endArray();
    settings.endGroup();
}


void ExcludeRules::writeSettings()
{
    QSettings settings;
    settings.beginGroup( "ExcludeRules" );
    settings.beginWriteArray( "Rules", _rules.size() );

    for ( int i=0; i < _rules.size(); ++i )
    {
	ExcludeRule * rule = _rules.at(i);
	QRegExp regexp = rule->regexp();
	settings.setArrayIndex( i );
	settings.setValue( "Pattern"	  , regexp.pattern() );
	settings.setValue( "CaseSensitive", regexp.caseSensitivity() == Qt::CaseSensitive );
	settings.setValue( "UseFullPath",   rule->useFullPath() );

	writeEnumEntry( settings, "Syntax",
			regexp.patternSyntax(),
			patternSyntaxMapping() );
    }

    settings.endArray();
    settings.endGroup();
}
