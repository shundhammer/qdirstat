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
#include "Exception.h"
#include "ExcludeRules.h"
#include "FileInfo.h"
#include "Logger.h"
#include "MimeCategorizer.h"
#include "MimeCategoryConfigPage.h"
#include "OutputWindow.h"
#include "Refresher.h"
#include "SelectionModel.h"
#include "Settings.h"
#include "Trash.h"
#include "Version.h"


using namespace QDirStat;

using QDirStat::DataColumns;
using QDirStat::DirTreeModel;
using QDirStat::SelectionModel;
using QDirStat::CleanupCollection;


MainWindow::MainWindow():
    QMainWindow(),
    _ui( new Ui::MainWindow ),
    _configDialog(0),
    _modified( false ),
    _verboseSelection( false ),
    _statusBarTimeout( 3000 ), // millisec
    _treeLevelMapper(0)
{
    CHECK_PTR( _ui );

    _ui->setupUi( this );
    ActionManager::instance()->addWidgetTree( this );
    readSettings();

    _dirTreeModel = new DirTreeModel( this );
    CHECK_NEW( _dirTreeModel );

    _selectionModel = new SelectionModel( _dirTreeModel, this );
    CHECK_NEW( _selectionModel );

    _ui->dirTreeView->setModel( _dirTreeModel );
    _ui->dirTreeView->setSelectionModel( _selectionModel );

    _ui->treemapView->setDirTree( _dirTreeModel->tree() );
    _ui->treemapView->setSelectionModel( _selectionModel );

    _dirTreeModel->setSelectionModel( _selectionModel );

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

#ifdef Q_OS_MACX
    // this makes the application to look like more "native" on macOS
    setUnifiedTitleAndToolBarOnMac(true);
    _ui->toolBar->setMovable(false);
#endif

    connect( _selectionModel,  SIGNAL( currentBranchChanged( QModelIndex ) ),
	     _ui->dirTreeView, SLOT  ( closeAllExcept	   ( QModelIndex ) ) );

    connect( _dirTreeModel->tree(),	SIGNAL( startingReading() ),
	     this,			SLOT  ( startingReading() ) );

    connect( _dirTreeModel->tree(),	SIGNAL( finished()	  ),
	     this,			SLOT  ( readingFinished() ) );

    connect( _dirTreeModel->tree(),	SIGNAL( aborted()	  ),
	     this,			SLOT  ( readingAborted()  ) );

    connect( _selectionModel,  SIGNAL( selectionChanged() ),
	     this,	       SLOT  ( updateActions()	 ) );

    connect( _selectionModel,  SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,	       SLOT  ( updateActions()				   ) );

    connect( _ui->treemapView, SIGNAL( treemapChanged() ),
	     this,	       SLOT  ( updateActions()	 ) );

    connect( _cleanupCollection, SIGNAL( startingCleanup( QString ) ),
	     this,		 SLOT  ( startingCleanup( QString ) ) );

    connect( _cleanupCollection, SIGNAL( cleanupFinished( int ) ),
	     this,		 SLOT  ( cleanupFinished( int ) ) );


    // Debug connections

    connect( _ui->dirTreeView, SIGNAL( clicked	  ( QModelIndex ) ),
	     this,	       SLOT  ( itemClicked( QModelIndex ) ) );

    connect( _selectionModel, SIGNAL( selectionChanged() ),
	     this,	      SLOT  ( selectionChanged() ) );

    connect( _selectionModel, SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,	      SLOT  ( currentItemChanged( FileInfo *, FileInfo * ) ) );

    connectActions();


    if ( ExcludeRules::instance()->isEmpty() )
    {
	ExcludeRules::add( QRegExp( ".snapshot",
				    Qt::CaseSensitive,
				    QRegExp::FixedString ) );
    }

    Debug::dumpExcludeRules();

    if ( ! _ui->actionShowTreemap->isChecked() )
	_ui->treemapView->disable();

    toggleVerboseSelection();
    updateActions();
}


MainWindow::~MainWindow()
{
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
}


