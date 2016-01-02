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


ExcludeRule::ExcludeRule( const QRegExp & regexp ):
    _regexp( regexp )
{
    // NOP
}


ExcludeRule::ExcludeRule( const QString & regexp ):
    _regexp( QRegExp( regexp ) )
{
    // NOP
}


ExcludeRule::~ExcludeRule()
{
    // NOP
}


bool ExcludeRule::match( const QString & text )
{
    if ( text.isEmpty() )
	return false;

    return _regexp.exactMatch( text );
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
	_rules.append( rule );
}


void ExcludeRules::add( const QRegExp & regexp )
{
    ExcludeRule * rule = new ExcludeRule( regexp );
    instance()->add( rule );
}


void ExcludeRules::add( const QString & regexp )
{
    add( QRegExp( regexp ) );
}


bool ExcludeRules::match( const QString & text )
{
    _lastMatchingRule = 0;
    if ( text.isEmpty() )
	return false;

    foreach ( ExcludeRule * rule, _rules )
    {
	if ( rule->match( text ) )
	{
	    _lastMatchingRule = rule;
#if VERBOSE_EXCLUDE_MATCHES

	    logDebug() << text << " matches " << rule << endl;

#endif
	    return true;
	}
    }

    return false;
}


const ExcludeRule * ExcludeRules::matchingRule( const QString & text )
{
    if ( text.isEmpty() )
	return 0;

    foreach ( ExcludeRule * rule, _rules )
    {
	if ( rule->match( text ) )
	    return rule;
    }

    return 0;
}


void ExcludeRules::readSettings()
{
    QSettings settings;
    settings.beginGroup( "Exclude_Rules" );
    int size = settings.beginReadArray( "Rules" );

    if ( size > 0 )
    {
	clear();

	for ( int i=0; i < size; ++i )
	{
	    settings.setArrayIndex( i );
	    QString pattern    = settings.value( "Pattern"	       ).toString();
	    bool caseSensitive = settings.value( "CaseSensitive", true ).toBool();
	    int syntax = readEnumEntry( settings, "Syntax",
					QRegExp::RegExp,
					patternSyntaxMapping() );

	    QRegExp regexp( pattern,
			    caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive,
			    static_cast<QRegExp::PatternSyntax>( syntax ) );

	    if ( ! pattern.isEmpty() && regexp.isValid() )
		add( regexp );
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
    settings.beginGroup( "Exclude_Rules" );
    settings.beginWriteArray( "Rules", _rules.size() );

    for ( int i=0; i < _rules.size(); ++i )
    {
	QRegExp regexp = _rules.at(i)->regexp();
	settings.setArrayIndex( i );
	settings.setValue( "Pattern"	  , regexp.pattern() );
	settings.setValue( "CaseSensitive", regexp.caseSensitivity() == Qt::CaseSensitive );

	writeEnumEntry( settings, "Syntax",
			regexp.patternSyntax(),
			patternSyntaxMapping() );
    }

    settings.endArray();
    settings.endGroup();
}
