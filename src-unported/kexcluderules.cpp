/*
 *   File name:	kexcluderules.cpp
 *   Summary:	Support classes for QDirStat
 *   License:   GPL V2 - See file LICENSE for details.
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 *
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "Logger.h"
#include "kexcluderules.h"

#define VERBOSE_EXCLUDE_MATCHES	1

using namespace QDirStat;


ExcludeRule::KExcludeRule( const QRegExp & regexp )
    : _regexp( regexp )
    , _enabled( true )
{
    // NOP
}


ExcludeRule::~KExcludeRule()
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
    // Make the rules list automatically delete
    _rules.setAutoDelete( true );
}


ExcludeRules::~ExcludeRules()
{
    // Do not try to delete the rules here: The rules list will automatically
    // do that since it has autoDelete enabled.
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

    ExcludeRule * rule = _rules.first();

    while ( rule )
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

	rule = _rules.next();
    }

    return false;
}


const ExcludeRule *
ExcludeRules::matchingRule( const QString & text )
{
    if ( text.isEmpty() )
	return false;

    ExcludeRule * rule = _rules.first();

    while ( rule )
    {
	if ( rule->match( text ) )
	    return rule;

	rule = _rules.next();
    }

    return 0;
}


// EOF
