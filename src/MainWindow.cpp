/*
 *   File name: MainWindow.cpp
 *   Summary:	QDirStat main window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QApplication>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>

#include "MainWindow.h"
#include "ActionManager.h"
#include "CleanupCollection.h"
#include "CleanupConfigPage.h"
#include "ConfigDialog.h"
#include "DataColumns.h"
#include "DebugHelpers.h"
#include "DirTree.h"
#include "DirTreeCache.h"
#include "DirTreeModel.h"
#include "Exception.h"
#include "ExcludeRules.h"
#include "FileDetailsView.h"
#include "FileSizeStatsWindow.h"
#include "FileTypeStatsWindow.h"
#include "Logger.h"
#include "MimeCategorizer.h"
#include "OpenDirDialog.h"
#include "OpenPkgDialog.h"
#include "OutputWindow.h"
#include "PanelMessage.h"
#include "PkgManager.h"
#include "PkgQuery.h"
#include "QDirStatApp.h"
#include "Refresher.h"
#include "SelectionModel.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "SysUtil.h"
#include "Trash.h"
#include "UnreadableDirsWindow.h"
#include "Version.h"

#define LONG_MESSAGE		25*1000
#define UPDATE_MILLISEC		200

#define USE_CUSTOM_OPEN_DIR_DIALOG 1

using namespace QDirStat;


MainWindow::MainWindow():
    QMainWindow(),
    _ui( new Ui::MainWindow ),
    _configDialog( 0 ),
    _enableDirPermissionsWarning( false ),
    _verboseSelection( false ),
    _urlInWindowTitle( false ),
    _useTreemapHover( false ),
    _statusBarTimeout( 3000 ), // millisec
    _treeLevelMapper(0),
    _currentLayout( 0 )
{
    CHECK_PTR( _ui );

    _ui->setupUi( this );
    ActionManager::instance()->addWidgetTree( this );
    initLayoutActions();
    createLayouts();    // see MainWindowLayout.cpp
    readSettings();
    _updateTimer.setInterval( UPDATE_MILLISEC );
    _treeExpandTimer.setSingleShot( true );
    _dUrl = _ui->actionDonate->iconText();
    _futureSelection.setUseRootFallback( false );
    _ui->menubar->setCornerWidget( new QLabel( MENUBAR_VERSION ) );

    // The first call to app() creates the QDirStatApp and with it
    // - the DirTreeModel
    // - the DirTree (owned and managed by the DirTreeModel)
    // - the SelectionModel
    // - the CleanupCollection.

    _ui->dirTreeView->setModel( app()->dirTreeModel() );
    _ui->dirTreeView->setSelectionModel( app()->selectionModel() );

    _ui->treemapView->setDirTree( app()->dirTree() );
    _ui->treemapView->setSelectionModel( app()->selectionModel() );

    app()->cleanupCollection()->addToMenu   ( _ui->menuCleanup,
                                              true ); // keepUpdated
    app()->cleanupCollection()->addToToolBar( _ui->toolBar,
                                              true ); // keepUpdated


    _ui->dirTreeView->setCleanupCollection( app()->cleanupCollection() );
    _ui->treemapView->setCleanupCollection( app()->cleanupCollection() );

    _ui->breadcrumbNavigator->clear();

    _historyButtons = new HistoryButtons( _ui->actionGoBack,
                                          _ui->actionGoForward );
    CHECK_NEW( _historyButtons );

    _discoverActions = new DiscoverActions( this );
    CHECK_NEW( _discoverActions );

#ifdef Q_OS_MACX
    // This makes the application to look like more "native" on macOS
    setUnifiedTitleAndToolBarOnMac( true );
    _ui->toolBar->setMovable( false );
#endif

    connectSignals();
    connectMenuActions();               // see MainWindowMenus.cpp
    changeLayout( _layoutName );        // see MainWindowLayout.cpp

    checkPkgManagerSupport();

    if ( ! _ui->actionShowTreemap->isChecked() )
	_ui->treemapView->disable();

    toggleVerboseSelection();
    updateActions();
}


MainWindow::~MainWindow()
{
    // logDebug() << "Destroying main window" << endl;

    if ( _currentLayout )
	saveLayout( _currentLayout );   // see MainWindowLayout.cpp

    writeSettings();
    ExcludeRules::instance()->writeSettings();
    MimeCategorizer::instance()->writeSettings();

    // Relying on the QObject hierarchy to properly clean this up resulted in a
    //	segfault; there was probably a problem in the deletion order.

    if ( _configDialog )
	delete _configDialog;

    delete _ui->dirTreeView;
    delete _ui;
    delete _historyButtons;

    qDeleteAll( _layouts );

    QDirStatApp::deleteInstance();

    // logDebug() << "Main window destroyed" << endl;
}


void MainWindow::checkPkgManagerSupport()
{
    if ( ! PkgQuery::haveGetInstalledPkgSupport() ||
	 ! PkgQuery::haveFileListSupport()	    )
    {
	logInfo() << "No package manager support "
		  << "for getting installed packages or file lists"
		  << endl;

	_ui->actionOpenPkg->setEnabled( false );
    }

    PkgManager * pkgManager = PkgQuery::primaryPkgManager();

    if ( ! pkgManager || ! pkgManager->supportsFileListCache() )
    {
	logInfo() << "No package manager support "
		  << "for getting a file lists cache"
		  << endl;

	_ui->actionShowUnpkgFiles->setEnabled( false );
    }
}


void MainWindow::connectSignals()
{
    connect( app()->selectionModel(),	 SIGNAL( currentBranchChanged( QModelIndex ) ),
	     _ui->dirTreeView,		 SLOT  ( closeAllExcept	     ( QModelIndex ) ) );

    connect( app()->dirTree(),		 SIGNAL( startingReading() ),
	     this,			 SLOT  ( startingReading() ) );

    connect( app()->dirTree(),		 SIGNAL( finished()	   ),
	     this,			 SLOT  ( readingFinished() ) );

    connect( app()->dirTree(),		 SIGNAL( aborted()	   ),
	     this,			 SLOT  ( readingAborted()  ) );

    connect( app()->selectionModel(),	 SIGNAL( selectionChanged() ),
	     this,			 SLOT  ( updateActions()    ) );

    connect( app()->selectionModel(),	 SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,			 SLOT  ( updateActions()			    ) );

    connect( app()->selectionModel(),	 SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     _ui->breadcrumbNavigator,	 SLOT  ( setPath	   ( FileInfo *		    ) ) );

    connect( app()->selectionModel(),	 SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     _historyButtons,		 SLOT  ( addToHistory	   ( FileInfo *		    ) ) );

    connect( _historyButtons,		 SIGNAL( navigateToUrl( QString ) ),
	     this,			 SLOT  ( navigateToUrl( QString ) ) );

    connect( _ui->breadcrumbNavigator,	 SIGNAL( pathClicked   ( QString ) ),
	     app()->selectionModel(),	 SLOT  ( setCurrentItem( QString ) ) );

    connect( _ui->treemapView,		 SIGNAL( treemapChanged() ),
	     this,			 SLOT  ( updateActions()  ) );

    connect( app()->cleanupCollection(), SIGNAL( startingCleanup( QString ) ),
	     this,			 SLOT  ( startingCleanup( QString ) ) );

    connect( app()->cleanupCollection(), SIGNAL( cleanupFinished( int ) ),
	     this,			 SLOT  ( cleanupFinished( int ) ) );

    connect( &_updateTimer,		 SIGNAL( timeout()	   ),
	     this,			 SLOT  ( showElapsedTime() ) );

    connect( &_treeExpandTimer,		  SIGNAL( timeout() ),
	     _ui->actionExpandTreeLevel1, SLOT( trigger()   ) );

    if ( _useTreemapHover )
    {
	connect( _ui->treemapView,	  SIGNAL( hoverEnter ( FileInfo * ) ),
		 this,			  SLOT	( showCurrent( FileInfo * ) ) );

	connect( _ui->treemapView,	  SIGNAL( hoverLeave ( FileInfo * ) ),
		 this,			  SLOT	( showSummary()		  ) );
    }

    connect( app()->selectionModel(),	  SIGNAL( selectionChanged() ),
	     this,			  SLOT	( selectionChanged() ) );

    connect( app()->selectionModel(),	  SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,			  SLOT	( currentItemChanged( FileInfo *, FileInfo * ) ) );
}


void MainWindow::updateActions()
{
    bool reading	     = app()->dirTree()->isBusy();
    FileInfo * currentItem   = app()->selectionModel()->currentItem();
    FileInfo * firstToplevel = app()->dirTree()->firstToplevel();
    bool pkgView	     = firstToplevel && firstToplevel->isPkgInfo();

    _ui->actionStopReading->setEnabled( reading );
    _ui->actionRefreshAll->setEnabled	( ! reading );
    _ui->actionAskReadCache->setEnabled ( ! reading );
    _ui->actionAskWriteCache->setEnabled( ! reading );

    _ui->actionCopyPathToClipboard->setEnabled( currentItem );
    _ui->actionGoUp->setEnabled( currentItem && currentItem->treeLevel() > 1 );
    _ui->actionGoToToplevel->setEnabled( firstToplevel && ( ! currentItem || currentItem->treeLevel() > 1 ));

    FileInfoSet selectedItems = app()->selectionModel()->selectedItems();
    FileInfo * sel	      = selectedItems.first();
    int selSize		      = selectedItems.size();

    bool oneDirSelected	   = selSize == 1 && sel && sel->isDir() && ! sel->isPkgInfo();
    bool pseudoDirSelected = selectedItems.containsPseudoDir();
    bool pkgSelected	   = selectedItems.containsPkg();

    _ui->actionMoveToTrash->setEnabled( sel && ! pseudoDirSelected && ! pkgSelected && ! reading );
    _ui->actionRefreshSelected->setEnabled( selSize == 1 && ! sel->isExcluded() && ! sel->isMountPoint() && ! pkgView );
    _ui->actionContinueReadingAtMountPoint->setEnabled( oneDirSelected && sel->isMountPoint() );
    _ui->actionReadExcludedDirectory->setEnabled      ( oneDirSelected && sel->isExcluded()   );

    bool nothingOrOneDir = selectedItems.isEmpty() || oneDirSelected;

    _ui->actionFileSizeStats->setEnabled( ! reading && nothingOrOneDir );
    _ui->actionFileTypeStats->setEnabled( ! reading && nothingOrOneDir );
    _ui->actionFileAgeStats->setEnabled ( ! reading && nothingOrOneDir );

    bool showingTreemap = _ui->treemapView->isVisible();

    _ui->actionTreemapAsSidePanel->setEnabled( showingTreemap );
    _ui->actionTreemapZoomIn->setEnabled   ( showingTreemap && _ui->treemapView->canZoomIn() );
    _ui->actionTreemapZoomOut->setEnabled  ( showingTreemap && _ui->treemapView->canZoomOut() );
    _ui->actionResetTreemapZoom->setEnabled( showingTreemap && _ui->treemapView->canZoomOut() );
    _ui->actionTreemapRebuild->setEnabled  ( showingTreemap );

    _historyButtons->updateActions();
}


void MainWindow::readSettings()
{
    QDirStat::Settings settings;
    settings.beginGroup( "MainWindow" );

    _statusBarTimeout	  = settings.value( "StatusBarTimeoutMillisec", 3000  ).toInt();
    bool showTreemap	  = settings.value( "ShowTreemap"	      , true  ).toBool();
    bool treemapOnSide	  = settings.value( "TreemapOnSide"	      , false ).toBool();

    _verboseSelection	  = settings.value( "VerboseSelection"	      , false ).toBool();
    _urlInWindowTitle	  = settings.value( "UrlInWindowTitle"	      , false ).toBool();
    _useTreemapHover	  = settings.value( "UseTreemapHover"	      , false ).toBool();
    _layoutName		  = settings.value( "Layout"		      , "L2"  ).toString();

    settings.endGroup();

    settings.beginGroup( "MainWindow-Subwindows" );
    QByteArray mainSplitterState = settings.value( "MainSplitter" , QByteArray() ).toByteArray();
    QByteArray topSplitterState	 = settings.value( "TopSplitter"  , QByteArray() ).toByteArray();
    settings.endGroup();

    _ui->actionShowTreemap->setChecked( showTreemap );
    _ui->actionTreemapAsSidePanel->setChecked( treemapOnSide );
    treemapAsSidePanel();

    _ui->actionVerboseSelection->setChecked( _verboseSelection );

    foreach ( QAction * action, _layoutActionGroup->actions() )
    {
	if ( action->data().toString() == _layoutName )
	    action->setChecked( true );
    }

    readWindowSettings( this, "MainWindow" );

    if ( ! mainSplitterState.isNull() )
	_ui->mainWinSplitter->restoreState( mainSplitterState );

    if ( ! topSplitterState.isNull() )
	_ui->topViewsSplitter->restoreState( topSplitterState );
    else
    {
	// The Qt designer refuses to let me set a reasonable size for that
	// widget, so let's set one here. Yes, that's not really how this is
	// supposed to be, but I am fed up with that stuff.

	_ui->fileDetailsPanel->resize( QSize( 300, 300 ) );
    }

    foreach ( TreeLayout * layout, _layouts )
	readLayoutSettings( layout );   // see MainWindowLayout.cpp

    ExcludeRules::instance()->readSettings();
    Debug::dumpExcludeRules();
}


void MainWindow::writeSettings()
{
    QDirStat::Settings settings;
    settings.beginGroup( "MainWindow" );

    settings.setValue( "ShowTreemap"	 , _ui->actionShowTreemap->isChecked() );
    settings.setValue( "TreemapOnSide"	 , _ui->actionTreemapAsSidePanel->isChecked() );
    settings.setValue( "VerboseSelection", _verboseSelection );
    settings.setValue( "Layout"		 , _layoutName );

    // Those are only set if not already in the settings (they might have been
    // set from a config dialog).
    settings.setDefaultValue( "StatusBarTimeoutMillisec", _statusBarTimeout );
    settings.setDefaultValue( "UrlInWindowTitle"	, _urlInWindowTitle );
    settings.setDefaultValue( "UseTreemapHover"		, _useTreemapHover );

    settings.endGroup();

    writeWindowSettings( this, "MainWindow" );

    settings.beginGroup( "MainWindow-Subwindows" );
    settings.setValue( "MainSplitter"		 , _ui->mainWinSplitter->saveState()  );
    settings.setValue( "TopSplitter"		 , _ui->topViewsSplitter->saveState() );
    settings.endGroup();

    foreach ( TreeLayout * layout, _layouts )
	writeLayoutSettings( layout );  // see MainWindowLayout.cpp
}


void MainWindow::showTreemapView()
{
    if ( _ui->actionShowTreemap->isChecked() )
	_ui->treemapView->enable();
    else
	_ui->treemapView->disable();
}


void MainWindow::treemapAsSidePanel()
{
    if ( _ui->actionTreemapAsSidePanel->isChecked() )
	_ui->mainWinSplitter->setOrientation( Qt::Horizontal );
    else
	_ui->mainWinSplitter->setOrientation( Qt::Vertical );
}


void MainWindow::busyDisplay()
{
    _ui->treemapView->disable();
    updateActions();

    // If it is open, close the window that lists unreadable directories:
    // With the next directory read, things might have changed; the user may
    // have fixed permissions or ownership of those directories.

    UnreadableDirsWindow::closeSharedInstance();

    if ( _dirPermissionsWarning )
        _dirPermissionsWarning->deleteLater();

    _updateTimer.start();

    // It would be nice to sort by read jobs during reading, but this confuses
    // the hell out of the Qt side of the data model; so let's sort by name
    // instead.

    int sortCol = QDirStat::DataColumns::toViewCol( QDirStat::NameCol );
    _ui->dirTreeView->sortByColumn( sortCol, Qt::AscendingOrder );

    if ( ! PkgFilter::isPkgUrl( app()->dirTree()->url() ) &&
	 ! app()->selectionModel()->currentBranch() )
    {
        _treeExpandTimer.start( 200 );
        // This will trigger actionExpandTreeLevel1. Hopefully after those 200
        // millisec there will be some items in the tree to expand.
    }
}


void MainWindow::idleDisplay()
{
    logInfo() << endl;

    updateActions();
    _updateTimer.stop();
    int sortCol = QDirStat::DataColumns::toViewCol( QDirStat::PercentNumCol );
    _ui->dirTreeView->sortByColumn( sortCol, Qt::DescendingOrder );

    if ( ! _futureSelection.isEmpty() )
    {
        _treeExpandTimer.stop();
        applyFutureSelection();
    }
    else if ( ! app()->selectionModel()->currentBranch() )
    {
	logDebug() << "No current branch - expanding tree to level 1" << endl;
	expandTreeToLevel( 1 );
    }

    updateFileDetailsView();
    showTreemapView();
}


void MainWindow::updateFileDetailsView()
{
    if ( _ui->fileDetailsView->isVisible() )
    {
	FileInfoSet sel = app()->selectionModel()->selectedItems();

	if ( sel.isEmpty() )
	    _ui->fileDetailsView->showDetails( app()->selectionModel()->currentItem() );
	else
	{
	    if ( sel.count() == 1 )
		_ui->fileDetailsView->showDetails( sel.first() );
	    else
		_ui->fileDetailsView->showDetails( sel );
	}
    }
}


void MainWindow::startingReading()
{
    _stopWatch.start();
    busyDisplay();
}


void MainWindow::readingFinished()
{
    logInfo() << endl;

    idleDisplay();

    QString elapsedTime = formatMillisec( _stopWatch.elapsed() );
    _ui->statusBar->showMessage( tr( "Finished. Elapsed time: %1").arg( elapsedTime ), LONG_MESSAGE );
    logInfo() << "Reading finished after " << elapsedTime << endl;

    if ( app()->dirTree()->firstToplevel() &&
	 app()->dirTree()->firstToplevel()->errSubDirCount() > 0 )
    {
	showDirPermissionsWarning();
    }

    // Debug::dumpModelTree( app()->dirTreeModel(), QModelIndex(), "" );
}


void MainWindow::readingAborted()
{
    logInfo() << endl;

    idleDisplay();
    QString elapsedTime = formatMillisec( _stopWatch.elapsed() );
    _ui->statusBar->showMessage( tr( "Aborted. Elapsed time: %1").arg( elapsedTime ), LONG_MESSAGE );
    logInfo() << "Reading aborted after " << elapsedTime << endl;
}


void MainWindow::openUrl( const QString & url )
{
    _enableDirPermissionsWarning = true;
    _historyButtons->clearHistory();

    if ( PkgFilter::isPkgUrl( url ) )
	readPkg( url );
    else if ( isUnpkgUrl( url ) )
	showUnpkgFiles( url );  // see MainWinUnpkg.cpp
    else
	openDir( url );
}


void MainWindow::openDir( const QString & url )
{
    try
    {
	app()->dirTreeModel()->openUrl( url );
	updateWindowTitle( app()->dirTree()->url() );
    }
    catch ( const SysCallFailedException & ex )
    {
	CAUGHT( ex );
        showOpenDirErrorPopup( ex );
	askOpenDir();
    }

    updateActions();
    expandTreeToLevel( 1 );
}


void MainWindow::showOpenDirErrorPopup( const SysCallFailedException & ex )
{
    updateWindowTitle( "" );
    app()->dirTree()->sendFinished();

    QMessageBox errorPopup( QMessageBox::Warning,	// icon
                            tr( "Error" ),		// title
                            tr( "Could not open directory %1" ).arg( ex.resourceName() ), // text
                            QMessageBox::Ok,            // buttons
                            this );			// parent
    errorPopup.setDetailedText( ex.what() );
    errorPopup.exec();
}


void MainWindow::askOpenDir()
{
    QString path;
    DirTree * tree = app()->dirTree();
    bool crossFilesystems = tree->crossFilesystems();

#if USE_CUSTOM_OPEN_DIR_DIALOG
    path = QDirStat::OpenDirDialog::askOpenDir( &crossFilesystems, this );
#else
    path = QFileDialog::getExistingDirectory( this, // parent
                                              tr("Select directory to scan") );
#endif

    if ( ! path.isEmpty() )
    {
	tree->reset();
	tree->setCrossFilesystems( crossFilesystems );
	openUrl( path );
    }
}


void MainWindow::askOpenPkg()
{
    bool canceled;
    PkgFilter pkgFilter = OpenPkgDialog::askPkgFilter( &canceled );

    if ( ! canceled )
    {
	app()->dirTree()->reset();
	readPkg( pkgFilter );
    }
}


void MainWindow::readPkg( const PkgFilter & pkgFilter )
{
    // logInfo() << "URL: " << pkgFilter.url() << endl;

    updateWindowTitle( pkgFilter.url() );
    expandTreeToLevel( 0 );   // Performance boost: Down from 25 to 6 sec.
    app()->dirTreeModel()->readPkg( pkgFilter );
}


void MainWindow::refreshAll()
{
    _enableDirPermissionsWarning = true;
    QString url = app()->dirTree()->url();

    if ( ! url.isEmpty() )
    {
	logDebug() << "Refreshing " << url << endl;

	if ( PkgFilter::isPkgUrl( url ) )
	    app()->dirTreeModel()->readPkg( url );
	else
	    app()->dirTreeModel()->openUrl( url );

        // No need to check if the URL is an unpkg:/ URL:
        //
        // In that case, the previous filters are still set, and just reading
        // the dir tree again from disk with openUrl() will filter out the
        // unwanted packaged files, ignored extensions and excluded directories
        // again.

	updateActions();
    }
    else
    {
	askOpenDir();
    }
}


void MainWindow::refreshSelected()
{
    busyDisplay();
    _futureSelection.set( app()->selectionModel()->selectedItems().first() );
    // logDebug() << "Setting future selection: " << _futureSelection.subtree() << endl;
    app()->dirTreeModel()->refreshSelected();
    updateActions();
}


void MainWindow::applyFutureSelection()
{
    FileInfo * sel = _futureSelection.subtree();
    // logDebug() << "Using future selection: " << sel << endl;

    if ( sel )
    {
        _treeExpandTimer.stop();
        _futureSelection.clear();
        app()->selectionModel()->setCurrentBranch( sel );

        if ( sel->isMountPoint() )
            _ui->dirTreeView->setExpanded( sel, true );
    }
}


void MainWindow::stopReading()
{
    if ( app()->dirTree()->isBusy() )
    {
	app()->dirTree()->abortReading();
	_ui->statusBar->showMessage( tr( "Reading aborted." ), LONG_MESSAGE );
    }
}


void MainWindow::readCache( const QString & cacheFileName )
{
    app()->dirTreeModel()->clear();
    _historyButtons->clearHistory();

    if ( ! cacheFileName.isEmpty() )
	app()->dirTree()->readCache( cacheFileName );
}


void MainWindow::askReadCache()
{
    QString fileName = QFileDialog::getOpenFileName( this, // parent
						     tr( "Select QDirStat cache file" ),
						     DEFAULT_CACHE_NAME );
    if ( ! fileName.isEmpty() )
	readCache( fileName );

    updateActions();
}


void MainWindow::askWriteCache()
{
    QString fileName = QFileDialog::getSaveFileName( this, // parent
						     tr( "Enter name for QDirStat cache file"),
						     DEFAULT_CACHE_NAME );
    if ( ! fileName.isEmpty() )
    {
	bool ok = app()->dirTree()->writeCache( fileName );

	if ( ok )
	{
	    showProgress( tr( "Directory tree written to file %1" ).arg( fileName ) );
	}
	else
	{
	    QMessageBox::critical( this,
				   tr( "Error" ), // Title
				   tr( "ERROR writing cache file %1").arg( fileName ) );
	}
    }
}


void MainWindow::updateWindowTitle( const QString & url )
{
    QString windowTitle = "QDirStat";

    if ( SysUtil::runningAsRoot() )
	windowTitle += tr( " [root]" );

    if ( _urlInWindowTitle )
	windowTitle += " " + url;

    setWindowTitle( windowTitle );
}


void MainWindow::showProgress( const QString & text )
{
    _ui->statusBar->showMessage( text, _statusBarTimeout );
}


void MainWindow::showElapsedTime()
{
    showProgress( tr( "Reading... %1" )
		  .arg( formatMillisec( _stopWatch.elapsed(), false ) ) );
}


void MainWindow::showCurrent( FileInfo * item )
{
    if ( item )
    {
	QString msg = QString( "%1  (%2%3)" )
	    .arg( item->debugUrl() )
	    .arg( item->sizePrefix() )
	    .arg( formatSize( item->totalSize() ) );

	if ( item->readState() == DirPermissionDenied )
	    msg += tr( "  [Permission Denied]" );
        else if ( item->readState() == DirError )
	    msg += tr( "  [Read Error]" );

	_ui->statusBar->showMessage( msg );
    }
    else
    {
	_ui->statusBar->clearMessage();
    }
}


void MainWindow::showSummary()
{
    FileInfoSet sel = app()->selectionModel()->selectedItems();
    int count = sel.size();

    if ( count <= 1 )
	showCurrent( app()->selectionModel()->currentItem() );
    else
    {
	sel = sel.normalized();

	_ui->statusBar->showMessage( tr( "%1 items selected (%2 total)" )
				     .arg( count )
				     .arg( formatSize( sel.totalSize() ) ) );
    }
}


void MainWindow::startingCleanup( const QString & cleanupName )
{
    showProgress( tr( "Starting cleanup action %1" ).arg( cleanupName ) );
}


void MainWindow::cleanupFinished( int errorCount )
{
    logDebug() << "Error count: " << errorCount << endl;

    if ( errorCount == 0 )
	showProgress( tr( "Cleanup action finished successfully." ) );
    else
	showProgress( tr( "Cleanup action finished with %1 errors." ).arg( errorCount ) );
}


void MainWindow::notImplemented()
{
    QMessageBox::warning( this, tr( "Error" ), tr( "Not implemented!" ) );
}


void MainWindow::copyCurrentPathToClipboard()
{
    FileInfo * currentItem = app()->selectionModel()->currentItem();

    if ( currentItem )
    {
	QClipboard * clipboard = QApplication::clipboard();
	QString path = currentItem->path();
	clipboard->setText( path );
	showProgress( tr( "Copied to system clipboard: %1" ).arg( path ) );
    }
    else
    {
	showProgress( tr( "No current item" ) );
    }
}


void MainWindow::expandTreeToLevel( int level )
{
    logDebug() << "Expanding tree to level " << level << endl;

    if ( level < 1 )
	_ui->dirTreeView->collapseAll();
    else
	_ui->dirTreeView->expandToDepth( level - 1 );
}


void MainWindow::navigateUp()
{
    FileInfo * currentItem = app()->selectionModel()->currentItem();

    if ( currentItem && currentItem->parent() &&
	 currentItem->parent() != app()->dirTree()->root() )
    {
	app()->selectionModel()->setCurrentItem( currentItem->parent(),
					 true ); // select
    }
}


void MainWindow::navigateToToplevel()
{
    FileInfo * toplevel = app()->dirTree()->firstToplevel();

    if ( toplevel )
    {
	expandTreeToLevel( 1 );
	app()->selectionModel()->setCurrentItem( toplevel,
                                                 true ); // select
    }
}


void MainWindow::navigateToUrl( const QString & url )
{
    // logDebug() << "Navigating to " << url << endl;

    if ( ! url.isEmpty() )
    {
        FileInfo * sel = app()->dirTree()->locate( url,
                                                   true ); // findPseudoDirs

        if ( sel )

        {
            app()->selectionModel()->setCurrentItem( sel,
                                                     true ); // select
            _ui->dirTreeView->setExpanded( sel, true );
        }
    }
}


void MainWindow::moveToTrash()
{
    FileInfoSet selectedItems = app()->selectionModel()->selectedItems().normalized();

    // Prepare output window

    OutputWindow * outputWindow = new OutputWindow( qApp->activeWindow() );
    CHECK_NEW( outputWindow );

    // Prepare refresher

    FileInfoSet refreshSet = Refresher::parents( selectedItems );
    app()->selectionModel()->prepareRefresh( refreshSet );
    Refresher * refresher  = new Refresher( refreshSet, this );
    CHECK_NEW( refresher );

    connect( outputWindow, SIGNAL( lastProcessFinished( int ) ),
	     refresher,	   SLOT	 ( refresh()		      ) );

    outputWindow->showAfterTimeout();

    // Move all selected items to trash

    foreach ( FileInfo * item, selectedItems )
    {
	bool success = Trash::trash( item->path() );

	if ( success )
	    outputWindow->addStdout( tr( "Moved to trash: %1" ).arg( item->path() ) );
	else
	    outputWindow->addStderr( tr( "Move to trash failed for %1" ).arg( item->path() ) );
    }

    outputWindow->noMoreProcesses();
}


void MainWindow::openConfigDialog()
{
    if ( _configDialog && _configDialog->isVisible() )
	return;

    // For whatever crazy reason it is considerably faster to delete that
    // complex dialog and recreate it from scratch than to simply leave it
    // alive and just show it again. Well, whatever - so be it.
    //
    // And yes, I added debug logging here, in the dialog's setup(), in
    // showEvent(); I added update(). No result whatsoever.
    // Okay, then let's take the long way around.

    if ( _configDialog )
	delete _configDialog;

    _configDialog = new ConfigDialog( this );
    CHECK_PTR( _configDialog );
    _configDialog->cleanupConfigPage()->setCleanupCollection( app()->cleanupCollection() );

    if ( ! _configDialog->isVisible() )
    {
	_configDialog->setup();
	_configDialog->show();
    }
}


void MainWindow::showFileTypeStats()
{
    FileTypeStatsWindow::populateSharedInstance( app()->selectedDirOrRoot() );
}


void MainWindow::showFileSizeStats()
{
    FileSizeStatsWindow::populateSharedInstance( app()->selectedDirOrRoot() );
}


void MainWindow::showFileAgeStats()
{
    if ( ! _fileAgeStatsWindow )
    {
	// This deletes itself when the user closes it. The associated QPointer
	// keeps track of that and sets the pointer to 0 when it happens.

	_fileAgeStatsWindow = new FileAgeStatsWindow( this );

        connect( app()->selectionModel(), SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
                 _fileAgeStatsWindow,     SLOT  ( syncedPopulate    ( FileInfo *             ) ) );

        connect( _fileAgeStatsWindow,     SIGNAL( locateFilesFromYear   ( QString, short ) ),
                 _discoverActions,        SLOT  ( discoverFilesFromYear ( QString, short ) ) );

        connect( _fileAgeStatsWindow,     SIGNAL( locateFilesFromMonth  ( QString, short, short ) ),
                 _discoverActions,        SLOT  ( discoverFilesFromMonth( QString, short, short ) ) );
    }

    _fileAgeStatsWindow->populate( app()->selectedDirOrRoot() );
    _fileAgeStatsWindow->show();
}


void MainWindow::showFilesystems()
{
    if ( ! _filesystemsWindow )
    {
	// This deletes itself when the user closes it. The associated QPointer
	// keeps track of that and sets the pointer to 0 when it happens.

	_filesystemsWindow = new FilesystemsWindow( this );

        connect( _filesystemsWindow, SIGNAL( readFilesystem( QString ) ),
                 this,               SLOT  ( openUrl       ( QString ) ) );
    }

    _filesystemsWindow->populate();
    _filesystemsWindow->show();
}


void MainWindow::showDirPermissionsWarning()
{
    if ( _dirPermissionsWarning || ! _enableDirPermissionsWarning )
	return;

    PanelMessage * msg = new PanelMessage( _ui->messagePanel );
    CHECK_NEW( msg );

    msg->setHeading( tr( "Some directories could not be read." ) );
    msg->setText( tr( "You might not have sufficient permissions." ) );
    msg->setIcon( QPixmap( ":/icons/lock-closed.png" ) );

    msg->connectDetailsLink( this, SLOT( showUnreadableDirs() ) );

    _ui->messagePanel->add( msg );
    _dirPermissionsWarning = msg;
    _enableDirPermissionsWarning = false;
}


void MainWindow::showUnreadableDirs()
{
    UnreadableDirsWindow::populateSharedInstance( app()->dirTree()->root() );
}


void MainWindow::selectionChanged()
{
    showSummary();
    updateFileDetailsView();

    if ( _verboseSelection )
    {
	logNewline();
	app()->selectionModel()->dumpSelectedItems();
    }
}


void MainWindow::currentItemChanged( FileInfo * newCurrent, FileInfo * oldCurrent )
{
    showSummary();

    if ( ! oldCurrent )
	updateFileDetailsView();

    if ( _verboseSelection )
    {
	logDebug() << "new current: " << newCurrent << endl;
	logDebug() << "old current: " << oldCurrent << endl;
	app()->selectionModel()->dumpSelectedItems();
    }
}


void MainWindow::mousePressEvent( QMouseEvent * event )
{
    if ( event )
    {
        QAction * action = 0;

        switch ( event->button() )
        {
            // Handle the back / forward buttons on the mouse to act like the
            // history back / forward buttons in the tool bar

            case Qt::BackButton:
                // logDebug() << "BackButton" << endl;
                action = _ui->actionGoBack;
                break;

            case Qt::ForwardButton:
                // logDebug() << "ForwardButton" << endl;
                action = _ui->actionGoForward;
                break;

            default:
                QMainWindow::mousePressEvent( event );
                break;
        }

        if ( action )
        {
            if ( action->isEnabled() )
                action->trigger();
        }
    }
}




//---------------------------------------------------------------------------
//			       Debugging Helpers
//---------------------------------------------------------------------------


void MainWindow::toggleVerboseSelection()
{
    // Verbose selection is toggled with Shift-F7

    _verboseSelection = _ui->actionVerboseSelection->isChecked();

    if ( app()->selectionModel() )
	app()->selectionModel()->setVerbose( _verboseSelection );

    logInfo() << "Verbose selection is now " << ( _verboseSelection ? "on" : "off" )
	      << ". Change this with Shift-F7." << endl;
}


void MainWindow::itemClicked( const QModelIndex & index )
{
    if ( ! _verboseSelection )
	return;

    if ( index.isValid() )
    {
	FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );

	logDebug() << "Clicked row " << index.row()
		   << " col " << index.column()
		   << " (" << QDirStat::DataColumns::fromViewCol( index.column() ) << ")"
		   << "\t" << item
		   << endl;
	// << " data(0): " << index.model()->data( index, 0 ).toString()
	// logDebug() << "Ancestors: " << Debug::modelTreeAncestors( index ).join( " -> " ) << endl;
    }
    else
    {
	logDebug() << "Invalid model index" << endl;
    }

    // app()->dirTreeModel()->dumpPersistentIndexList();
}


// For more MainWindow:: methods, See also:
//
//   - MainWindowHelp.cpp
//   - MainWindowLayout.cpp
//   - MainWindowMenus.cpp
//   - MainWindowUnpkg.cpp

