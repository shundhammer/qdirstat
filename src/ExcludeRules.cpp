/*
 *   File name:	ExcludeRules.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "ExcludeRules.h"
#include "DirInfo.h"
#include "DotEntry.h"
#include "FileInfoIterator.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"


#define VERBOSE_EXCLUDE_MATCHES	1

using namespace QDirStat;


ExcludeRule::ExcludeRule( const QRegExp & regexp,
                          bool            useFullPath,
                          bool            checkAnyFileChild ):
    _regexp( regexp ),
    _useFullPath( useFullPath ),
    _checkAnyFileChild( checkAnyFileChild )
{
    // NOP
}


ExcludeRule::ExcludeRule( const QString & regexp,
                          bool            useFullPath,
                          bool            checkAnyFileChild ):
    _regexp( QRegExp( regexp ) ),
    _useFullPath( useFullPath ),
    _checkAnyFileChild( checkAnyFileChild )
{
    // NOP
}


ExcludeRule::~ExcludeRule()
{
    // NOP
}


bool ExcludeRule::match( const QString & fullPath, const QString & fileName )
{
    if ( _checkAnyFileChild )  // use matchDirectChildren() for those rules
        return false;

    QString matchText = _useFullPath ? fullPath : fileName;

    if ( matchText.isEmpty() )
	return false;

    if ( _regexp.pattern().isEmpty() )
	return false;

    return _regexp.exactMatch( matchText );
}


bool ExcludeRule::matchDirectChildren( DirInfo * dir )
{
    if ( ! _checkAnyFileChild || ! dir )
        return false;

    if ( _regexp.pattern().isEmpty() )
        return false;

    FileInfoIterator it( dir->dotEntry() ? dir->dotEntry() : dir );

    while ( *it )
    {
        if ( ! (*it)->isDir() )
        {
            if ( _regexp.exactMatch( (*it)->name() ) )
                return true;
        }

        ++it;
    }

    return false;
}


//
//---------------------------------------------------------------------------
//


ExcludeRules::ExcludeRules():
    QObject(),
    _listMover( _rules )
{
    _lastMatchingRule  = 0;
    _defaultRulesAdded = false;
}


ExcludeRules::ExcludeRules( const QStringList & paths ):
    QObject(),
    _listMover( _rules )
{
    _lastMatchingRule  = 0;
    _defaultRulesAdded = false;

    foreach ( const QString & path, paths )
    {
        QRegExp regexp( path );
        regexp.setPatternSyntax( QRegExp::Wildcard );
        ExcludeRule * rule = new ExcludeRule( regexp,
                                              true,    // useFullPath
                                              false ); // checkAnyFileChild
        CHECK_NEW( rule );
        _rules << rule;
    }
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


void ExcludeRules::add( const QRegExp & regexp,
                        bool            useFullPath,
                        bool            checkAnyFileChild )
{
    ExcludeRule * rule = new ExcludeRule( regexp, useFullPath, checkAnyFileChild );
    CHECK_NEW( rule );

    instance()->add( rule );
}


void ExcludeRules::add( const QString & regexp,
                        bool            useFullPath,
                        bool            checkAnyFileChild )
{
    add( QRegExp( regexp ), useFullPath, checkAnyFileChild );
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


bool ExcludeRules::matchDirectChildren( DirInfo * dir )
{
    _lastMatchingRule = 0;
    if ( ! dir )
	return false;

    foreach ( ExcludeRule * rule, _rules )
    {
	if ( rule->matchDirectChildren( dir ) )
	{
	    _lastMatchingRule = rule;
#if VERBOSE_EXCLUDE_MATCHES

	    logDebug() << dir << " matches " << rule << endl;

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


void ExcludeRules::addDefaultRules()
{
    logInfo() << "Adding default exclude rules" << endl;

    QRegExp regexp( ".snapshot", Qt::CaseSensitive, QRegExp::FixedString );
    ExcludeRule * rule = new ExcludeRule( regexp );
    CHECK_NEW( rule );
    add( rule );
    _defaultRulesAdded = true;

    logInfo() << "Added " << rule << endl;
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

	    QString pattern        = settings.value( "Pattern"	                ).toString();
	    bool caseSensitive     = settings.value( "CaseSensitive",     true  ).toBool();
	    bool useFullPath       = settings.value( "UseFullPath",	  false ).toBool();
            bool checkAnyFileChild = settings.value( "CheckAnyFileChild", false ).toBool();
	    int syntax = readEnumEntry( settings, "Syntax",
					QRegExp::RegExp,
					patternSyntaxMapping() );

	    QRegExp regexp( pattern,
			    caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive,
			    static_cast<QRegExp::PatternSyntax>( syntax ) );

	    if ( ! pattern.isEmpty() && regexp.isValid() )
		add( regexp, useFullPath, checkAnyFileChild );
	    else
	    {
		logError() << "Invalid regexp: \"" << regexp.pattern()
			   << "\": " << regexp.errorString()
			   << endl;
	    }

	    settings.endGroup(); // [ExcludeRule_01], [ExcludeRule_02], ...
	}
    }

    _defaultRulesAdded = settings.value( "DefaultExcludeRulesAdded", false ).toBool();

    if ( isEmpty() && ! _defaultRulesAdded )
        addDefaultRules();
}


void ExcludeRules::writeSettings()
{
    ExcludeRuleSettings settings;

    // Remove all leftover exclude rule descriptions
    settings.removeGroups( settings.groupPrefix() );

    // Similar to CleanupCollection::writeSettings(), using a separate group
    // for each exclude rule for better readability in the settings file.

    for ( int i=0; i < _rules.size(); ++i )
    {
	ExcludeRule * rule = _rules.at(i);
	QRegExp regexp = rule->regexp();

	if ( ! regexp.pattern().isEmpty() )
	{
	    settings.beginGroup( "ExcludeRule", i+1 );

	    settings.setValue( "Pattern",	    regexp.pattern() );
	    settings.setValue( "CaseSensitive",     regexp.caseSensitivity() == Qt::CaseSensitive );
	    settings.setValue( "UseFullPath",	    rule->useFullPath()       );
	    settings.setValue( "CheckAnyFileChild", rule->checkAnyFileChild() );

	    writeEnumEntry( settings, "Syntax",
			    regexp.patternSyntax(),
			    patternSyntaxMapping() );

	    settings.endGroup(); // [ExcludeRule_01], [ExcludeRule_02], ...
	}
    }

    if ( _defaultRulesAdded )
        settings.setValue( "DefaultExcludeRulesAdded", true );
}
