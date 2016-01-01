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
#include <QFileDialog>
#include <QSignalMapper>
#include <QClipboard>

#include "MainWindow.h"
#include "Logger.h"
#include "Exception.h"
#include "ExcludeRules.h"
#include "DirTree.h"
#include "DirTreeCache.h"
#include "DataColumns.h"
#include "DebugHelpers.h"
#include "Version.h"


using namespace QDirStat;

using QDirStat::DataColumns;
using QDirStat::DirTreeModel;
using QDirStat::SelectionModel;


MainWindow::MainWindow():
    QMainWindow(),
    _ui( new Ui::MainWindow ),
    _modified( false ),
    _statusBarTimeOut( 3000 ), // millisec
    _treeLevelMapper(0)
{
    _ui->setupUi( this );
    _dirTreeModel = new DirTreeModel( this );
    CHECK_NEW( _dirTreeModel );

    _selectionModel = new SelectionModel( _dirTreeModel, this );
    CHECK_NEW( _selectionModel );

    _ui->dirTreeView->setModel( _dirTreeModel );
    _ui->dirTreeView->setSelectionModel( _selectionModel );

    _ui->treemapView->setDirTree( _dirTreeModel->tree() );
    _ui->treemapView->setSelectionModel( _selectionModel );


    connect( _dirTreeModel->tree(),	SIGNAL( startingReading() ),
	     this,			SLOT  ( startingReading() ) );

    connect( _dirTreeModel->tree(),	SIGNAL( finished()	  ),
	     this,			SLOT  ( readingFinished() ) );

    connect( _dirTreeModel->tree(),	SIGNAL( aborted()	  ),
	     this,			SLOT  ( readingAborted()  ) );

    connect( _dirTreeModel->tree(),	SIGNAL( progressInfo( QString ) ),
	     this,			SLOT  ( showProgress( QString ) ) );

    connect( _selectionModel,  SIGNAL( selectionChanged() ),
	     this,	       SLOT  ( updateActions()	 ) );

    connect( _selectionModel,  SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,	       SLOT  ( updateActions()				   ) );

    connect( _ui->treemapView, SIGNAL( treemapChanged() ),
	     this,	       SLOT  ( updateActions()	 ) );


    // Debug connections

    connect( _ui->dirTreeView, SIGNAL( clicked	  ( QModelIndex ) ),
	     this,	       SLOT  ( itemClicked( QModelIndex ) ) );

    connect( _selectionModel, SIGNAL( selectionChanged() ),
	     this,	      SLOT  ( selectionChanged() ) );

    connect( _selectionModel, SIGNAL( currentItemChanged( FileInfo *, FileInfo * ) ),
	     this,	      SLOT  ( currentItemChanged( FileInfo *, FileInfo * ) ) );

#if 0
    connect( _selectionModel, SIGNAL( currentChanged( QModelIndex, QModelIndex	) ),
	     this,	      SLOT  ( currentChanged( QModelIndex, QModelIndex	) ) );

    connect( _selectionModel, SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
	     this,	      SLOT  ( selectionChanged( QItemSelection, QItemSelection ) ) );
#endif

    connectActions();

    ExcludeRules::add( ".*/\\.snapshot$" );
#if 0
    ExcludeRules::add( ".*/\\.git$" );
#endif

    if ( ! _ui->actionShowTreemap->isChecked() )
	_ui->treemapView->disable();

    updateActions();
}


MainWindow::~MainWindow()
{
    // Relying on the QObject hierarchy to properly clean this up resulted in a
    //	segfault; there was probably a problem in the deletion order.
    delete _ui->dirTreeView;
    delete _selectionModel;
    delete _dirTreeModel;
}


#define CONNECT_ACTION(ACTION, RECEIVER, RCVR_SLOT) \
    connect( (ACTION), SIGNAL( triggered() ), (RECEIVER), SLOT( RCVR_SLOT ) )


void MainWindow::connectActions()
{
    // "File" menu

    CONNECT_ACTION( _ui->actionOpen,	      this, askOpenUrl()    );
    CONNECT_ACTION( _ui->actionRefreshAll,    this, refreshAll()    );
    CONNECT_ACTION( _ui->actionStopReading,   this, stopReading()   );
    CONNECT_ACTION( _ui->actionAskWriteCache, this, askWriteCache() );
    CONNECT_ACTION( _ui->actionAskReadCache,  this, askReadCache()  );
    CONNECT_ACTION( _ui->actionQuit,	      qApp, quit()	    );


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


    // "Edit" menu

    CONNECT_ACTION( _ui->actionCopyUrlToClipboard, this, copyCurrentUrlToClipboard() );


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

    // "Help" menu

    CONNECT_ACTION( _ui->actionAbout,	this, showAboutDialog() );
    CONNECT_ACTION( _ui->actionAboutQt, qApp, aboutQt() );
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

    bool showingTreemap = _ui->treemapView->isVisible();

    _ui->actionTreemapZoomIn->setEnabled   ( showingTreemap && _ui->treemapView->canZoomIn() );
    _ui->actionTreemapZoomOut->setEnabled  ( showingTreemap && _ui->treemapView->canZoomOut() );
    _ui->actionResetTreemapZoom->setEnabled( showingTreemap && _ui->treemapView->canZoomOut() );
    _ui->actionTreemapRebuild->setEnabled  ( showingTreemap );
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

    // Wait until the toplevel entry has some children, then expand to level 1
    QTimer::singleShot( 200, _ui->actionExpandTreeLevel1, SLOT( trigger() ) );
}