#define CONNECT_ACTION(ACTION, RECEIVER, RCVR_SLOT) \
    connect( (ACTION), SIGNAL( triggered() ), (RECEIVER), SLOT( RCVR_SLOT ) )


void MainWindow::connectActions()
{
    // "File" menu

    CONNECT_ACTION( _ui->actionOpen,			    this, askOpenUrl()	    );
    CONNECT_ACTION( _ui->actionRefreshAll,		    this, refreshAll()	    );
    CONNECT_ACTION( _ui->actionRefreshSelected,		    this, refreshSelected() );
    CONNECT_ACTION( _ui->actionReadExcludedDirectory,	    this, refreshSelected() );
    CONNECT_ACTION( _ui->actionContinueReadingAtMountPoint, this, refreshSelected() );
    CONNECT_ACTION( _ui->actionStopReading,		    this, stopReading()	    );
    CONNECT_ACTION( _ui->actionAskWriteCache,		    this, askWriteCache()   );
    CONNECT_ACTION( _ui->actionAskReadCache,		    this, askReadCache()    );
    CONNECT_ACTION( _ui->actionQuit,			    qApp, quit()	    );



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

    CONNECT_ACTION( _ui->actionFileTypeStats,	   this, toggleFileTypeStats() );

    _ui->actionFileTypeStats->setShortcutContext( Qt::ApplicationShortcut );



    // "Edit" menu

    CONNECT_ACTION( _ui->actionCopyUrlToClipboard, this, copyCurrentUrlToClipboard() );
    CONNECT_ACTION( _ui->actionMoveToTrash,	   this, moveToTrash() );


    // "Go To" menu

    CONNECT_ACTION( _ui->actionGoUp,	     this, navigateUp() );
    CONNECT_ACTION( _ui->actionGoToToplevel, this, navigateToToplevel() );


    // "Treemap" menu

    connect( _ui->actionShowTreemap, SIGNAL( toggled( bool )   ),
	     this,		     SLOT  ( showTreemapView() ) );

    CONNECT_ACTION( _ui->actionTreemapZoomIn,	 _ui->treemapView, zoomIn()	    );
    CONNECT_ACTION( _ui->actionTreemapZoomOut,	 _ui->treemapView, zoomOut()	    );
    CONNECT_ACTION( _ui->actionResetTreemapZoom, _ui->treemapView, resetZoom()	    );
    CONNECT_ACTION( _ui->actionTreemapRebuild,	 _ui->treemapView, rebuildTreemap() );

    // "Settings" menu

    CONNECT_ACTION( _ui->actionConfigure, this, openConfigDialog() );


    // "Help" menu

    CONNECT_ACTION( _ui->actionHelp,	this, showHelp() );
    CONNECT_ACTION( _ui->actionAbout,	this, showAboutDialog() );
    CONNECT_ACTION( _ui->actionAboutQt, qApp, aboutQt() );


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
    bool reading = _dirTreeModel->tree()->isBusy();

    _ui->actionStopReading->setEnabled( reading );
    _ui->actionRefreshAll->setEnabled	( ! reading );
    _ui->actionAskReadCache->setEnabled ( ! reading );
    _ui->actionAskWriteCache->setEnabled( ! reading );

    bool haveCurrentItem = ( _selectionModel->currentItem() != 0 );
    bool treeNotEmpty	 = ( _dirTreeModel->tree()->firstToplevel() != 0 );

    _ui->actionCopyUrlToClipboard->setEnabled( haveCurrentItem );
    _ui->actionGoUp->setEnabled( haveCurrentItem );
    _ui->actionGoToToplevel->setEnabled( treeNotEmpty );

    FileInfoSet selectedItems = _selectionModel->selectedItems();
    FileInfo * sel = selectedItems.first();
    bool oneDirSelected = ( selectedItems.size() == 1 ) && ( sel->isDir() );

    _ui->actionMoveToTrash->setEnabled( sel && ! sel->isDotEntry() );
    _ui->actionRefreshSelected->setEnabled( oneDirSelected && ! sel->isExcluded() && ! sel->isMountPoint() );
    _ui->actionContinueReadingAtMountPoint->setEnabled( oneDirSelected && sel->isMountPoint() );
    _ui->actionReadExcludedDirectory->setEnabled      ( oneDirSelected && sel->isExcluded()   );

    bool showingTreemap = _ui->treemapView->isVisible();

    _ui->actionTreemapZoomIn->setEnabled   ( showingTreemap && _ui->treemapView->canZoomIn() );
    _ui->actionTreemapZoomOut->setEnabled  ( showingTreemap && _ui->treemapView->canZoomOut() );
    _ui->actionResetTreemapZoom->setEnabled( showingTreemap && _ui->treemapView->canZoomOut() );
    _ui->actionTreemapRebuild->setEnabled  ( showingTreemap );
}


