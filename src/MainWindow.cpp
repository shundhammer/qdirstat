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
    _modified( false )
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


    connect( _ui->actionOpen,		SIGNAL( triggered()  ),
	     this,			SLOT  ( askOpenUrl() ) );

    connect( _ui->actionQuit,		SIGNAL( triggered() ),
	     qApp,			SLOT  ( quit()	    ) );

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

