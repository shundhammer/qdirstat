/*
 *   File name: CleanupCollection.cpp
 *   Summary:	QDirStat classes to reclaim disk space
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMenu>
#include <QSettings>
#include <QMessageBox>

#include "CleanupCollection.h"
#include "Cleanup.h"
#include "StdCleanup.h"
#include "SettingsHelpers.h"
#include "SelectionModel.h"
#include "OutputWindow.h"
#include "Refresher.h"
#include "Logger.h"
#include "Exception.h"

#define MAX_URLS_IN_CONFIRMATION_POPUP 7

using namespace QDirStat;


CleanupCollection::CleanupCollection( SelectionModel * selectionModel,
				      QObject	     * parent ):
    QObject( parent ),
    _selectionModel( selectionModel )
{
    readSettings();

    if ( _cleanupList.isEmpty() )
	addStdCleanups();

    updateActions();

    connect( selectionModel, SIGNAL( selectionChanged() ),
	     this,	     SLOT  ( updateActions()	) );
}


CleanupCollection::~CleanupCollection()
{
    writeSettings();
    clear();
}


void CleanupCollection::add( Cleanup * cleanup )
{
    CHECK_PTR( cleanup );
    _cleanupList << cleanup;

    connect( cleanup, SIGNAL( triggered() ),
	     this,    SLOT  ( execute()	  ) );

    updateMenus();
}


void CleanupCollection::remove( Cleanup * cleanup )
{
    int index = indexOf( cleanup );

    if ( index == -1 )
    {
	logError() << "No such cleanup: " << cleanup << endl;
	return;
    }

    _cleanupList.removeAt( index );

    // No need for updateMenus() since QObject/QWidget will take care of
    // deleted actions all by itself.
}


void CleanupCollection::addStdCleanups()
{
    foreach ( Cleanup * cleanup, StdCleanup::stdCleanups( this ) )
    {
	add( cleanup );
    }
}


int CleanupCollection::indexOf( Cleanup * cleanup ) const
{
    int index = _cleanupList.indexOf( cleanup );

    if ( index == -1 )
	logError() << "Cleanup " << cleanup << " is not in this collection" << endl;

    return index;
}


Cleanup * CleanupCollection::at( int index ) const
{
    if ( index >= 0 && index < _cleanupList.size() )
	return _cleanupList.at( index );
    else
	return 0;
}


void CleanupCollection::clear()
{
    qDeleteAll( _cleanupList );
    _cleanupList.clear();

    // No need for updateMenus() since QObject/QWidget will take care of
    // deleted actions all by itself.
}


void CleanupCollection::updateActions()
{
    FileInfoSet sel = _selectionModel->selectedItems();

    bool dirSelected	  = sel.containsDir();
    bool fileSelected	  = sel.containsFile();
    bool dotEntrySelected = sel.containsDotEntry();
    bool busy             = sel.containsBusyItem();

    foreach ( Cleanup * cleanup, _cleanupList )
    {
	if ( ! cleanup->active() || sel.isEmpty() )
	    cleanup->setEnabled( false );
	else
	{
	    bool enabled = ! busy;

	    if ( dirSelected && ! cleanup->worksForDir() )
		enabled = false;

	    if ( dotEntrySelected && ! cleanup->worksForDotEntry() )
		enabled = false;

	    if ( fileSelected && ! cleanup->worksForFile() )
		enabled = false;

	    cleanup->setEnabled( enabled );
	}
    }
}


void CleanupCollection::updateMenus()
{
    _menus.removeAll( 0 ); // Remove QPointers that have become invalid

    foreach ( QMenu * menu, _menus )
    {
	if ( menu )
	{
	    // Remove all Cleanups from this menu

	    foreach ( QAction * action, menu->actions() )
	    {
		Cleanup * cleanup = dynamic_cast<Cleanup *>( action );

		if ( cleanup )
		    menu->removeAction( cleanup );
	    }

	    // Add the current cleanups in the current order
	    addToMenu( menu );
	}
    }
}


void CleanupCollection::execute()
{
    Cleanup * cleanup = qobject_cast<Cleanup *>( sender() );

    if ( ! cleanup )
    {
	logError() << "Wrong sender type: "
		   << sender()->metaObject()->className() << endl;
	return;
    }

    FileInfoSet selection = _selectionModel->selectedItems();

    if ( selection.isEmpty() )
    {
	logWarning() << "Nothing selected" << endl;
	return;
    }

    if ( cleanup->askForConfirmation() && ! confirmation( cleanup, selection ) )
    {
	logDebug() << "User declined confirmation" << endl;
	return;
    }

    emit startingCleanup( cleanup->cleanTitle() );

    OutputWindow * outputWindow = new OutputWindow( qApp->activeWindow() );
    CHECK_NEW( outputWindow );
    outputWindow->setAutoClose( cleanup->outputWindowAutoClose() );

    switch ( cleanup->outputWindowPolicy() )
    {
	case Cleanup::ShowAlways:
	    outputWindow->show();
	    break;

	case Cleanup::ShowAfterTimeout:
	    outputWindow->showAfterTimeout( cleanup->outputWindowTimeout() );
	    break;

	case Cleanup::ShowIfErrorOutput: // showOnStderr is default
	    break;

	case Cleanup::ShowNever:
	    outputWindow->setShowOnStderr( false );
	    break;
    }

    if ( cleanup->refreshPolicy() == Cleanup::RefreshThis ||
	 cleanup->refreshPolicy() == Cleanup::RefreshParent )
    {
	FileInfoSet refreshSet =
	    cleanup->refreshPolicy() == Cleanup::RefreshParent ?
	    Refresher::parents( selection ) : selection;

	Refresher * refresher = new Refresher( refreshSet, this );
	CHECK_NEW( refresher );

	connect( outputWindow, SIGNAL( lastProcessFinished( int ) ),
		 refresher,    SLOT  ( refresh()		) );
    }

    connect( outputWindow, SIGNAL( lastProcessFinished( int ) ),
	     this,	   SIGNAL( cleanupFinished    ( int ) ) );

    foreach ( FileInfo * item, selection )
    {
	if ( cleanup->worksFor( item ) )
	{
	    cleanup->execute( item, outputWindow );
	}
	else
	{
	    logWarning() << "Cleanup " << cleanup
			 << " does not work for " << item << endl;
	}
    }

    outputWindow->noMoreProcesses();
}


bool CleanupCollection::confirmation( Cleanup * cleanup, const FileInfoSet & items )
{
    QString msg;
    QString title = cleanup->cleanTitle();

    if ( items.size() == 1 ) // The most common case
    {
	FileInfo * item = items.first();

	if ( item->isDir() || item->isDotEntry() )
	    msg = tr( "%1\nin directory %2" ).arg( title ).arg( item->url() );
	else
	    msg = tr( "%1\nfor file %2" ).arg( title ).arg( item->url() );
    }
    else // Multiple items selected
    {
	// Build a list of the first couple (7 max)

	QStringList urls;

	for ( FileInfoSet::const_iterator it = items.begin();
	      it != items.end() && urls.size() <= MAX_URLS_IN_CONFIRMATION_POPUP;
	      ++it )
	{
	    urls << (*it)->url();
	}

	if ( urls.size() < items.size() ) // Only displaying part of the items?
	{
	    urls << "...";
	    urls << tr( "(%1 items total)" ).arg( items.size() );
	}


	msg = tr( "%1 for:\n\n%2" ).arg( title ).arg( urls.join( "\n" ) );
    }

    int ret = QMessageBox::question( qApp->activeWindow(),
				     tr( "Please Confirm" ), // title
				     msg );		     // text
    return ret == QMessageBox::Yes;
}



void CleanupCollection::addToMenu( QMenu * menu, bool keepUpdated )
{
    CHECK_PTR( menu );

    foreach ( Cleanup * cleanup, _cleanupList )
    {
	if ( cleanup->active() )
	    menu->addAction( cleanup );
    }

    if ( keepUpdated && ! _menus.contains( menu ) )
	_menus << menu;
}


void CleanupCollection::moveUp( Cleanup * cleanup )
{
    int oldPos = indexOf( cleanup );

    if ( oldPos > 0 )
    {
	_cleanupList.removeAt( oldPos );
	_cleanupList.insert( oldPos - 1, cleanup );
	updateMenus();
    }
}


void CleanupCollection::moveDown( Cleanup * cleanup )
{
    int oldPos = indexOf( cleanup );

    if ( oldPos < _cleanupList.size() - 1 )
    {
	_cleanupList.removeAt( oldPos );
	_cleanupList.insert( oldPos + 1, cleanup );
	updateMenus();
    }
}


void CleanupCollection::moveToTop( Cleanup * cleanup )
{
    int oldPos = indexOf( cleanup );

    if ( oldPos > 0 )
    {
	_cleanupList.removeAt( oldPos );
	_cleanupList.insert( 0, cleanup );
	updateMenus();
    }
}


void CleanupCollection::moveToBottom( Cleanup * cleanup )
{
    int oldPos = indexOf( cleanup );

    if ( oldPos < _cleanupList.size() - 1 )
    {
	_cleanupList.removeAt( oldPos );
	_cleanupList.insert( _cleanupList.size(), cleanup );
	updateMenus();
   }
}


void CleanupCollection::readSettings()
{
    QSettings settings;

    while ( ! settings.group().isEmpty() )  // ensure using toplevel settings
	settings.endGroup();

    // Find all [Cleanup_xx] groups

    QStringList cleanupGroups;

    foreach ( const QString & group, settings.childGroups() )
    {
	if ( group.startsWith( "Cleanup_" ) )
	    cleanupGroups << group;
    }

    if ( ! cleanupGroups.isEmpty() ) // Keep defaults (StdCleanups) if settings empty
    {
	clear();

	// Read all settings groups [Cleanup_xx] that were found

	foreach ( const QString & groupName, cleanupGroups )
	{
	    settings.beginGroup( groupName );

	    // Read one cleanup

	    QString command  = settings.value( "Command" ).toString();
	    QString title    = settings.value( "Title"	 ).toString();
	    QString iconName = settings.value( "Icon"	 ).toString();
	    QString hotkey   = settings.value( "Hotkey"	 ).toString();
	    QString shell    = settings.value( "Shell"	 ).toString();

	    bool active		       = settings.value( "Active"		, true	).toBool();
	    bool worksForDir	       = settings.value( "WorksForDir"		, true	).toBool();
	    bool worksForFile	       = settings.value( "WorksForFile"		, true	).toBool();
	    bool worksForDotEntry      = settings.value( "WorksForDotEntry"	, true	).toBool();
	    bool recurse	       = settings.value( "Recurse"		, false ).toBool();
	    bool askForConfirmation    = settings.value( "AskForConfirmation"	, false ).toBool();
	    bool outputWindowAutoClose = settings.value( "OutputWindowAutoClose", false ).toBool();
	    int	 outputWindowTimeout   = settings.value( "OutputWindowTimeout"	, 0	).toInt();

	    int refreshPolicy	    = readEnumEntry( settings, "RefreshPolicy",
						     Cleanup::NoRefresh,
						     Cleanup::refreshPolicyMapping() );

	    int outputWindowPolicy  = readEnumEntry( settings, "OutputWindowPolicy",
						     Cleanup::ShowAfterTimeout,
						     Cleanup::outputWindowPolicyMapping() );
	    if ( ! command.isEmpty() &&
		 ! title.isEmpty()     )
	    {
		Cleanup * cleanup = new Cleanup( command, title, this );
		CHECK_NEW( cleanup );
		add( cleanup );

		cleanup->setActive	    ( active	       );
		cleanup->setWorksForDir	    ( worksForDir      );
		cleanup->setWorksForFile    ( worksForFile     );
		cleanup->setWorksForDotEntry( worksForDotEntry );
		cleanup->setRecurse	    ( recurse	       );
		cleanup->setShell	    ( shell	       );
		cleanup->setAskForConfirmation	 ( askForConfirmation	 );
		cleanup->setOutputWindowAutoClose( outputWindowAutoClose );
		cleanup->setOutputWindowTimeout	 ( outputWindowTimeout	 );
		cleanup->setRefreshPolicy     ( static_cast<Cleanup::RefreshPolicy>( refreshPolicy ) );
		cleanup->setOutputWindowPolicy( static_cast<Cleanup::OutputWindowPolicy>( outputWindowPolicy ) );

		if ( ! iconName.isEmpty() )
		    cleanup->setIcon( iconName );

		if ( ! hotkey.isEmpty() )
		    cleanup->setShortcut( hotkey );

	    }
	    else
	    {
		logError() << "Need at least Command and Title for a cleanup" << endl;
	    }

	    settings.endGroup(); // [Cleanup_01], [Cleanup_02], ...
	}
    }
}


void CleanupCollection::writeSettings()
{
    QSettings settings;

    while ( ! settings.group().isEmpty() )  // ensure using toplevel settings
	settings.endGroup();

    // Remove all leftover cleanup descriptions:
    // Remove all settings groups starting with "Cleanup".

    foreach ( const QString & group, settings.childGroups() )
    {
	if ( group.startsWith( "Cleanup_" ) )
	    settings.remove( group );
    }

    // Using a separate group for each cleanup for better readability in the
    // file.
    //
    // QSettings arrays are hard to read and to edit if there are more than,
    // say, 2-3 entries for each array index. Plus, a user editing the file
    // would have to take care of the array count - which is very error prone.
    //
    // We are using [Cleanup_01], [Cleanup_02], ... here just because that's
    // easiest to generate automatically; upon reading, the numbers are
    // irrelevant. It's just important that each group name is
    // unique. readSettings() will happily pick up any group that starts with
    // "Cleanup_".

    for ( int i=0; i < _cleanupList.size(); ++i )
    {
	QString groupName;
	groupName.sprintf( "Cleanup_%02d", i+1 );
	settings.beginGroup( groupName );

	Cleanup * cleanup = _cleanupList.at(i);

	settings.setValue( "Command"		  , cleanup->command()		     );
	settings.setValue( "Title"		  , cleanup->title()		     );
	settings.setValue( "Active"		  , cleanup->active()		     );
	settings.setValue( "WorksForDir"	  , cleanup->worksForDir()	     );
	settings.setValue( "WorksForFile"	  , cleanup->worksForFile()	     );
	settings.setValue( "WorksForDotEntry"	  , cleanup->worksForDotEntry()	     );
	settings.setValue( "Recurse"		  , cleanup->recurse()		     );
	settings.setValue( "AskForConfirmation"	  , cleanup->askForConfirmation()    );
	settings.setValue( "OutputWindowAutoClose", cleanup->outputWindowAutoClose() );

	if ( cleanup->outputWindowTimeout() > 0 )
	    settings.setValue( "OutputWindowTimeout"  , cleanup->outputWindowTimeout()	 );

	writeEnumEntry( settings, "RefreshPolicy",
			cleanup->refreshPolicy(),
			Cleanup::refreshPolicyMapping() );

	writeEnumEntry( settings, "OutputWindowPolicy",
			cleanup->outputWindowPolicy(),
			Cleanup::outputWindowPolicyMapping() );

	if ( ! cleanup->shell().isEmpty() )
	     settings.setValue( "Shell", cleanup->shell() );

	if ( ! cleanup->iconName().isEmpty() )
	    settings.setValue( "Icon", cleanup->iconName() );

	if ( ! cleanup->shortcut().isEmpty() )
	    settings.setValue( "Hotkey" , cleanup->shortcut().toString() );

	settings.endGroup(); // [Cleanup_01], [Cleanup_02], ...
    }
}

