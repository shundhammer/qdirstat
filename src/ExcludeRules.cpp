/*
 *   File name:	ExcludeRules.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Logger.h"
#include "ExcludeRules.h"

#define VERBOSE_EXCLUDE_MATCHES	1

using namespace QDirStat;


ExcludeRule::ExcludeRule( const QRegExp & regexp )
    : _regexp( regexp )
    , _enabled( true )
{
    // NOP
}


ExcludeRule::~ExcludeRule()
{
    // NOP
}


bool
ExcludeRule::match( const QString & text )
{
    if ( text.isEmpty() || ! _enabled )
	return false;

    return _regexp.exactMatch( text );
}



ExcludeRules::ExcludeRules()
{
}


ExcludeRules::~ExcludeRules()
{
    clear();
}



ExcludeRules * ExcludeRules::excludeRules()
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
}


void ExcludeRules::add( ExcludeRule * rule )
{
    if ( rule )
	_rules.append( rule );
}


bool
ExcludeRules::match( const QString & text )
{
    if ( text.isEmpty() )
	return false;

    foreach ( ExcludeRule * rule, _rules )
    {
	if ( rule->match( text ) )
	{
#if VERBOSE_EXCLUDE_MATCHES

	    logDebug() << text << " matches exclude rule "
		      << rule->regexp().pattern()
		      << endl;

#endif
	    return true;
	}
    }

    return false;
}


const ExcludeRule *
ExcludeRules::matchingRule( const QString & text )
{
    if ( text.isEmpty() )
	return false;

    foreach ( ExcludeRule * rule, _rules )
    {
	if ( rule->match( text ) )
	    return rule;
    }

    return 0;
}

