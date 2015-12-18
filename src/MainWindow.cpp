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
#include <QSortFilterProxyModel>
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
    _treeLevelMapper(0)
{
    _ui->setupUi( this );
    _dirTreeModel = new QDirStat::DirTreeModel( this );
    CHECK_NEW( _dirTreeModel );

    _sortModel = new QSortFilterProxyModel( this );
    CHECK_NEW( _sortModel );

    _sortModel->setSourceModel( _dirTreeModel );
    _sortModel->setSortRole( QDirStat::SortRole );

    _ui->dirTreeView->setModel( _sortModel );
    _ui->dirTreeView->setSortingEnabled( true );

    QHeaderView * header = _ui->dirTreeView->header();
    CHECK_PTR( header );

    header->setSortIndicator( QDirStat::DirTreeModel::NameCol, Qt::AscendingOrder );
    header->setStretchLastSection( false );

    // TO DO: This is too strict. But it's better than the brain-dead defaults.
    header->setSectionResizeMode( QHeaderView::ResizeToContents );
    _ui->dirTreeView->setRootIsDecorated( true );


    connect( _dirTreeModel->tree(),	SIGNAL( finished()   ),
	     this,			SLOT  ( expandTree() ) );

    connect( _ui->dirTreeView,		SIGNAL( clicked	   ( QModelIndex ) ),
	     this,			SLOT  ( itemClicked( QModelIndex ) ) );

    connectActions();

    // DEBUG
    // DEBUG
    // DEBUG
    ExcludeRules::add( ".*/\\.git$" );
    // DEBUG
    // DEBUG
    // DEBUG
}


MainWindow::~MainWindow()
{
}


void MainWindow::connectActions()
{
    //
    // "File" menu
    //

    connect( _ui->actionOpen,		SIGNAL( triggered()  ),
	     this,			SLOT  ( askOpenUrl() ) );

    connect( _ui->actionQuit,		SIGNAL( triggered() ),
	     qApp,			SLOT  ( quit()	    ) );

    //
    // "View" menu
    //

    _treeLevelMapper = new QSignalMapper( this );

    connect( _treeLevelMapper, SIGNAL( mapped           ( int ) ),
             this,             SLOT  ( expandTreeToLevel( int ) ) );

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
    connect( action,           SIGNAL( triggered() ),
             _treeLevelMapper, SLOT  ( map()       ) );
    _treeLevelMapper->setMapping( action, level );
}


void MainWindow::openUrl( const QString & url )
{
    _dirTreeModel->openUrl( url );
}


void MainWindow::askOpenUrl()
{
    QString url = QFileDialog::getExistingDirectory( this, // parent
                                                     tr("Select directory to scan") );
    if ( ! url.isEmpty() )
        openUrl( url );
}


void MainWindow::expandTreeToLevel( int level )
{
    if ( level < 1 )
        _ui->dirTreeView->collapseAll();
    else
        _ui->dirTreeView->expandToDepth( level - 1 );
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


void MainWindow::notImplemented()
{
    QMessageBox::warning( this, tr( "Error" ), tr( "Not implemented!" ) );
}


void MainWindow::expandTree()
{
#if 1
    QString cacheName( "/tmp/test-qdirstat.cache.gz" );
    CacheWriter writer( cacheName, _dirTreeModel->tree() );
    logDebug() << "Cache file written to " << cacheName << " ok: " << writer.ok() << endl;
#endif

#if 1
    logDebug() << "Expanding tree" << endl;
    _ui->dirTreeView->expandToDepth( 1 ); // TO DO
#endif

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
}

