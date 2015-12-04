/*
 *   File name:	kstdcleanup.cpp
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <klocale.h>
#include "kcleanup.h"
#include "kstdcleanup.h"

using namespace KDirStat;


KCleanup *
KStdCleanup::openInKonqueror( KActionCollection *parent )
{
    KCleanup *cleanup = new KCleanup( "cleanup_open_in_konqueror",
				      "kfmclient openURL %p",
				      i18n( "Open in &Konqueror" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true );
    cleanup->setWorksForFile    ( true );
    cleanup->setWorksForDotEntry( true );
    cleanup->setWorksLocalOnly	( false );
    cleanup->setRefreshPolicy( KCleanup::noRefresh );
    cleanup->setIcon( "konqueror.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_K );
    
    return cleanup;
}


KCleanup *
KStdCleanup::openInTerminal( KActionCollection *parent )
{
    KCleanup *cleanup = new KCleanup( "cleanup_open_in_terminal",
				      "konsole",
				      i18n( "Open in &Terminal" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true );
    cleanup->setWorksForFile    ( true );
    cleanup->setWorksForDotEntry( true );
    cleanup->setRefreshPolicy( KCleanup::noRefresh );
    cleanup->setIcon( "konsole.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_T );

    return cleanup;
}


KCleanup *
KStdCleanup::compressSubtree( KActionCollection *parent )
{
    KCleanup *cleanup = new KCleanup( "cleanup_compress_subtree",
				      "cd ..; tar cjvf %n.tar.bz2 %n && rm -rf %n",
				      i18n( "&Compress" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( false );
    cleanup->setWorksForDotEntry( false );
    cleanup->setRefreshPolicy( KCleanup::refreshParent );
    cleanup->setIcon( "ark.png" );

    return cleanup;
}


KCleanup *
KStdCleanup::makeClean( KActionCollection *parent )
{
    KCleanup *cleanup = new KCleanup( "cleanup_make_clean",
				      "make clean",
				      i18n( "&make clean" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( false );
    cleanup->setWorksForDotEntry( true  );
    cleanup->setRefreshPolicy( KCleanup::refreshThis );

    return cleanup;
}


KCleanup *
KStdCleanup::deleteTrash( KActionCollection *parent )
{
    KCleanup *cleanup = new KCleanup( "cleanup_delete_trash",
				      "rm -f *.o *~ *.bak *.auto core",
				      i18n( "Delete T&rash Files" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( false );
    cleanup->setWorksForDotEntry( true  );
    cleanup->setRefreshPolicy( KCleanup::refreshThis );
    cleanup->setRecurse( true );

    return cleanup;
}


KCleanup *
KStdCleanup::moveToTrashBin( KActionCollection *parent )
{
    KCleanup *cleanup = new KCleanup( "cleanup_move_to_trash_bin",
				      "kfmclient move %p %t",
				      i18n( "Delete (to Trash &Bin)" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( true  );
    cleanup->setWorksForDotEntry( false );
    cleanup->setRefreshPolicy( KCleanup::assumeDeleted );
    cleanup->setIcon( "edittrash.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_X );

    return cleanup;
}
	

KCleanup *
KStdCleanup::hardDelete( KActionCollection *parent )
{
    KCleanup *cleanup = new KCleanup( "cleanup_hard_delete",
				      "rm -rf %p",
				      i18n( "&Delete (no way to undelete!)" ),
				      parent );
    CHECK_PTR( cleanup );
    cleanup->setWorksForDir     ( true  );
    cleanup->setWorksForFile    ( true  );
    cleanup->setWorksForDotEntry( false );
    cleanup->setAskForConfirmation( true );
    cleanup->setRefreshPolicy( KCleanup::assumeDeleted );
    cleanup->setIcon( "editdelete.png" );
    cleanup->setShortcut( Qt::CTRL + Qt::Key_Delete );

    return cleanup;
}
	


// EOF
