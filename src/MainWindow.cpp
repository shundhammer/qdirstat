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
#include <QSortFilterProxyModel>

#include "MainWindow.h"
#include "Logger.h"
#include "ExcludeRules.h"
#include "DirTree.h"
#include "DirTreeCache.h"


using namespace QDirStat;


MainWindow::MainWindow():
    QMainWindow(),
    _ui( new Ui::MainWindow ),
    _modified( false )
{
    _ui->setupUi( this );
    _dirTreeModel = new QDirStat::DirTreeModel( this );
#if 1
    _sortModel = new QSortFilterProxyModel( this );
    _sortModel->setSourceModel( _dirTreeModel );
    _sortModel->setSortRole( QDirStat::SortRole );

    _ui->dirTreeView->setModel( _sortModel );
#else
    _ui->dirTreeView->setModel( _dirTreeModel );
#endif
    _ui->dirTreeView->setSortingEnabled( true );
    _ui->dirTreeView->setRootIsDecorated( true );

    _ui->dirTreeView->header()->setSortIndicator( QDirStat::DirTreeModel::NameCol, Qt::AscendingOrder );
    _dirTreeModel->openUrl( ".." );

    connect( _dirTreeModel->tree(),	SIGNAL( finished()   ),
	     this,			SLOT  ( expandTree() ) );


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
    QString cacheName( "/tmp/qdirstat.cache.gz" );
    CacheWriter writer( cacheName, _dirTreeModel->tree() );
    logDebug() << "Cache file written to " << cacheName << " ok: " << writer.ok() << endl;
    logDebug() << "Expanding tree" << endl;
    _ui->dirTreeView->expandToDepth( 3 ); // TO DO
}
