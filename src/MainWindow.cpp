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

#include "MainWindow.h"
#include "Logger.h"
#include "Exception.h"
#include "ExcludeRules.h"
#include "DirTree.h"
#include "DirTreeCache.h"
#include "DebugHelpers.h"


using namespace QDirStat;


MainWindow::MainWindow():
    QMainWindow(),
    _ui( new Ui::MainWindow ),
    _modified( false ),
    _statusBarTimeOut( 3000 ), // millisec
    _treeLevelMapper(0)
{
    _ui->setupUi( this );
    _dirTreeModel = new QDirStat::DirTreeModel( this );
    CHECK_NEW( _dirTreeModel );

    _ui->dirTreeView->setModel( _dirTreeModel );
    _ui->dirTreeView->setSortingEnabled( true );

    QHeaderView * header = _ui->dirTreeView->header();
    CHECK_PTR( header );

    header->setSortIndicator( QDirStat::NameCol, Qt::AscendingOrder );
    header->setStretchLastSection( false );

    // TO DO: This is too strict. But it's better than the brain-dead defaults.
    header->setSectionResizeMode( QHeaderView::ResizeToContents );
    _ui->dirTreeView->setRootIsDecorated( true );

    int col = _dirTreeModel->viewCol( QDirStat::PercentBarCol );
    _percentBarDelegate = new PercentBarDelegate( _ui->dirTreeView, col );
    CHECK_NEW( _percentBarDelegate );
    _ui->dirTreeView->setItemDelegate( _percentBarDelegate );


    connect( _dirTreeModel->tree(),	SIGNAL( finished()	  ),
	     this,			SLOT  ( readingFinished() ) );

    connect( _dirTreeModel->tree(),	SIGNAL( startingReading() ),
	     this,			SLOT  ( updateActions()	  ) );

    connect( _dirTreeModel->tree(),	SIGNAL( finished()	  ),
	     this,			SLOT  ( updateActions()	  ) );

    connect( _dirTreeModel->tree(),	SIGNAL( aborted()	  ),
	     this,			SLOT  ( updateActions()	  ) );

    connect( _dirTreeModel->tree(),	SIGNAL( progressInfo( QString ) ),
	     this,			SLOT  ( showProgress( QString ) ) );

    connect( _ui->dirTreeView,		SIGNAL( clicked	   ( QModelIndex ) ),
	     this,			SLOT  ( itemClicked( QModelIndex ) ) );

    connectActions();

    ExcludeRules::add( ".*/\\.snapshot$" );
#if 0
    ExcludeRules::add( ".*/\\.git$" );
#endif

    updateActions();
}


MainWindow::~MainWindow()
{
    // Relying on the QObject hierarchy to properly clean this up resulted in a
    //  segfault; there was probably a problem in the deletion order.
    delete _percentBarDelegate;
}


void MainWindow::connectActions()
{
    //
    // "File" menu
    //

    connect( _ui->actionOpen,		SIGNAL( triggered()  ),
	     this,			SLOT  ( askOpenUrl() ) );

    connect( _ui->actionRefreshAll,	SIGNAL( triggered()  ),
	     this,			SLOT  ( refreshAll() ) );

    connect( _ui->actionStopReading,	SIGNAL( triggered()   ),
	     this,			SLOT  ( stopReading() ) );

    connect( _ui->actionAskWriteCache,	SIGNAL( triggered()	),
	     this,			SLOT  ( askWriteCache() ) );

    connect( _ui->actionAskReadCache,	SIGNAL( triggered()	),
	     this,			SLOT  ( askReadCache()	) );

    connect( _ui->actionQuit,		SIGNAL( triggered() ),
	     qApp,			SLOT  ( quit()	    ) );

    //
    // "View" menu
    //

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
}


void MainWindow::mapTreeExpandAction( QAction * action, int level )
{
    connect( action,	       SIGNAL( triggered() ),
	     _treeLevelMapper, SLOT  ( map()	   ) );
    _treeLevelMapper->setMapping( action, level );
}


void MainWindow::updateActions()
{
    bool reading = _dirTreeModel->tree()->isBusy();

    _ui->actionStopReading->setEnabled( reading );
    _ui->actionRefreshAll->setEnabled	( ! reading );
    _ui->actionAskReadCache->setEnabled ( ! reading );
    _ui->actionAskWriteCache->setEnabled( ! reading );
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


void MainWindow::notImplemented()
{
    QMessageBox::warning( this, tr( "Error" ), tr( "Not implemented!" ) );
}


void MainWindow::readingFinished()
{
    logDebug() << endl;
    _ui->statusBar->showMessage( tr( "Ready.") );
    expandTreeToLevel( 1 );
    int sortCol = _dirTreeModel->viewCol( QDirStat::TotalSizeCol );
    _ui->dirTreeView->sortByColumn( sortCol, Qt::DescendingOrder );

    // Debug::dumpModelTree( _dirTreeModel, QModelIndex(), "" );
}


void MainWindow::itemClicked( const QModelIndex & index )
{
    if ( index.isValid() )
    {
	logDebug() << "Clicked row #" << index.row()
		   << " col #" << index.column()
		   << " data(0): " << index.model()->data( index, 0 ).toString()
		   << endl;
	logDebug() << "Ancestors: " << Debug::modelTreeAncestors( index ).join( " -> " ) << endl;
    }
    else
    {
	logDebug() << "Invalid model index" << endl;
    }

    // _dirTreeModel->dumpPersistentIndexList();
}

