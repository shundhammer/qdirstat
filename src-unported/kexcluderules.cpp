/*
 *   File name:	kexcluderules.cpp
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2008-12-18
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <kdebug.h>
#include "kexcluderules.h"

#define VERBOSE_EXCLUDE_MATCHES	1

using namespace KDirStat;


KExcludeRule::KExcludeRule( const QRegExp & regexp )
    : _regexp( regexp )
    , _enabled( true )
{
    // NOP
}


KExcludeRule::~KExcludeRule()
{
    // NOP
}


bool
KExcludeRule::match( const QString & text )
{
    if ( text.isEmpty() || ! _enabled )
	return false;

    return _regexp.exactMatch( text );
}



KExcludeRules::KExcludeRules()
{
    // Make the rules list automatically delete
    _rules.setAutoDelete( true );
}


KExcludeRules::~KExcludeRules()
{
    // Do not try to delete the rules here: The rules list will automatically
    // do that since it has autoDelete enabled.
}


KExcludeRules * KExcludeRules::excludeRules()
{
    static KExcludeRules * singleton = 0;

    if ( ! singleton )
    {
	singleton = new KExcludeRules();
    }

    return singleton;
}


void KExcludeRules::add( KExcludeRule * rule )
{
    if ( rule )
	_rules.append( rule );
}


bool
KExcludeRules::match( const QString & text )
{
    if ( text.isEmpty() )
	return false;

    KExcludeRule * rule = _rules.first();

    while ( rule )
    {
	if ( rule->match( text ) )
	{
#if VERBOSE_EXCLUDE_MATCHES
	    
	    kdDebug() << text << " matches exclude rule "
		      << rule->regexp().pattern()
		      << endl;

#endif
	    return true;
	}

	rule = _rules.next();
    }

    return false;
}


const KExcludeRule *
KExcludeRules::matchingRule( const QString & text )
{
    if ( text.isEmpty() )
	return false;

    KExcludeRule * rule = _rules.first();

    while ( rule )
    {
	if ( rule->match( text ) )
	    return rule;

	rule = _rules.next();
    }

    return 0;
}


// EOF
