/*
 *   File name:	ExcludeRules.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "ExcludeRules.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"


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

    if ( _regexp.pattern().isEmpty() )
	return false;

    return _regexp.exactMatch( matchText );
}


//
//---------------------------------------------------------------------------
//


ExcludeRules::ExcludeRules():
    QObject(),
    _listMover( _rules )
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
    CHECK_PTR( rule );
    _rules << rule;
}


void ExcludeRules::add( const QRegExp & regexp, bool useFullPath )
{
    ExcludeRule * rule = new ExcludeRule( regexp, useFullPath );
    CHECK_NEW( rule );

    instance()->add( rule );
}


void ExcludeRules::add( const QString & regexp, bool useFullPath )
{
    add( QRegExp( regexp ), useFullPath );
}


void ExcludeRules::remove( ExcludeRule * rule )
{
    CHECK_PTR( rule );

    _rules.removeAll( rule );
    delete rule;
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


void ExcludeRules::moveUp( ExcludeRule * rule )
{
    _listMover.moveUp( rule );
}


void ExcludeRules::moveDown( ExcludeRule * rule )
{
    _listMover.moveDown( rule );
}


void ExcludeRules::moveToTop( ExcludeRule * rule )
{
    _listMover.moveToTop( rule );
}


void ExcludeRules::moveToBottom( ExcludeRule * rule )
{
    _listMover.moveToBottom( rule );
}


void ExcludeRules::readSettings()
{
    ExcludeRuleSettings settings;
    QStringList excludeRuleGroups = settings.findGroups( settings.groupPrefix() );

    if ( ! excludeRuleGroups.isEmpty() ) // Keep defaults if settings empty
    {
	clear();

	// Read all settings groups [ExcludeRule_xx] that were found

	foreach ( const QString & groupName, excludeRuleGroups )
	{
	    settings.beginGroup( groupName );

	    // Read one exclude rule

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

	    settings.endGroup(); // [ExcludeRule_01], [ExcludeRule_02], ...
	}
    }
}


void ExcludeRules::writeSettings()
{
    ExcludeRuleSettings settings;

    // Remove all leftover cleanup descriptions
    settings.removeGroups( settings.groupPrefix() );

    // Similar to CleanupCollection::writeSettings(), using a separate group
    // for each exclude rule for better readability in the settings file.

    for ( int i=0; i < _rules.size(); ++i )
    {
	ExcludeRule * rule = _rules.at(i);
	QRegExp regexp = rule->regexp();

	if ( ! regexp.pattern().isEmpty() )
	{
	    QString groupName;
	    groupName.sprintf( "ExcludeRule_%02d", i+1 );
	    settings.beginGroup( groupName );

	    settings.setValue( "Pattern",	regexp.pattern() );
	    settings.setValue( "CaseSensitive", regexp.caseSensitivity() == Qt::CaseSensitive );
	    settings.setValue( "UseFullPath",	rule->useFullPath() );

	    writeEnumEntry( settings, "Syntax",
			    regexp.patternSyntax(),
			    patternSyntaxMapping() );

	    settings.endGroup(); // [ExcludeRule_01], [ExcludeRule_02], ...
	}
    }
}