void MainWindow::readSettings()
{
    QDirStat::Settings settings;
    settings.beginGroup( "MainWindow" );

    _statusBarTimeout	 = settings.value( "StatusBarTimeoutMillisec", 3000 ).toInt();
    bool   showTreemap	 = settings.value( "ShowTreemap"	     , true ).toBool();
    QPoint winPos	 = settings.value( "WindowPos"		     , QPoint( -99, -99 ) ).toPoint();
    QSize  winSize	 = settings.value( "WindowSize"		     , QSize (	 0,   0 ) ).toSize();
    _verboseSelection	 = settings.value( "VerboseSelection"	     , false ).toBool();

    settings.endGroup();

    _ui->actionShowTreemap->setChecked( showTreemap );
    _ui->actionVerboseSelection->setChecked( _verboseSelection );

    if ( winSize.height() > 100 && winSize.width() > 100 )
	resize( winSize );

    if ( winPos.x() != -99 && winPos.y() != -99 )
	move( winPos );

    ExcludeRules::instance()->readSettings();
}


void MainWindow::writeSettings()
{
    QDirStat::Settings settings;
    settings.beginGroup( "MainWindow" );

    settings.setValue( "StatusBarTimeoutMillisec", _statusBarTimeout );
    settings.setValue( "ShowTreemap"		 , _ui->actionShowTreemap->isChecked() );
    settings.setValue( "WindowPos"		 , pos()  );
    settings.setValue( "WindowSize"		 , size() );
    settings.setValue( "VerboseSelection"	 , _verboseSelection );

    settings.endGroup();
}


void MainWindow::showTreemapView()
{
    if ( _ui->actionShowTreemap->isChecked() )
	_ui->treemapView->enable();
    else
	_ui->treemapView->disable();
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

    // During reading, PercentBarCol contains the number of read jobs.

    int sortCol = QDirStat::DataColumns::toViewCol( QDirStat::PercentBarCol );
    _ui->dirTreeView->sortByColumn( sortCol, Qt::DescendingOrder );

    if ( ! _selectionModel->currentBranch() )
    {
	// Wait until the toplevel entry has some children, then expand to level 1
	QTimer::singleShot( 200, _ui->actionExpandTreeLevel1, SLOT( trigger() ) );
    }
}


void MainWindow::idleDisplay()
{
    updateActions();
    int sortCol = QDirStat::DataColumns::toViewCol( QDirStat::PercentNumCol );
    _ui->dirTreeView->sortByColumn( sortCol, Qt::DescendingOrder );

    if ( ! _selectionModel->currentBranch() )
    {
	logDebug() << "No current branch - expanding tree to level 1" << endl;
	expandTreeToLevel( 1 );
    }

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
    _ui->statusBar->showMessage( tr( "Finished. Elapsed time: %1")
				 .arg( formatTime( _stopWatch.elapsed() ) ) );

    // Debug::dumpModelTree( _dirTreeModel, QModelIndex(), "" );
}


void MainWindow::readingAborted()
{
    logInfo() << endl;

    idleDisplay();
    _ui->statusBar->showMessage( tr( "Aborted. Elapsed time: %1")
				 .arg( formatTime( _stopWatch.elapsed() ) ) );
}