void MainWindow::idleDisplay()
{
    updateActions();
    expandTreeToLevel( 1 );
    int sortCol = QDirStat::DataColumns::toViewCol( QDirStat::PercentNumCol );
    _ui->dirTreeView->sortByColumn( sortCol, Qt::DescendingOrder );
    showTreemapView();
}


void MainWindow::startingReading()
{
    _stopWatch.start();
    busyDisplay();
}


void MainWindow::readingFinished()
{
    logDebug() << endl;

    idleDisplay();
    _ui->statusBar->showMessage( tr( "Finished. Elapsed time: %1")
				 .arg( formatTime( _stopWatch.elapsed() ) ) );

    // Debug::dumpModelTree( _dirTreeModel, QModelIndex(), "" );
}


void MainWindow::readingAborted()
{
    logDebug() << endl;

    idleDisplay();
    _ui->statusBar->showMessage( tr( "Aborted. Elapsed time: %1")
				 .arg( formatTime( _stopWatch.elapsed() ) ) );
}


void MainWindow::openUrl( const QString & url )
{
    _dirTreeModel->openUrl( url );
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


void MainWindow::stopReading()
{
    if ( _dirTreeModel->tree()->isBusy() )
    {
	_dirTreeModel->tree()->abortReading();
	_ui->statusBar->showMessage( tr( "Reading aborted." ) );
    }
}


void MainWindow::askReadCache()
{
    QString fileName = QFileDialog::getOpenFileName( this, // parent
						     tr( "Select QDirStat cache file" ),
						     DEFAULT_CACHE_NAME );
    if ( ! fileName.isEmpty() )
    {
	_dirTreeModel->clear();
	_dirTreeModel->tree()->readCache( fileName );
    }
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
	_ui->statusBar->showMessage( msg, _statusBarTimeOut );
    }
}


void MainWindow::expandTreeToLevel( int level )
{
    if ( level < 1 )
	_ui->dirTreeView->collapseAll();
    else
	_ui->dirTreeView->expandToDepth( level - 1 );
}


void MainWindow::showProgress( const QString & text )
{
    _ui->statusBar->showMessage( text, _statusBarTimeOut );
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


void MainWindow::showAboutDialog()
{
    QString homePage = "https://github.com/shundhammer/qdirstat";
    QString mailTo   = "qdirstat@gmx.de";

    QString text = "<h2>QDirStat " QDIRSTAT_VERSION "</h2>";
    text += "<p>";
    text += tr( "Qt-based directory statistics -- showing where all your disk space has gone "
		" and trying to help you to clean it up." );
    text += "</p><p>";
    text += "(c) 2015 Stefan Hundhammer";
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
    logDebug() << endl;
    showSummary();
    _selectionModel->dumpSelectedItems();
}


void MainWindow::currentItemChanged( FileInfo * newCurrent, FileInfo * oldCurrent )
{
    logDebug() << "new current: " << newCurrent << endl;
    logDebug() << "old current: " << oldCurrent << endl;
    showSummary();
    _selectionModel->dumpSelectedItems();
}


void MainWindow::currentChanged( const QModelIndex & newCurrent,
				 const QModelIndex & oldCurrent )
{
    logDebug() << "new current: " << newCurrent << endl;
    logDebug() << "old current: " << oldCurrent << endl;
    _selectionModel->dumpSelectedItems();
}


void MainWindow::selectionChanged( const QItemSelection & selected,
				   const QItemSelection & deselected )
{
    logDebug() << endl;
#if 0
    foreach ( const QModelIndex index, selected.indexes() )
    {
	logDebug() << "Selected: " << index << endl;
    }

    foreach ( const QModelIndex index, deselected.indexes() )
    {
	logDebug() << "Deselected: " << index << endl;
    }
#else
    Q_UNUSED( selected );
    Q_UNUSED( deselected );
#endif

    _selectionModel->dumpSelectedItems();
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
