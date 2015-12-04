/*
 *   File name:	StdCleanup.cpp
 *   Summary:	Support classes for QDirStat
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <klocale.h>
#include "Cleanup.h"
#include "StdCleanup.h"

using namespace QDirStat;


Cleanup *
StdCleanup::openInKonqueror( KActionCollection *parent )
{
    Cleanup *cleanup = new KCleanup( "cleanup_open_in_konqueror",
				      "kfmclient openURL %p",
				      i18n( "Open in &Konqueror" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true );
    cleanup->setWorksForFile    ( true );
    cleanup->setWorksForDotEntry( true );
    cleanup->setWorksLocalOnly	( false );
    cleanup->setRefreshPolicy( Cleanup::noRefresh );
    cleanup->setIcon( "konqueror.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_K );
    
    return cleanup;
}


Cleanup *
StdCleanup::openInTerminal( KActionCollection *parent )
{
    Cleanup *cleanup = new KCleanup( "cleanup_open_in_terminal",
				      "konsole",
				      i18n( "Open in &Terminal" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true );
    cleanup->setWorksForFile    ( true );
    cleanup->setWorksForDotEntry( true );
    cleanup->setRefreshPolicy( Cleanup::noRefresh );
    cleanup->setIcon( "konsole.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_T );

    return cleanup;
}


Cleanup *
StdCleanup::compressSubtree( KActionCollection *parent )
{
    Cleanup *cleanup = new KCleanup( "cleanup_compress_subtree",
				      "cd ..; tar cjvf %n.tar.bz2 %n && rm -rf %n",
				      i18n( "&Compress" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( false );
    cleanup->setWorksForDotEntry( false );
    cleanup->setRefreshPolicy( Cleanup::refreshParent );
    cleanup->setIcon( "ark.png" );

    return cleanup;
}


Cleanup *
StdCleanup::makeClean( KActionCollection *parent )
{
    Cleanup *cleanup = new KCleanup( "cleanup_make_clean",
				      "make clean",
				      i18n( "&make clean" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( false );
    cleanup->setWorksForDotEntry( true  );
    cleanup->setRefreshPolicy( Cleanup::refreshThis );

    return cleanup;
}


Cleanup *
StdCleanup::deleteTrash( KActionCollection *parent )
{
    Cleanup *cleanup = new KCleanup( "cleanup_delete_trash",
				      "rm -f *.o *~ *.bak *.auto core",
				      i18n( "Delete T&rash Files" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( false );
    cleanup->setWorksForDotEntry( true  );
    cleanup->setRefreshPolicy( Cleanup::refreshThis );
    cleanup->setRecurse( true );

    return cleanup;
}


Cleanup *
StdCleanup::moveToTrashBin( KActionCollection *parent )
{
    Cleanup *cleanup = new KCleanup( "cleanup_move_to_trash_bin",
				      "kfmclient move %p %t",
				      i18n( "Delete (to Trash &Bin)" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( true  );
    cleanup->setWorksForDotEntry( false );
    cleanup->setRefreshPolicy( Cleanup::assumeDeleted );
    cleanup->setIcon( "edittrash.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_X );

    return cleanup;
}
	

Cleanup *
StdCleanup::hardDelete( KActionCollection *parent )
{
    Cleanup *cleanup = new KCleanup( "cleanup_hard_delete",
				      "rm -rf %p",
				      i18n( "&Delete (no way to undelete!)" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( true  );
    cleanup->setWorksForDotEntry( false );
    cleanup->setAskForConfirmation( true );
    cleanup->setRefreshPolicy( Cleanup::assumeDeleted );
    cleanup->setIcon( "editdelete.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_Delete );

    return cleanup;
}
	


// EOF
