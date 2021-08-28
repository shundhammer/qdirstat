/*
 *   File name: MainWindowMenus.cpp
 *   Summary:	Connecting menu actions in the QDirStat main window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QSignalMapper>

#include "MainWindow.h"
#include "QDirStatApp.h"
#include "SelectionModel.h"
#include "Version.h"
#include "Exception.h"
#include "Logger.h"


#if (QT_VERSION < QT_VERSION_CHECK( 5, 13, 0 ))
#  define HAVE_SIGNAL_MAPPER	  1
#else
// QSignalMapper is deprecated from Qt 5.13 on
#  define HAVE_SIGNAL_MAPPER	  0
#endif

using namespace QDirStat;


#define CONNECT_ACTION(ACTION, RECEIVER, RCVR_SLOT) \
    connect( (ACTION), SIGNAL( triggered() ), (RECEIVER), SLOT( RCVR_SLOT ) )


void MainWindow::connectMenuActions()
{
    connectFileMenu();
    connectEditMenu();
    connectViewMenu();
    connectGoMenu();
    connectDiscoverMenu();
    // CleanupCollection::add() handles the cleanup actions
    connectHelpMenu();

    connectDebugActions();      // Invisible F7 / Shift-F7 actions
}


void MainWindow::connectFileMenu()
{
    CONNECT_ACTION( _ui->actionOpenDir,			    this, askOpenDir()	      );
    CONNECT_ACTION( _ui->actionOpenPkg,			    this, askOpenPkg()	      );
    CONNECT_ACTION( _ui->actionShowUnpkgFiles,		    this, askShowUnpkgFiles() );
    CONNECT_ACTION( _ui->actionRefreshAll,		    this, refreshAll()	      );
    CONNECT_ACTION( _ui->actionRefreshSelected,		    this, refreshSelected()   );
    CONNECT_ACTION( _ui->actionReadExcludedDirectory,	    this, refreshSelected()   );
    CONNECT_ACTION( _ui->actionContinueReadingAtMountPoint, this, refreshSelected()   );
    CONNECT_ACTION( _ui->actionStopReading,		    this, stopReading()	      );
    CONNECT_ACTION( _ui->actionAskWriteCache,		    this, askWriteCache()     );
    CONNECT_ACTION( _ui->actionAskReadCache,		    this, askReadCache()      );
    CONNECT_ACTION( _ui->actionQuit,			    qApp, quit()	      );
}


void MainWindow::connectEditMenu()
{

    CONNECT_ACTION( _ui->actionCopyPathToClipboard, this, copyCurrentPathToClipboard() );
    CONNECT_ACTION( _ui->actionMoveToTrash,	    this, moveToTrash()                );
    CONNECT_ACTION( _ui->actionConfigure,           this, openConfigDialog()           );
}


void MainWindow::connectViewMenu()
{
    connectViewExpandMenu();
    connectViewTreemapMenu();

    connect( _ui->actionShowCurrentPath,  SIGNAL( toggled   ( bool ) ),
	     _ui->breadcrumbNavigator,	  SLOT	( setVisible( bool ) ) );

    connect( _ui->actionShowDetailsPanel, SIGNAL( toggled   ( bool ) ),
	     _ui->fileDetailsPanel,	  SLOT	( setVisible( bool ) ) );

    CONNECT_ACTION( _ui->actionLayout1,		   this, changeLayout() );
    CONNECT_ACTION( _ui->actionLayout2,		   this, changeLayout() );
    CONNECT_ACTION( _ui->actionLayout3,		   this, changeLayout() );

    CONNECT_ACTION( _ui->actionFileSizeStats,	   this, showFileSizeStats() );
    CONNECT_ACTION( _ui->actionFileTypeStats,	   this, showFileTypeStats() );

    _ui->actionFileTypeStats->setShortcutContext( Qt::ApplicationShortcut );

    CONNECT_ACTION( _ui->actionFileAgeStats,	   this, showFileAgeStats()  );
    CONNECT_ACTION( _ui->actionShowFilesystems,	   this, showFilesystems()   );
}


void MainWindow::connectViewExpandMenu()
{
#if HAVE_SIGNAL_MAPPER

    // QSignalMapper is deprecated from Qt 5.13 on.
    // On systems with older versions, there may or may not be C++11 compiler.

    _treeLevelMapper = new QSignalMapper( this );

    connect( _treeLevelMapper, SIGNAL( mapped		( int ) ),
	     this,	       SLOT  ( expandTreeToLevel( int ) ) );

    mapTreeExpandAction( _ui->actionExpandTreeLevel0, 0 );
    mapTreeExpandAction( _ui->actionExpandTreeLevel1, 1 );
    mapTreeExpandAction( _ui->actionExpandTreeLevel2, 2 );
    mapTreeExpandAction( _ui->actionExpandTreeLevel3, 3 );
    mapTreeExpandAction( _ui->actionExpandTreeLevel4, 4 );
    mapTreeExpandAction( _ui->actionExpandTreeLevel5, 5 );
    mapTreeExpandAction( _ui->actionExpandTreeLevel6, 6 );
    mapTreeExpandAction( _ui->actionExpandTreeLevel7, 7 );
    mapTreeExpandAction( _ui->actionExpandTreeLevel8, 8 );
    mapTreeExpandAction( _ui->actionExpandTreeLevel9, 9 );

    mapTreeExpandAction( _ui->actionCloseAllTreeLevels, 0 );

#else   // QSignalMapper not available / deprecated? (Qt 5.13 or later) -> use a C++11 lambda

    connect( _ui->actionExpandTreeLevel0,   &QAction::triggered, [=]() { expandTreeToLevel( 0 ); } );
    connect( _ui->actionExpandTreeLevel1,   &QAction::triggered, [=]() { expandTreeToLevel( 1 ); } );
    connect( _ui->actionExpandTreeLevel2,   &QAction::triggered, [=]() { expandTreeToLevel( 2 ); } );
    connect( _ui->actionExpandTreeLevel3,   &QAction::triggered, [=]() { expandTreeToLevel( 3 ); } );
    connect( _ui->actionExpandTreeLevel4,   &QAction::triggered, [=]() { expandTreeToLevel( 4 ); } );
    connect( _ui->actionExpandTreeLevel5,   &QAction::triggered, [=]() { expandTreeToLevel( 5 ); } );
    connect( _ui->actionExpandTreeLevel6,   &QAction::triggered, [=]() { expandTreeToLevel( 6 ); } );
    connect( _ui->actionExpandTreeLevel7,   &QAction::triggered, [=]() { expandTreeToLevel( 7 ); } );
    connect( _ui->actionExpandTreeLevel8,   &QAction::triggered, [=]() { expandTreeToLevel( 8 ); } );
    connect( _ui->actionExpandTreeLevel9,   &QAction::triggered, [=]() { expandTreeToLevel( 9 ); } );

    connect( _ui->actionCloseAllTreeLevels, &QAction::triggered, [=]() { expandTreeToLevel( 0 ); } );

#endif
}


void MainWindow::connectViewTreemapMenu()
{
    connect( _ui->actionShowTreemap, SIGNAL( toggled( bool )   ),
	     this,		     SLOT  ( showTreemapView() ) );

    connect( _ui->actionTreemapAsSidePanel, SIGNAL( toggled( bool )	 ),
	     this,			    SLOT  ( treemapAsSidePanel() ) );

    CONNECT_ACTION( _ui->actionTreemapZoomIn,	 _ui->treemapView, zoomIn()	    );
    CONNECT_ACTION( _ui->actionTreemapZoomOut,	 _ui->treemapView, zoomOut()	    );
    CONNECT_ACTION( _ui->actionResetTreemapZoom, _ui->treemapView, resetZoom()	    );
    CONNECT_ACTION( _ui->actionTreemapRebuild,	 _ui->treemapView, rebuildTreemap() );
}


void MainWindow::connectGoMenu()
{
    CONNECT_ACTION( _ui->actionGoBack,	     _historyButtons,   historyGoBack()      );
    CONNECT_ACTION( _ui->actionGoForward,    _historyButtons,   historyGoForward()   );
    CONNECT_ACTION( _ui->actionGoUp,	     this,              navigateUp()         );
    CONNECT_ACTION( _ui->actionGoToToplevel, this,              navigateToToplevel() );
}


void MainWindow::connectDiscoverMenu()
{
    CONNECT_ACTION( _ui->actionDiscoverLargestFiles,    _discoverActions, discoverLargestFiles()    );
    CONNECT_ACTION( _ui->actionDiscoverNewestFiles,     _discoverActions, discoverNewestFiles()     );
    CONNECT_ACTION( _ui->actionDiscoverOldestFiles,     _discoverActions, discoverOldestFiles()     );
    CONNECT_ACTION( _ui->actionDiscoverHardLinkedFiles, _discoverActions, discoverHardLinkedFiles() );
    CONNECT_ACTION( _ui->actionDiscoverBrokenSymLinks,  _discoverActions, discoverBrokenSymLinks()  );
    CONNECT_ACTION( _ui->actionDiscoverSparseFiles,     _discoverActions, discoverSparseFiles()     );
}


void MainWindow::connectHelpMenu()
{
    // See MainWindowHelp.cpp for the methods

    _ui->actionWhatsNew->setStatusTip( RELEASE_URL ); // defined in Version.h

    CONNECT_ACTION( _ui->actionHelp,		 this, openActionUrl()    );
    CONNECT_ACTION( _ui->actionPkgViewHelp,	 this, openActionUrl()    );
    CONNECT_ACTION( _ui->actionUnpkgViewHelp,	 this, openActionUrl()    );
    CONNECT_ACTION( _ui->actionFileAgeStatsHelp, this, openActionUrl()    );
    CONNECT_ACTION( _ui->actionWhatsNew,	 this, openActionUrl()	  );

    CONNECT_ACTION( _ui->actionAbout,		 this, showAboutDialog()  );
    CONNECT_ACTION( _ui->actionAboutQt,		 qApp, aboutQt()	  );
    CONNECT_ACTION( _ui->actionDonate,		 this, showDonateDialog() );

    connectHelpSolutionsMenu();
}


void MainWindow::connectHelpSolutionsMenu()
{
    // Connect all actions of submenu "Help" -> "Problems and Solutions"
    // to display the URL that they have in their statusTip property in a browser

    foreach ( QAction * action, _ui->menuProblemsAndSolutions->actions() )
    {
        QString url = action->statusTip();

        if ( url.isEmpty() )
            logWarning() << "No URL in statusTip property of action " << action->objectName() << endl;
        else
            CONNECT_ACTION( action, this, openActionUrl() );
    }
}


void MainWindow::connectDebugActions()
{
    // Invisible debug actions

    addAction( _ui->actionVerboseSelection );    // Shift-F7
    addAction( _ui->actionDumpSelection );       // F7

    connect( _ui->actionVerboseSelection, SIGNAL( toggled( bool )	   ),
	     this,			  SLOT	( toggleVerboseSelection() ) );

    CONNECT_ACTION( _ui->actionDumpSelection, app()->selectionModel(), dumpSelectedItems() );

    connect( _ui->dirTreeView,		  SIGNAL( clicked    ( QModelIndex ) ),
	     this,			  SLOT	( itemClicked( QModelIndex ) ) );
}


void MainWindow::mapTreeExpandAction( QAction * action, int level )
{
    if ( _treeLevelMapper )
    {
	CONNECT_ACTION( action, _treeLevelMapper, map() );
	_treeLevelMapper->setMapping( action, level );

        // Each action in the _treeLevelMapper is mapped to expandTreeToLevel()
    }
}


// For more MainWindow:: methods, See also:
//
//   - MainWindow.cpp
//   - MainWindowHelp.cpp
//   - MainWindowLayout.cpp
//   - MainWindowUnpkg.cpp
