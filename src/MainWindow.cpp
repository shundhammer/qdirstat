/*
 *   File name: MainWindow.cpp
 *   Summary:	QDirStat main window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QApplication>
#include <QCloseEvent>
#include <QMessageBox>
#include <QProcess>
#include <QFileDialog>
#include <QSignalMapper>
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
#include "DirTreePkgFilter.h"
#include "Exception.h"
#include "ExcludeRules.h"
#include "FileDetailsView.h"
#include "FileInfo.h"
#include "FileSizeStatsWindow.h"
#include "HeaderTweaker.h"
#include "Logger.h"
#include "MimeCategorizer.h"
#include "MimeCategoryConfigPage.h"
#include "OpenPkgDialog.h"
#include "OutputWindow.h"
#include "PkgManager.h"
#include "PkgQuery.h"
#include "Refresher.h"
#include "SelectionModel.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "ShowUnpkgFilesDialog.h"
#include "SysUtil.h"
#include "Trash.h"
#include "Version.h"

#define LONG_MESSAGE	25*1000
#define UPDATE_MILLISEC 200

using namespace QDirStat;

using QDirStat::DataColumns;
using QDirStat::DirTreeModel;
using QDirStat::SelectionModel;
using QDirStat::CleanupCollection;
using QDirStat::PkgFilter;


MainWindow::MainWindow():
    QMainWindow(),
    _ui( new Ui::MainWindow ),
    _configDialog(0),
    _modified( false ),
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
    createLayouts();
    readSettings();
    _updateTimer.setInterval( UPDATE_MILLISEC );
    _dUrl = _ui->actionDonate->iconText();

    _dirTreeModel = new DirTreeModel( this );
    CHECK_NEW( _dirTreeModel );

    _selectionModel = new SelectionModel( _dirTreeModel, this );
    CHECK_NEW( _selectionModel );

    _ui->dirTreeView->setModel( _dirTreeModel );
    _ui->dirTreeView->setSelectionModel( _selectionModel );

    _ui->treemapView->setDirTree( _dirTreeModel->tree() );
    _ui->treemapView->setSelectionModel( _selectionModel );

    _dirTreeModel->setSelectionModel( _selectionModel );
    changeLayout( _layoutName );

    _cleanupCollection = new CleanupCollection( _selectionModel );
    CHECK_NEW( _cleanupCollection );

    _cleanupCollection->addToMenu   ( _ui->menuCleanup,
				      true ); // keepUpdated
    _cleanupCollection->addToToolBar( _ui->toolBar,
				      true ); // keepUpdated


    _ui->dirTreeView->setCleanupCollection( _cleanupCollection );
    _ui->treemapView->setCleanupCollection( _cleanupCollection );

    _mimeCategorizer = new MimeCategorizer();
    CHECK_NEW( _mimeCategorizer );

    _ui->treemapView->setMimeCategorizer( _mimeCategorizer );
    _ui->breadcrumbNavigator->clear();

#ifdef Q_OS_MACX
    // This makes the application to look like more "native" on macOS
    setUnifiedTitleAndToolBarOnMac(true);
    _ui->toolBar->setMovable(false);
#endif

    connectSignals();
    connectActions();

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

    if ( ! _ui->actionShowTreemap->isChecked() )
	_ui->treemapView->disable();

    toggleVerboseSelection();
    updateActions();
}


MainWindow::~MainWindow()
{
    if ( _currentLayout )
	saveLayout( _currentLayout );

    writeSettings();
    ExcludeRules::instance()->writeSettings();

    // Relying on the QObject hierarchy to properly clean this up resulted in a
    //	segfault; there was probably a problem in the deletion order.

    if ( _configDialog )
	delete _configDialog;

    delete _ui->dirTreeView;
    delete _cleanupCollection;
    delete _mimeCategorizer;
    delete _selectionModel;
    delete _dirTreeModel;

    qDeleteAll( _layouts );
}


void MainWindow::connectSignals()
{
    connect( _selectionModel,		SIGNAL( currentBranchChanged( QModelIndex ) ),
	     _ui->dirTreeView,		SLOT  ( closeAllExcept	    ( QModelIndex ) ) );

    connect( _dirTreeModel->tree(),	SIGNAL( startingReading() ),
	     this,			SLOT  ( startingReading() ) );

    connect( _dirTreeModel->tree(),	SIGNAL( finished()	  ),
	     this,			SLOT  ( readingFinished() ) );

    connect( _dirTreeModel->tree(),	SIGNAL( aborted()	  ),
	     this,			SLOT  ( readingAborted()  ) );

    connect( _selectionModel,		SIGNAL( selectionChanged() ),
	     this,			SLOT  ( updateActions()	   ) );

    connect( _selectionModel,		SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,			SLOT  ( updateActions()				    ) );

    connect( _selectionModel,		SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     _ui->breadcrumbNavigator,	SLOT  ( setPath		  ( FileInfo *		   ) ) );

    connect( _ui->breadcrumbNavigator,	SIGNAL( pathClicked   ( QString ) ),
	     _selectionModel,		SLOT  ( setCurrentItem( QString ) ) );

    connect( _ui->treemapView,		SIGNAL( treemapChanged() ),
	     this,			SLOT  ( updateActions()	 ) );

    connect( _cleanupCollection,	SIGNAL( startingCleanup( QString ) ),
	     this,			SLOT  ( startingCleanup( QString ) ) );

    connect( _cleanupCollection,	SIGNAL( cleanupFinished( int ) ),
	     this,			SLOT  ( cleanupFinished( int ) ) );

    connect( &_updateTimer,		SIGNAL( timeout()	  ),
	     this,			SLOT  ( showElapsedTime() ) );

    if ( _useTreemapHover )
    {
	connect( _ui->treemapView,	SIGNAL( hoverEnter ( FileInfo * ) ),
		 this,			SLOT  ( showCurrent( FileInfo * ) ) );

	connect( _ui->treemapView,	SIGNAL( hoverLeave ( FileInfo * ) ),
		 this,			SLOT  ( showSummary()		) );
    }

    // Debug connections

    connect( _ui->dirTreeView,		SIGNAL( clicked	   ( QModelIndex ) ),
	     this,			SLOT  ( itemClicked( QModelIndex ) ) );

    connect( _selectionModel,		SIGNAL( selectionChanged() ),
	     this,			SLOT  ( selectionChanged() ) );

    connect( _selectionModel,		SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,			SLOT  ( currentItemChanged( FileInfo *, FileInfo * ) ) );
}


#define CONNECT_ACTION(ACTION, RECEIVER, RCVR_SLOT) \
    connect( (ACTION), SIGNAL( triggered() ), (RECEIVER), SLOT( RCVR_SLOT ) )


void MainWindow::connectActions()
{
    // "File" menu

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



    // "View" menu

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



    // "Edit" menu

    CONNECT_ACTION( _ui->actionCopyPathToClipboard, this, copyCurrentPathToClipboard() );
    CONNECT_ACTION( _ui->actionMoveToTrash,	    this, moveToTrash() );


    // "Go To" menu

    CONNECT_ACTION( _ui->actionGoUp,	     this, navigateUp() );
    CONNECT_ACTION( _ui->actionGoToToplevel, this, navigateToToplevel() );


    // "Treemap" menu

    connect( _ui->actionShowTreemap, SIGNAL( toggled( bool )   ),
	     this,		     SLOT  ( showTreemapView() ) );

    connect( _ui->actionTreemapAsSidePanel, SIGNAL( toggled( bool )	 ),
	     this,			    SLOT  ( treemapAsSidePanel() ) );

    CONNECT_ACTION( _ui->actionTreemapZoomIn,	 _ui->treemapView, zoomIn()	    );
    CONNECT_ACTION( _ui->actionTreemapZoomOut,	 _ui->treemapView, zoomOut()	    );
    CONNECT_ACTION( _ui->actionResetTreemapZoom, _ui->treemapView, resetZoom()	    );
    CONNECT_ACTION( _ui->actionTreemapRebuild,	 _ui->treemapView, rebuildTreemap() );

    // "Settings" menu

    CONNECT_ACTION( _ui->actionConfigure, this, openConfigDialog() );


    // "Help" menu

    CONNECT_ACTION( _ui->actionHelp,	    this, showHelp() );
    CONNECT_ACTION( _ui->actionPkgViewHelp, this, showPkgViewHelp() );
    CONNECT_ACTION( _ui->actionAbout,	    this, showAboutDialog() );
    CONNECT_ACTION( _ui->actionAboutQt,	    qApp, aboutQt() );
    CONNECT_ACTION( _ui->actionWhatsNew,    this, showWhatsNew() );
    CONNECT_ACTION( _ui->actionDonate,	    this, showDonateDialog() );


    // Invisible debug actions

    addAction( _ui->actionVerboseSelection );
    addAction( _ui->actionDumpSelection );

    connect( _ui->actionVerboseSelection, SIGNAL( toggled( bool )	   ),  // Shift-F7
	     this,			  SLOT	( toggleVerboseSelection() ) );

    CONNECT_ACTION( _ui->actionDumpSelection, _selectionModel, dumpSelectedItems() ); // F7
}


void MainWindow::mapTreeExpandAction( QAction * action, int level )
{
    CONNECT_ACTION( action, _treeLevelMapper, map() );
    _treeLevelMapper->setMapping( action, level );
}


void MainWindow::updateActions()
{
    bool reading	     = _dirTreeModel->tree()->isBusy();
    FileInfo * currentItem   = _selectionModel->currentItem();
    FileInfo * firstToplevel = _dirTreeModel->tree()->firstToplevel();
    bool pkgView	     = firstToplevel && firstToplevel->isPkgInfo();

    _ui->actionStopReading->setEnabled( reading );
    _ui->actionRefreshAll->setEnabled	( ! reading );
    _ui->actionAskReadCache->setEnabled ( ! reading );
    _ui->actionAskWriteCache->setEnabled( ! reading );

    _ui->actionCopyPathToClipboard->setEnabled( currentItem );
    _ui->actionGoUp->setEnabled( currentItem && currentItem->treeLevel() > 1 );
    _ui->actionGoToToplevel->setEnabled( firstToplevel && ( ! currentItem || currentItem->treeLevel() > 1 ));

    FileInfoSet selectedItems = _selectionModel->selectedItems();
    FileInfo * sel	      = selectedItems.first();
    int selSize		      = selectedItems.size();

    bool oneDirSelected	  = selSize == 1 && sel && sel->isDir() && ! sel->isPkgInfo();
    bool dotEntrySelected = selectedItems.containsDotEntry();
    bool pkgSelected	  = selectedItems.containsPkg();

    _ui->actionMoveToTrash->setEnabled( sel && ! dotEntrySelected && ! pkgSelected && ! reading );
    _ui->actionRefreshSelected->setEnabled( oneDirSelected && ! sel->isExcluded() && ! sel->isMountPoint() && ! pkgView );
    _ui->actionContinueReadingAtMountPoint->setEnabled( oneDirSelected && sel->isMountPoint() );
    _ui->actionReadExcludedDirectory->setEnabled      ( oneDirSelected && sel->isExcluded()   );

    bool nothingOrOneDir = selectedItems.isEmpty() || oneDirSelected;

    _ui->actionFileSizeStats->setEnabled( ! reading && nothingOrOneDir );
    _ui->actionFileTypeStats->setEnabled( ! reading && nothingOrOneDir );

    bool showingTreemap = _ui->treemapView->isVisible();

    _ui->actionTreemapAsSidePanel->setEnabled( showingTreemap );
    _ui->actionTreemapZoomIn->setEnabled   ( showingTreemap && _ui->treemapView->canZoomIn() );
    _ui->actionTreemapZoomOut->setEnabled  ( showingTreemap && _ui->treemapView->canZoomOut() );
    _ui->actionResetTreemapZoom->setEnabled( showingTreemap && _ui->treemapView->canZoomOut() );
    _ui->actionTreemapRebuild->setEnabled  ( showingTreemap );
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
	readLayoutSettings( layout );

    ExcludeRules::instance()->readSettings();
    Debug::dumpExcludeRules();
}


void MainWindow::readLayoutSettings( TreeLayout * layout )
{
    CHECK_PTR( layout );

    Settings settings;
    settings.beginGroup( QString( "TreeViewLayout_%1" ).arg( layout->name ) );

    layout->showCurrentPath  = settings.value( "ShowCurrentPath" , layout->showCurrentPath  ).toBool();
    layout->showDetailsPanel = settings.value( "ShowDetailsPanel", layout->showDetailsPanel ).toBool();

    settings.endGroup();
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
	writeLayoutSettings( layout );
}


void MainWindow::writeLayoutSettings( TreeLayout * layout )
{
    CHECK_PTR( layout );

    Settings settings;
    settings.beginGroup( QString( "TreeViewLayout_%1" ).arg( layout->name ) );

    settings.setValue( "ShowCurrentPath" , layout->showCurrentPath  );
    settings.setValue( "ShowDetailsPanel", layout->showDetailsPanel );

    settings.endGroup();
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
	_ui->mainWinSplitter->setOrientation(Qt::Horizontal);
    else
	_ui->mainWinSplitter->setOrientation(Qt::Vertical);
}


void MainWindow::closeEvent( QCloseEvent *event )
{
    if ( _modified )
    {
	int button = QMessageBox::question( this, tr( "Unsaved changes" ),
					    tr( "Save changes?" ),
					    QMessageBox::Save |
					    QMessageBox::Discard |
					    QMessageBox::Cancel );

	if ( button == QMessageBox::Cancel )
	{
	    event->ignore();
	    return;
	}

	if ( button == QMessageBox::Save )
	{
	    // saveFile();
	}

	event->accept();
    }
    else
    {
	event->accept();
    }
}


void MainWindow::busyDisplay()
{
    _ui->treemapView->disable();
    updateActions();
    _updateTimer.start();

    // It would be nice to sort by read jobs during reading, but this confuses
    // the hell out of the Qt side of the data model; so let's sort by name
    // instead.

    int sortCol = QDirStat::DataColumns::toViewCol( QDirStat::NameCol );
    _ui->dirTreeView->sortByColumn( sortCol, Qt::AscendingOrder );

    if ( ! _selectionModel->currentBranch() )
    {
	// Wait until the toplevel entry has some children, then expand to level 1
	QTimer::singleShot( 200, _ui->actionExpandTreeLevel0, SLOT( trigger() ) );
    }
}


void MainWindow::idleDisplay()
{
    updateActions();
    _updateTimer.stop();
    int sortCol = QDirStat::DataColumns::toViewCol( QDirStat::PercentNumCol );
    _ui->dirTreeView->sortByColumn( sortCol, Qt::DescendingOrder );

    if ( ! _selectionModel->currentBranch() )
    {
	logDebug() << "No current branch - expanding tree to level 1" << endl;
	expandTreeToLevel( 1 );
    }

    updateFileDetailsView();
    showTreemapView();
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

    QString elapsedTime = formatTime( _stopWatch.elapsed() );
    _ui->statusBar->showMessage( tr( "Finished. Elapsed time: %1").arg( elapsedTime ), LONG_MESSAGE );
    logInfo() << "Reading finished after " << elapsedTime << endl;

    // Debug::dumpModelTree( _dirTreeModel, QModelIndex(), "" );
}


void MainWindow::readingAborted()
{
    logInfo() << endl;

    idleDisplay();
    QString elapsedTime = formatTime( _stopWatch.elapsed() );
    _ui->statusBar->showMessage( tr( "Aborted. Elapsed time: %1").arg( elapsedTime ), LONG_MESSAGE );
    logInfo() << "Reading aborted after " << elapsedTime << endl;
}


void MainWindow::openUrl( const QString & url )
{
    if ( PkgFilter::isPkgUrl( url ) )
	readPkg( url );
    else
	openDir( url );
}


void MainWindow::openDir( const QString & url )
{
    try
    {
	_dirTreeModel->openUrl( url );
	updateWindowTitle( _dirTreeModel->tree()->url() );
    }
    catch ( const SysCallFailedException & ex )
    {
	CAUGHT( ex );
	updateWindowTitle( "" );
	_dirTreeModel->tree()->sendFinished();

	QMessageBox errorPopup( QMessageBox::Warning,	// icon
				tr( "Error" ),		// title
				tr( "Could not open directory %1" ).arg( ex.resourceName() ), // text
				QMessageBox::Ok,	// buttons
				this );			// parent
	errorPopup.setDetailedText( ex.what() );
	errorPopup.exec();
	askOpenDir();
    }

    updateActions();
    expandTreeToLevel( 1 );
}


void MainWindow::askOpenDir()
{
    QString url = QFileDialog::getExistingDirectory( this, // parent
						     tr("Select directory to scan") );
    if ( ! url.isEmpty() )
    {
	_dirTreeModel->tree()->reset();
	openUrl( url );
    }
}


void MainWindow::askOpenPkg()
{
    bool canceled;
    PkgFilter pkgFilter = OpenPkgDialog::askPkgFilter( canceled );

    if ( ! canceled )
    {
	_dirTreeModel->tree()->reset();
	readPkg( pkgFilter );
    }
}


void MainWindow::askShowUnpkgFiles()
{
    PkgManager * pkgManager = PkgQuery::primaryPkgManager();

    if ( ! pkgManager )
    {
	logError() << "No supported primary package manager" << endl;
	return;
    }

    ShowUnpkgFilesDialog dialog( this );

    if ( dialog.exec() == QDialog::Accepted )
    {
	// Fetch the data the user entered from the dialog

	QString	    dir		= dialog.startingDir();
	QStringList excludeDirs = dialog.excludeDirs();

	logDebug() << "starting dir: " << dir << endl;
	logDebug() << "exclude dirs: " << excludeDirs << endl;


	// Set up the exclude rules accordingly

	ExcludeRules * excludeRules = new ExcludeRules( excludeDirs );
	CHECK_NEW( excludeRules );

	DirTree * tree = _dirTreeModel->tree();
	tree->setExcludeRules( excludeRules );


	// Prepare the filters with the complete file list of all installed packages

	DirTreeFilter * filter = new DirTreePkgFilter( pkgManager );
	CHECK_NEW( filter );

	tree->clearFilters();
	tree->addFilter( filter );


	// Start reading the directory

	_dirTreeModel->openUrl( dir );
    }
}


void MainWindow::refreshAll()
{
    QString url = _dirTreeModel->tree()->url();

    if ( ! url.isEmpty() )
    {
	logDebug() << "Refreshing " << url << endl;

	if ( PkgFilter::isPkgUrl( url ) )
	    _dirTreeModel->readPkg( url );
	else
	    _dirTreeModel->openUrl( url );

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
    _dirTreeModel->refreshSelected();
    updateActions();
}


void MainWindow::stopReading()
{
    if ( _dirTreeModel->tree()->isBusy() )
    {
	_dirTreeModel->tree()->abortReading();
	_ui->statusBar->showMessage( tr( "Reading aborted." ), LONG_MESSAGE );
    }
}


void MainWindow::readCache( const QString & cacheFileName )
{
    _dirTreeModel->clear();

    if ( ! cacheFileName.isEmpty() )
	_dirTreeModel->tree()->readCache( cacheFileName );
}


void MainWindow::askReadCache()
{
    QString fileName = QFileDialog::getOpenFileName( this, // parent
						     tr( "Select QDirStat cache file" ),
						     DEFAULT_CACHE_NAME );
    if ( ! fileName.isEmpty() )
	readCache( fileName );
}


void MainWindow::askWriteCache()
{
    QString fileName = QFileDialog::getSaveFileName( this, // parent
						     tr( "Enter name for QDirStat cache file"),
						     DEFAULT_CACHE_NAME );
    if ( ! fileName.isEmpty() )
    {
	bool ok = _dirTreeModel->tree()->writeCache( fileName );

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


void MainWindow::readPkg( const PkgFilter & pkgFilter )
{
    // logInfo() << "URL: " << pkgFilter.url() << endl;

    updateWindowTitle( pkgFilter.url() );
    expandTreeToLevel( 0 );   // Performance boost: Down from 25 to 6 sec.
    _dirTreeModel->readPkg( pkgFilter );
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


void MainWindow::expandTreeToLevel( int level )
{
    logDebug() << "Expanding tree to level " << level << endl;

    if ( level < 1 )
	_ui->dirTreeView->collapseAll();
    else
	_ui->dirTreeView->expandToDepth( level - 1 );
}


void MainWindow::showProgress( const QString & text )
{
    _ui->statusBar->showMessage( text, _statusBarTimeout );
}


void MainWindow::showCurrent( FileInfo * item )
{
    if ( item )
    {
	_ui->statusBar->showMessage( QString( "%1  (%2)" )
				     .arg( item->debugUrl() )
				     .arg( formatSize( item->totalSize() ) ) );
    }
    else
    {
	_ui->statusBar->clearMessage();
    }
}


void MainWindow::showSummary()
{
    FileInfoSet sel = _selectionModel->selectedItems();
    int count = sel.size();

    if ( count <= 1 )
	showCurrent( _selectionModel->currentItem() );
    else
    {
	sel = sel.normalized();

	_ui->statusBar->showMessage( tr( "%1 items selected (%2 total)" )
				     .arg( count )
				     .arg( formatSize( sel.totalSize() ) ) );
    }
}


void MainWindow::updateFileDetailsView()
{
    if ( _ui->fileDetailsView->isVisible() )
    {
	FileInfoSet sel = _selectionModel->selectedItems();

	if ( sel.isEmpty() )
	    _ui->fileDetailsView->showDetails( _selectionModel->currentItem() );
	else
	{
	    if ( sel.count() == 1 )
		_ui->fileDetailsView->showDetails( sel.first() );
	    else
		_ui->fileDetailsView->showDetails( sel );
	}
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
    FileInfo * currentItem = _selectionModel->currentItem();

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


void MainWindow::navigateUp()
{
    FileInfo * currentItem = _selectionModel->currentItem();

    if ( currentItem && currentItem->parent() &&
	 currentItem->parent() != _dirTreeModel->tree()->root() )
    {
	_selectionModel->setCurrentItem( currentItem->parent(),
					 true ); // select
    }
}


void MainWindow::navigateToToplevel()
{
    FileInfo * toplevel = _dirTreeModel->tree()->firstToplevel();

    if ( toplevel )
    {
	expandTreeToLevel( 1 );
	_selectionModel->setCurrentItem( toplevel,
					 true ); // select
    }
}


void MainWindow::moveToTrash()
{
    FileInfoSet selectedItems = _selectionModel->selectedItems().normalized();

    // Prepare output window

    OutputWindow * outputWindow = new OutputWindow( qApp->activeWindow() );
    CHECK_NEW( outputWindow );

    // Prepare refresher

    FileInfoSet refreshSet = Refresher::parents( selectedItems );
    _selectionModel->prepareRefresh( refreshSet );
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
    _configDialog->cleanupConfigPage()->setCleanupCollection( _cleanupCollection );
    _configDialog->mimeCategoryConfigPage()->setMimeCategorizer( _mimeCategorizer );

    if ( ! _configDialog->isVisible() )
    {
	_configDialog->setup();
	_configDialog->show();
    }
}


void MainWindow::showElapsedTime()
{
    showProgress( tr( "Reading... %1" )
		  .arg( formatTime( _stopWatch.elapsed(), false ) ) );
}


void MainWindow::showFileTypeStats()
{
    if ( ! _fileTypeStatsWindow )
    {
	// This deletes itself when the user closes it. The associated QPointer
	// keeps track of that and sets the pointer to 0 when it happens.

	_fileTypeStatsWindow = new QDirStat::FileTypeStatsWindow( _selectionModel,
								  this );
    }

    _fileTypeStatsWindow->populate( selectedDirOrRoot() );
    _fileTypeStatsWindow->show();
}


void MainWindow::showFileSizeStats()
{
    FileInfo * sel = selectedDirOrRoot();

    if ( ! sel || ! sel->hasChildren() )
	return;

    FileSizeStatsWindow::populateSharedInstance( sel );
}


void MainWindow::initLayoutActions()
{
    // Qt Designer does not support QActionGroups; it was there for Qt 3, but
    // they dropped that feature for Qt 4/5.

    _layoutActionGroup = new QActionGroup( this );
    CHECK_NEW( _layoutActionGroup );

    _layoutActionGroup->addAction( _ui->actionLayout1 );
    _layoutActionGroup->addAction( _ui->actionLayout2 );
    _layoutActionGroup->addAction( _ui->actionLayout3 );

    _ui->actionLayout1->setData( "L1" );
    _ui->actionLayout2->setData( "L2" );
    _ui->actionLayout3->setData( "L3" );
}


void MainWindow::createLayouts()
{
    TreeLayout * layout;

    layout = new TreeLayout( "L1" );
    CHECK_NEW( layout );
    _layouts[ "L1" ] = layout;

    layout = new TreeLayout( "L2" );
    CHECK_NEW( layout );
    _layouts[ "L2" ] = layout;

    layout = new TreeLayout( "L3" );
    CHECK_NEW( layout );
    _layouts[ "L3" ] = layout;

    layout->showDetailsPanel = false;
}


void MainWindow::changeLayout( const QString & name )
{
    _layoutName = name;

    if ( _layoutName.isEmpty() )
    {
	// Get the layout to use from data() from the QAction that sent the signal.

	QAction * action   = qobject_cast<QAction *>( sender() );
	_layoutName = action && action->data().isValid() ?
	    action->data().toString() : "L2";
    }

    logDebug() << "Changing to layout " << _layoutName << endl;

    _ui->dirTreeView->headerTweaker()->changeLayout( _layoutName );

    if ( _currentLayout )
	saveLayout( _currentLayout );

    if ( _layouts.contains( _layoutName ) )
    {
	_currentLayout = _layouts[ _layoutName ];
	applyLayout( _currentLayout );
    }
    else
    {
	logError() << "No layout " << _layoutName << endl;
    }
}


void MainWindow::saveLayout( TreeLayout * layout )
{
    CHECK_PTR( layout );

    layout->showCurrentPath  = _ui->actionShowCurrentPath->isChecked();
    layout->showDetailsPanel = _ui->actionShowDetailsPanel->isChecked();
}


void MainWindow::applyLayout( TreeLayout * layout )
{
    CHECK_PTR( layout );

    _ui->actionShowCurrentPath->setChecked ( layout->showCurrentPath  );
    _ui->actionShowDetailsPanel->setChecked( layout->showDetailsPanel );
}


FileInfo * MainWindow::selectedDirOrRoot() const
{
    FileInfoSet selectedItems = _selectionModel->selectedItems();
    FileInfo * sel = selectedItems.first();

    if ( ! sel )
	sel = _dirTreeModel->tree()->firstToplevel();

    return sel;
}


void MainWindow::toggleVerboseSelection()
{
    // Verbose selection is toggled with Shift-F7

    _verboseSelection = _ui->actionVerboseSelection->isChecked();

    if ( _selectionModel )
	_selectionModel->setVerbose( _verboseSelection );

    logInfo() << "Verbose selection is now " << ( _verboseSelection ? "on" : "off" )
	      << ". Change this with Shift-F7." << endl;
}


void MainWindow::showHelp()
{
    SysUtil::openInBrowser( "https://github.com/shundhammer/qdirstat/blob/master/README.md" );
}


void MainWindow::showPkgViewHelp()
{
    SysUtil::openInBrowser( "https://github.com/shundhammer/qdirstat/blob/master/doc/Pkg-View.md" );
}


void MainWindow::showWhatsNew()
{
    SysUtil::openInBrowser( RELEASE_URL ); // defined in Version.h
}


void MainWindow::showAboutDialog()
{
    QString homePage = "https://github.com/shundhammer/qdirstat";
    QString mailTo   = "qdirstat@gmx.de";

    QString text = "<h2>QDirStat " QDIRSTAT_VERSION "</h2>";
    text += "<p>";
    text += tr( "Qt-based directory statistics -- showing where all your disk space has gone "
		" and trying to help you to clean it up." );
    text += "</p><p>";
    text += "(c) 2015-2019 Stefan Hundhammer";
    text += "</p><p>";
    text += tr( "Contact: " ) + QString( "<a href=\"mailto:%1\">%2</a>" ).arg( mailTo ).arg( mailTo );
    text += "</p><p>";
    text += QString( "<p><a href=\"%1\">%2</a></p>" ).arg( homePage ).arg( homePage );
    text += tr( "License: GPL V2 (GNU General Public License Version 2)" );
    text += "</p><p>";
    text += tr( "This is free Open Source software, provided to you hoping that it might be "
		"useful for you. It does not cost you anything, but on the other hand there "
		"is no warranty or promise of anything." );
    text += "</p><p>";
    text += tr( "This software was made with the best intentions and greatest care, but still "
		"there is the off chance that something might go wrong which might damage "
		"data on your computer. Under no circumstances will the authors of this program "
		"be held responsible for anything like that. Use this program at your own risk." );
    text += "</p>";

    QMessageBox::about( this, tr( "About QDirStat" ), text );
}


void MainWindow::showDonateDialog()
{
    QString dUrl = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=EYJXAVLGNRR5W";

    QString text = "<h2>Donate</h2>";
    text += "<p>";
    text += tr( "QDirStat is Free Open Source Software. "
		"You are not required to pay anything. "
		"Donations are most welcome, of course." );
    text += "</p><p>";
    text += tr( "You can donate any amount of your choice:" );
    text += "</p><p>";
    text += QString( "<a href=\"%1\">QDirStat at PayPal</a>" ).arg(_dUrl );
    text += "</p><p>";
    text += tr( "(external browser window)" );
    text += "</p>";

    QMessageBox::about( this, tr( "Donate" ), text );
}




//---------------------------------------------------------------------------
//			       Debugging Helpers
//---------------------------------------------------------------------------


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

    // _dirTreeModel->dumpPersistentIndexList();
}


void MainWindow::selectionChanged()
{
    showSummary();
    updateFileDetailsView();

    if ( _verboseSelection )
    {
	logDebug() << endl;
	_selectionModel->dumpSelectedItems();
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
	_selectionModel->dumpSelectedItems();
    }
}


QString MainWindow::formatTime( qint64 millisec, bool showMillisec )
{
    QString formattedTime;
    int hours;
    int min;
    int sec;

    hours	= millisec / 3600000L;	// 60*60*1000
    millisec	%= 3600000L;

    min		= millisec / 60000L;	// 60*1000
    millisec	%= 60000L;

    sec		= millisec / 1000L;
    millisec	%= 1000L;

    if ( hours < 1 && min < 1 && sec < 60 )
    {
	if ( showMillisec )
	    formattedTime.sprintf ( "%2d.%03lld ", sec, millisec );
	else
	    formattedTime.sprintf ( "%2d ", sec );
	    // formattedTime.sprintf ( "%2d.%1lld ", sec, millisec / 100 );

	formattedTime += tr( "sec" );
    }
    else
    {
	formattedTime.sprintf ( "%02d:%02d:%02d", hours, min, sec );
    }

    return formattedTime;
}
