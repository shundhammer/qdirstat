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
	     << moveToTrash	 ( parent )
	     << hardDelete	 ( parent )
	     << compressSubtree	 ( parent )
	     << makeClean	 ( parent )
	     << deleteJunk	 ( parent )
	     << echoargs	 ( parent )
	     << pwd		 ( parent )
	;

    return cleanups;
}



Cleanup * StdCleanup::openInFileManager( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "cleanupOpenInFileManager",
				    "xdg-open %p",
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
    Cleanup *cleanup = new Cleanup( "cleanupOpenInTerminal",
				    "x-terminal-emulator --workdir %p",
				    QObject::tr( "Open in &Terminal" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true );
    cleanup->setWorksForFile	( false );
    cleanup->setWorksForDotEntry( true );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );
    cleanup->setIcon( ":/icons/terminal.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_T );

    return cleanup;
}


Cleanup * StdCleanup::compressSubtree( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "cleanupCompressSubtree",
				    "cd ..; tar cjvf %n.tar.bz2 %n && rm -rf %n",
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
    Cleanup *cleanup = new Cleanup( "cleanupMakeClean",
				    "make clean",
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
    Cleanup *cleanup = new Cleanup( "cleanupDeleteJunk",
				    "rm -f *.o *~ *.bak *.auto core",
				    QObject::tr( "Delete T&rash Files" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( false );
    cleanup->setWorksForDotEntry( true	);
    cleanup->setRefreshPolicy( Cleanup::RefreshThis );
    cleanup->setRecurse( true );

    return cleanup;
}


Cleanup * StdCleanup::moveToTrash( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "cleanupMoveToTrash",
				    "kfmclient move %p %t",
				    QObject::tr( "Delete (to Trash &Bin)" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( true	);
    cleanup->setWorksForDotEntry( false );
    cleanup->setRefreshPolicy( Cleanup::AssumeDeleted );
    cleanup->setIcon( ":/icons/trash.png" );
    cleanup->setShortcut( Qt::Key_Delete );

    return cleanup;
}


Cleanup * StdCleanup::hardDelete( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "cleanupHardDelete",
				    "rm -rf %p",
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


Cleanup * StdCleanup::echoargs( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "cleanupEchoargs",
				    "echoargs %p",
				    QObject::tr( "echoargs" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( true	);
    cleanup->setWorksForDotEntry( true );
    cleanup->setAskForConfirmation( true );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );

    return cleanup;
}


Cleanup * StdCleanup::pwd( QObject * parent )
{
    Cleanup *cleanup = new Cleanup( "cleanupPwd",
				    "pwd",
				    QObject::tr( "pwd" ),
				    parent );
    CHECK_NEW( cleanup );
    cleanup->setWorksForDir	( true	);
    cleanup->setWorksForFile	( true	);
    cleanup->setWorksForDotEntry( true );
    cleanup->setAskForConfirmation( true );
    cleanup->setRefreshPolicy( Cleanup::NoRefresh );

    return cleanup;
}

