/*
 *   File name:	kcleanup.cpp
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdlib.h>
#include <qapplication.h>
#include <qregexp.h>

#include <kapp.h>
#include <kprocess.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kglobalsettings.h>

#include "kcleanup.h"
#include "kdirsaver.h"

#define VERBOSE_RUN_COMMAND	1
#define SIMULATE_COMMAND	0

using namespace KDirStat;


KCleanup::KCleanup( QString		id,
		    QString		command,
		    QString		title,
		    KActionCollection *	parent	)
    
    : KAction( title,
	       0,	// accel
	       parent,
	       id )
    
    , _id	( id	  )
    , _command	( command )
    , _title	( title	  )
{
    _selection		= 0;
    _enabled		= true;
    _worksForDir	= true;
    _worksForFile	= false;
    _worksForDotEntry	= false;
    _worksLocalOnly	= true;
    _recurse		= false;
    _askForConfirmation	= false;
    _refreshPolicy	= noRefresh;
    
    KAction::setEnabled( false );
}


KCleanup::KCleanup( const KCleanup &src )
    : KAction()
{
    copy( src );
}


KCleanup &
KCleanup::operator= ( const KCleanup &src )
{
    copy( src );
    
    return *this;
}


void
KCleanup::copy( const KCleanup &src )
{
    setTitle( src.title() );
    _selection		= src.selection();
    _id			= src.id();
    _command		= src.command();
    _enabled		= src.enabled();
    _worksForDir	= src.worksForDir();
    _worksForFile	= src.worksForFile();
    _worksForDotEntry	= src.worksForDotEntry();
    _worksLocalOnly	= src.worksLocalOnly();
    _recurse		= src.recurse();
    _askForConfirmation	= src.askForConfirmation();
    _refreshPolicy	= src.refreshPolicy();
}


void
KCleanup::setTitle( const QString &title )
{
    _title = title;
    KAction::setText( _title );
}


bool
KCleanup::worksFor( KFileInfo *item ) const
{
    if ( ! _enabled || ! item )
	return false;

    if ( worksLocalOnly() && ! item->tree()->isFileProtocol() )
	return false;

    if	( item->isDotEntry() )	return worksForDotEntry();
    if	( item->isDir() )	return worksForDir();
    
    return worksForFile();
}


void
KCleanup::selectionChanged( KFileInfo *selection )
{
    bool enabled = false;
    _selection = selection;

    if ( selection )
    {
	enabled = worksFor( selection );

	if ( ! selection->isFinished() )
	{
	    // This subtree isn't finished reading yet

	    switch ( _refreshPolicy )
	    {
		// Refresh policies that would cause this subtree to be deleted
		case refreshThis:
		case refreshParent:
		case assumeDeleted:

		    // Prevent premature deletion of this tree - this would
		    // cause a core dump for sure. 
		    enabled = false;
		    break;
			
		default:
		    break;
	    }
	    
	}
    }
    
    KAction::setEnabled( enabled );
}


void
KCleanup::executeWithSelection()
{
    if ( _selection )
	execute( _selection );
}


bool
KCleanup::confirmation( KFileInfo * item )
{
    QString msg;

    if ( item->isDir() || item->isDotEntry() )
    {
	msg = i18n( "%1\nin directory %2" ).arg( cleanTitle() ).arg( item->url() );
    }
    else
    {
	msg = i18n( "%1\nfor file %2" ).arg( cleanTitle() ).arg( item->url() );
    }

    if ( KMessageBox::warningContinueCancel( 0,				// parentWidget
					     msg,			// message
					     i18n( "Please Confirm" ),	// caption
					     i18n( "Confirm" )		// confirmButtonLabel
					     ) == KMessageBox::Continue )
	return true;
    else
	return false;
}


void
KCleanup::execute( KFileInfo *item )
{
    if ( worksFor( item ) )
    {
	if ( _askForConfirmation && ! confirmation( item ) )
	    return;
	    
	KDirTree  * tree = item->tree();
	
	executeRecursive( item );
	
	switch ( _refreshPolicy )
	{
	    case noRefresh:
		// Do nothing.
		break;


	    case refreshThis:
		tree->refresh( item );
		break;


	    case refreshParent:
		tree->refresh( item->parent() );
		break;


	    case assumeDeleted:

		// Assume the cleanup action has deleted the item.
		// Modify the KDirTree accordingly.

		tree->deleteSubtree( item );

		// Don't try to figure out a reasonable next selection - the
		// views have to do that while handling the subtree
		// deletion. Only the views have any knowledge about a
		// reasonable strategy for choosing a next selection. Unlike
		// the view items, the KFileInfo items don't have an order that
		// makes any sense to the user.

		break;
	}
    }

    emit executed();
}


void
KCleanup::executeRecursive( KFileInfo *item )
{
    if ( worksFor( item ) )
    {
	if ( _recurse )
	{
	    // Recurse into all subdirectories.

	    KFileInfo * subdir = item->firstChild();

	    while ( subdir )
	    {
		if ( subdir->isDir() )
		{
		    /**
		     * Recursively execute in this subdirectory, but only if it
		     * really is a directory: File children might have been
		     * reparented to the directory (normally, they reside in
		     * the dot entry) if there are no real subdirectories on
		     * this directory level.
		     **/
		    executeRecursive( subdir );
		}
		subdir = subdir->next();
	    }
	}


	// Perform cleanup for this directory.

	runCommand( item, _command );
    }
}


