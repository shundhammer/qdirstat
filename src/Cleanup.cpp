/*
 *   File name: Cleanup.cpp
 *   Summary:	QDirStat classes to reclaim disk space
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QApplication>
#include <QRegExp>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>

#include "Cleanup.h"
#include "FileInfo.h"
#include "DirTree.h"
#include "DirSaver.h"
#include "OutputWindow.h"
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
    _active		   = true;
    _worksForDir	   = true;
    _worksForFile	   = false;
    _worksForDotEntry	   = false;
    _recurse		   = false;
    _askForConfirmation	   = false;
    _refreshPolicy	   = NoRefresh;
    _outputWindowPolicy	   = ShowAfterTimeout;
    _outputWindowTimeout   = 0;
    _outputWindowAutoClose = false;

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


void Cleanup::execute( FileInfo *item, OutputWindow * outputWindow )
{
    if ( worksFor( item ) )
    {
	DirTree * tree = item->tree();

	executeRecursive( item, outputWindow );

	switch ( _refreshPolicy )
	{
	    case NoRefresh:
		// Do nothing (by definition).
		break;

	    case RefreshThis:
	    case RefreshParent:
		// Done from CleanupCollection::execute() via a Refresher
		// object that is triggered by the
		// OutputWindow::lastProcessFinished() signal.
		//
		// Nothing left to do here.
		break;

	    case AssumeDeleted:

		// Assume the cleanup action has deleted the item.
		// Modify the DirTree accordingly.

		tree->deleteSubtree( item );
		break;
	}
    }
}


void Cleanup::executeRecursive( FileInfo *item, OutputWindow * outputWindow )
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
		    executeRecursive( subdir, outputWindow );
		}
		subdir = subdir->next();
	    }
	}


	// Perform cleanup for this directory.

	runCommand( item, _command, outputWindow );
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
			   OutputWindow	 * outputWindow ) const
{
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
    process->setWorkingDirectory( itemDir( item ) );
    logDebug() << "New process \"" << process << endl;

    outputWindow->addProcess( process );

    // The CleanupCollection will take care about refreshing if this is
    // configured for this cleanup.
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


QMap<int, QString> Cleanup::outputWindowPolicyMapping()
{
    QMap<int, QString> mapping;

    mapping[ ShowAlways	       ] = "ShowAlways";
    mapping[ ShowIfErrorOutput ] = "ShowIfErrorOutput";
    mapping[ ShowAfterTimeout  ] = "ShowAfterTimeout";
    mapping[ ShowNever	       ] = "ShowNever";

    return mapping;
}
