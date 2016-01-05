/*
 *   File name: Cleanup.cpp
 *   Summary:	QDirStat classes to reclaim disk space
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QApplication>
#include <QMessageBox>
#include <QRegExp>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>

#include "Cleanup.h"
#include "FileInfo.h"
#include "DirTree.h"
#include "DirSaver.h"
#include "ProcessOutput.h"
#include "Logger.h"
#include "Exception.h"


#define SIMULATE_COMMAND	1
#define WAIT_TIMEOUT_MILLISEC	30000

using namespace QDirStat;


Cleanup::Cleanup( QString   id,
		  QString   command,
		  QString   title,
		  QObject * parent ):
    QAction( title, parent ),
    _id( id ),
    _command( command ),
    _title( title )
{
    _active		= true;
    _worksForDir	= true;
    _worksForFile	= false;
    _worksForDotEntry	= false;
    _recurse		= false;
    _askForConfirmation = false;
    _refreshPolicy	= NoRefresh;

    QAction::setEnabled( true );
}


void Cleanup::setTitle( const QString &title )
{
    _title = title;
    QAction::setText( _title );
}


void Cleanup::setIcon( const QString & iconName )
{
    _iconName = iconName;
    QAction::setIcon( QPixmap( _iconName ) );
}


bool Cleanup::worksFor( FileInfo *item ) const
{
    if ( ! _active || ! item )
	return false;

    if	( item->isDotEntry() )	return worksForDotEntry();
    if	( item->isDir() )	return worksForDir();

    return worksForFile();
}


bool Cleanup::confirmation( FileInfo * item )
{
    QString msg;

    if ( item->isDir() || item->isDotEntry() )
	msg = tr( "%1\nin directory %2" ).arg( cleanTitle() ).arg( item->url() );
    else
	msg = tr( "%1\nfor file %2" ).arg( cleanTitle() ).arg( item->url() );

    int ret = QMessageBox::question( qApp->activeWindow(),
				     tr( "Please Confirm" ), // title
				     msg );		     // text
    return ret == QMessageBox::Yes;
}


void Cleanup::execute( FileInfo *item, ProcessOutput * processOutput )
{
    if ( worksFor( item ) )
    {
	if ( _askForConfirmation && ! confirmation( item ) )
	    return;

	DirTree * tree = item->tree();

	executeRecursive( item, processOutput );

	switch ( _refreshPolicy )
	{
	    case NoRefresh:
		// Do nothing.
		break;

	    case RefreshThis:

		if ( item->isDirInfo() )
		    tree->refresh( item->toDirInfo() );
		else
		    tree->refresh( item->parent() );
		break;

	    case RefreshParent:
		tree->refresh( item->parent() );
		break;

	    case AssumeDeleted:

		// Assume the cleanup action has deleted the item.
		// Modify the DirTree accordingly.

		tree->deleteSubtree( item );

		// Don't try to figure out a reasonable next selection - the
		// views have to do that while handling the subtree
		// deletion. Only the views have any knowledge about a
		// reasonable strategy for choosing a next selection. Unlike
		// the view items, the FileInfo items don't have an order that
		// makes any sense to the user.

		break;
	}
    }
}


void Cleanup::executeRecursive( FileInfo *item, ProcessOutput * processOutput )
{
    if ( worksFor( item ) )
    {
	if ( _recurse )
	{
	    // Recurse into all subdirectories.

	    FileInfo * subdir = item->firstChild();

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
		    executeRecursive( subdir, processOutput );
		}
		subdir = subdir->next();
	    }
	}


	// Perform cleanup for this directory.

	runCommand( item, _command, processOutput );
    }
}


const QString Cleanup::itemDir( const FileInfo *item ) const
{
    QString dir = item->url();

    if ( ! item->isDir() && ! item->isDotEntry() )
    {
	dir.replace ( QRegExp ( "/[^/]*$" ), "" );
    }

    return dir;
}


QString Cleanup::cleanTitle() const
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


QString Cleanup::expandVariables( const FileInfo * item,
				  const QString	 & unexpanded ) const
{
    QString expanded = unexpanded;
    expanded.replace( "%p", escapeAndQuote( item->url()	 ) );
    expanded.replace( "%n", escapeAndQuote( item->name() ) );

    logDebug() << "Expanded: \"" << expanded << "\"" << endl;
    return expanded;
}


QString Cleanup::escapeAndQuote( const QString & unescaped ) const
{
    QString escaped = unescaped;
    escaped.replace( "'", "\\'" );
    escaped = "'" + escaped + "'";

    return escaped;
}


void Cleanup::runCommand ( const FileInfo * item,
			   const QString  & command,
			   ProcessOutput  * processOutput ) const
{
    QString dir = itemDir( item );

    if ( processOutput )
	processOutput->addCommandLine( "cd " + dir );

    DirSaver dirSaver( dir );  // chdir itemdir
    QString  cleanupCommand( expandVariables( item, command ));

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString shell = env.value( "SHELL", "/bin/sh" );

    QProcess * process = new QProcess( parent() );
    CHECK_NEW( process );

    // Starting the cleanup command with the user's login shell to have
    // everything available that users are used to from their shell, including
    // their normal environment.

    process->setProgram( shell );
    process->setArguments( QStringList() << "-c" << cleanupCommand );

    if ( processOutput )
    {
	processOutput->addProcess( process );
	processOutput->addCommandLine( cleanupCommand );
    }

    logDebug() << "Starting \"" << process->program() << "\" with args \""
	       << process->arguments().join( "\", \"" ) << "\""
	       << endl;

    switch ( _refreshPolicy )
    {
	case NoRefresh:
	case AssumeDeleted:

	    // In either case it is no use waiting for the command to
	    // finish, so we are starting the command as a pure
	    // background process.

	    process->start();
	    logDebug() << "Leaving process to run in the background" << endl;
	    break;


	case RefreshThis:
	case RefreshParent:

	    // If a display refresh is due after the command, we need to
	    // wait for the command to be finished in order to avoid
	    // performing the update prematurely, so we are starting this
	    // process in blocking mode.

	    process->start();
#if 0
	    QApplication::setOverrideCursor( Qt::WaitCursor );
	    logDebug() << "Waiting for process to finish..." << endl;
	    process->waitForFinished( WAIT_TIMEOUT_MILLISEC );
	    logDebug() << "Process finished" << endl;
	    QApplication::restoreOverrideCursor();
#endif
	    break;
    }
}


QMap<int, QString> Cleanup::refreshPolicyMapping()
{
    QMap<int, QString> mapping;

    mapping[ NoRefresh	   ] = "NoRefresh";
    mapping[ RefreshThis   ] = "RefreshThis";
    mapping[ RefreshParent ] = "RefreshParent";
    mapping[ AssumeDeleted ] = "AssumeDeleted";

    return mapping;
}