void MainWindow::openUrl( const QString & url )
{
    try
    {
	_dirTreeModel->openUrl( url );
    }
    catch ( const SysCallFailedException & ex )
    {
        CAUGHT( ex );

	QMessageBox errorPopup( QMessageBox::Warning,   // icon
                                tr( "Error" ),		// title
                                tr( "Could not open directory %1" ).arg( ex.resourceName() ), // text
                                QMessageBox::Ok,	// buttons
                                this );                 // parent
	errorPopup.setDetailedText( ex.what() );
	errorPopup.exec();
        askOpenUrl();
    }

    updateActions();
    expandTreeToLevel( 1 );
}


void MainWindow::askOpenUrl()
{
    QString url = QFileDialog::getExistingDirectory( this, // parent
						     tr("Select directory to scan") );
    if ( ! url.isEmpty() )
	openUrl( url );
}


void MainWindow::refreshAll()
{
    QString url = _dirTreeModel->tree()->url();

    if ( ! url.isEmpty() )
    {
	logDebug() << "Refreshing " << url << endl;
	_dirTreeModel->openUrl( url );
	updateActions();
    }
    else
    {
	askOpenUrl();
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
	_ui->statusBar->showMessage( tr( "Reading aborted." ) );
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

	QString msg = ok ? tr( "Directory tree written to file %1" ).arg( fileName ) :
			   tr( "ERROR writing cache file %1").arg( fileName );
	_ui->statusBar->showMessage( msg, _statusBarTimeout );
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


void MainWindow::copyCurrentUrlToClipboard()
{
    FileInfo * currentItem = _selectionModel->currentItem();

    if ( currentItem )
    {
	QClipboard * clipboard = QApplication::clipboard();
	QString url = currentItem->url();
	clipboard->setText( url );
	showProgress( tr( "Copied to system clipboard: %1" ).arg( url ) );
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
	bool success = Trash::trash( item->url() );

	if ( success )
	    outputWindow->addStdout( tr( "Moved to trash: %1" ).arg( item->url() ) );
	else
	    outputWindow->addStderr( tr( "Move to trash failed for %1" ).arg( item->url() ) );
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


void MainWindow::toggleFileTypeStats()
{
    if ( _fileTypeStatsWindow )
    {
        _fileTypeStatsWindow->deleteLater();
        // No need to set the variable to 0 - it is a guarded QPointer
    }
    else
    {
        // This deletes itself when the user closes it. The associated QPointer
        // keeps track of that and sets the pointer to 0 when it happens.

        _fileTypeStatsWindow = new QDirStat::FileTypeStatsWindow( _dirTreeModel->tree(),
                                                                  _selectionModel,
                                                                  this );
        _fileTypeStatsWindow->show();
    }
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
    QString helpUrl = "https://github.com/shundhammer/qdirstat/blob/master/README.md";
    QString program = "/usr/bin/xdg-open";
    logInfo() << "Starting " << program << " " << helpUrl << endl;
    QProcess::startDetached( program, QStringList() << helpUrl );
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
    text += "(c) 2015-2017 Stefan Hundhammer";
    text += "</p><p>";
    text += tr( "Contact: " ) + QString( "<a href=\"mailto:%1\">%2</a>" ).arg( mailTo ).arg( mailTo );
    text += "</p><p>";
    text += QString( "<p><a href=\"%1\">%2</a></p>" ).arg( homePage ).arg( homePage );
    text += tr( "License: GPL V2 (GNU Public License Version 2)" );
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

    if ( _verboseSelection )
    {
	logDebug() << endl;
	_selectionModel->dumpSelectedItems();
    }
}


void MainWindow::currentItemChanged( FileInfo * newCurrent, FileInfo * oldCurrent )
{
    showSummary();

    if ( _verboseSelection )
    {
	logDebug() << "new current: " << newCurrent << endl;
	logDebug() << "old current: " << oldCurrent << endl;
	_selectionModel->dumpSelectedItems();
    }
}


QString MainWindow::formatTime( qint64 millisec )
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
	formattedTime.sprintf ( "%2d.%03lld ", sec, millisec );
	formattedTime += tr( "sec" );
    }
    else
    {
	formattedTime.sprintf ( "%02d:%02d:%02d", hours, min, sec );
    }

    return formattedTime;
}
