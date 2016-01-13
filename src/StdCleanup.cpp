/*
 *   File name: StdCleanup.cpp
 *   Summary:	QDirStat classes to reclaim disk space
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Cleanup.h"
#include "StdCleanup.h"
#include "Exception.h"
#include "Logger.h"

using namespace QDirStat;


CleanupList StdCleanup::stdCleanups( QObject * parent )
{
    CleanupList cleanups;

    cleanups << openInFileManager( parent )
	     << openInTerminal	 ( parent )
	     << hardDelete	 ( parent )
	     << compressSubtree	 ( parent )
	     << makeClean	 ( parent )
	     << deleteJunk	 ( parent )
#if USE_DEBUG_ACTIONS
	     << echoargs	 ( parent )
	     << echoargsMixed	 ( parent )
	     << segfaulter	 ( parent )
	     << commandNotFound	 ( parent )
	     << sleepy		 ( parent )
#endif
	;

    return cleanups;
}



Cleanup * StdCleanup::openInFileManager( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "xdg-open %p",
				    QObject::tr( "Open in &File Manager" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true );
    cleanup->setWorksForFile	( true );
    cleanup->setWorksForDotEntry( true );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );
    cleanup->setIcon( ":/icons/file-manager.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_K );

    return cleanup;
}


Cleanup * StdCleanup::openInTerminal( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "x-terminal-emulator --workdir %p",
				    QObject::tr( "Open in &Terminal" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true );
    cleanup->setWorksForFile	( false );
    cleanup->setWorksForDotEntry( true );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );
    cleanup->setIcon( ":/icons/terminal.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_T );
    cleanup->setOutputWindowPolicy( Cleanup::ShowNever );

    return cleanup;
}


Cleanup * StdCleanup::compressSubtree( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "cd ..; tar cjvf %n.tar.bz2 %n && rm -rf %n",
				    QObject::tr( "&Compress" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( false );
    cleanup->setWorksForDotEntry( false );
    cleanup->setRefreshPolicy( Cleanup::RefreshParent );

    return cleanup;
}


Cleanup * StdCleanup::makeClean( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "make clean",
				    QObject::tr( "&make clean" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( false );
    cleanup->setWorksForDotEntry( true	);
    cleanup->setRefreshPolicy( Cleanup::RefreshThis );

    return cleanup;
}


Cleanup * StdCleanup::deleteJunk( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "rm -f *.o *~ *.bak *.auto core",
				    QObject::tr( "Delete &Junk Files" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( false );
    cleanup->setWorksForDotEntry( true	);
    cleanup->setRefreshPolicy( Cleanup::RefreshThis );
    cleanup->setRecurse( true );
    cleanup->setShell( "/bin/bash" );

    return cleanup;
}


Cleanup * StdCleanup::hardDelete( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "rm -rf %p",
				    QObject::tr( "&Delete (no way to undelete!)" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( true	);
    cleanup->setWorksForDotEntry( false );
    cleanup->setAskForConfirmation( true );
    cleanup->setRefreshPolicy( Cleanup::AssumeDeleted );
    cleanup->setIcon( ":/icons/delete.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_Delete );

    return cleanup;
}


#if USE_DEBUG_ACTIONS

Cleanup * StdCleanup::echoargs( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "echoargs %p",
				    QObject::tr( "echoargs" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( true	);
    cleanup->setWorksForDotEntry( true );
    cleanup->setAskForConfirmation( false );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );

    return cleanup;
}


Cleanup * StdCleanup::echoargsMixed( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "echoargs_mixed %n one two three four",
				    QObject::tr( "Output on stdout and stderr" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( true	);
    cleanup->setWorksForDotEntry( true );
    cleanup->setAskForConfirmation( true );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );

    return cleanup;
}


Cleanup * StdCleanup::segfaulter( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "segfaulter",
				    QObject::tr( "Segfaulter" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( true	);
    cleanup->setWorksForDotEntry( true );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );

    return cleanup;
}


Cleanup * StdCleanup::commandNotFound( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "wrglbrmpf",
				    QObject::tr( "Nonexistent command" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( true	);
    cleanup->setWorksForDotEntry( true );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );

    return cleanup;
}


Cleanup * StdCleanup::sleepy( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "sleep 1; echoargs %p",
				    QObject::tr( "Sleepy echoargs" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( true	);
    cleanup->setWorksForDotEntry( true );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );

    return cleanup;
}


#endif