const QString
KCleanup::itemDir( const KFileInfo *item ) const
{
    QString dir = item->url();

    if ( ! item->isDir() && ! item->isDotEntry() )
    {
	dir.replace ( QRegExp ( "/[^/]*$" ), "" );
    }

    return dir;
}


QString
KCleanup::cleanTitle() const
{
    // Use the cleanup action's title, if possible.
   
    QString title = _title;

    if ( title.isEmpty() )
    {
	title = _id;
    }

    // Get rid of any "&" characters in the text that denote keyboard
    // shortcuts in menus.
    title.replace( QRegExp( "&" ), "" );

    return title;
}


QString
KCleanup::expandVariables( const KFileInfo *	item,
			   const QString &	unexpanded ) const
{
    QString expanded = unexpanded;

    expanded.replace( QRegExp( "%p" ),
		      "\"" + QString::fromLocal8Bit( item->url() )  + "\"" );
    expanded.replace( QRegExp( "%n" ),
		      "\"" + QString::fromLocal8Bit( item->name() ) + "\"" );

    if ( KDE::versionMajor() >= 3 && KDE::versionMinor() >= 4 )
	expanded.replace( QRegExp( "%t" ), "trash:/" );
    else
	expanded.replace( QRegExp( "%t" ), KGlobalSettings::trashPath() );

    return expanded;
}

#include <qtextcodec.h>
void
KCleanup::runCommand ( const KFileInfo *	item,
		       const QString &		command ) const
{
    KProcess	proc;
    KDirSaver	dir( itemDir( item ) );
    QString	cmd( expandVariables( item, command ));

#if VERBOSE_RUN_COMMAND
    printf( "\ncd " );
    fflush( stdout );
    system( "pwd" );
    QTextCodec * codec = QTextCodec::codecForLocale();
    printf( "%s\n", (const char *) codec->fromUnicode( cmd ) );
    fflush( stdout );
#endif

#if ! SIMULATE_COMMAND
    proc << "sh";
    proc << "-c";
    proc << cmd;

    switch ( _refreshPolicy )
    {
	case noRefresh:
	case assumeDeleted:

	    // In either case it is no use waiting for the command to
	    // finish, so we are starting the command as a pure
	    // background process.

	    proc.start( KProcess::DontCare );
	    break;


	case refreshThis:
	case refreshParent:

	    // If a display refresh is due after the command, we need to
	    // wait for the command to be finished in order to avoid
	    // performing the update prematurely, so we are starting this
	    // process in blocking mode.

	    QApplication::setOverrideCursor( waitCursor );
	    proc.start( KProcess::Block );
	    QApplication::restoreOverrideCursor();
	    break;
    }

#endif
}


void
KCleanup::readConfig()
{
    KConfig *config = kapp->config();
    KConfigGroupSaver saver( config, _id );

    bool valid = config->readBoolEntry( "valid", false	);

    // If the config section requested exists, it should contain a
    // "valid" field with a true value. If not, there is no such
    // section within the config file. In this case, just leave this
    // cleanup action undisturbed - we'd rather have a good default
    // value (as provided - hopefully - by our application upon
    // startup) than a generic empty cleanup action.
   
    if ( valid )
    {
	_command		= config->readEntry	( "command"		);
	_enabled		= config->readBoolEntry ( "enabled"		);
	_worksForDir		= config->readBoolEntry ( "worksForDir"		);
	_worksForFile		= config->readBoolEntry ( "worksForFile"	);
	_worksForDotEntry	= config->readBoolEntry ( "worksForDotEntry"	);
	_worksLocalOnly		= config->readBoolEntry ( "worksLocalOnly"	);
	_recurse		= config->readBoolEntry ( "recurse"		, false	);
	_askForConfirmation	= config->readBoolEntry ( "askForConfirmation"	, false	);
	_refreshPolicy		= (KCleanup::RefreshPolicy) config->readNumEntry( "refreshPolicy" );
	setTitle( config->readEntry( "title" ) );
    }
}


void
KCleanup::saveConfig() const
{
    KConfig *config = kapp->config();
    KConfigGroupSaver saver( config, _id );

    config->writeEntry( "valid",		true			);
    config->writeEntry( "command",		_command		);
    config->writeEntry( "title",		_title			);
    config->writeEntry( "enabled",		_enabled		);
    config->writeEntry( "worksForDir",		_worksForDir		);
    config->writeEntry( "worksForFile",		_worksForFile		);
    config->writeEntry( "worksForDotEntry",	_worksForDotEntry	);
    config->writeEntry( "worksLocalOnly",	_worksLocalOnly		);
    config->writeEntry( "recurse",		_recurse		);
    config->writeEntry( "askForConfirmation",	_askForConfirmation	);
    config->writeEntry( "refreshPolicy",	(int) _refreshPolicy	);
}


// EOF
