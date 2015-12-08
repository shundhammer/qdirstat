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


using namespace QDirStat;


MainWindow::MainWindow():
    QMainWindow(),
    _ui( new Ui::MainWindow ),
    _modified( false )
{
    _ui->setupUi( this );
    _dirTreeModel = new QDirStat::DirTreeModel( this );
    _sortModel = new QSortFilterProxyModel( this );
    _sortModel->setSourceModel( _dirTreeModel );
    _sortModel->setSortRole( QDirStat::SortRole );

    _ui->dirTreeView->setModel( _sortModel );
    _ui->dirTreeView->setSortingEnabled( true );
    _ui->dirTreeView->setRootIsDecorated( true );
    _ui->dirTreeView->expandToDepth( 2 ); // TO DO

#if 0
    connect( _ui->dirTreeView->header(), SIGNAL( sortIndicatorChanged( int, Qt::SortOrder ) ),
	     _ui->dirTreeView,		 SLOT  ( sortByColumn	     ( int, Qt::SortOrder ) ) );
#endif
    _ui->dirTreeView->header()->setSortIndicator( QDirStat::DirTreeModel::NameCol, Qt::AscendingOrder );


    connect( _ui->actionQuit,		SIGNAL( triggered() ),
	     qApp,			SLOT  ( quit()	   ) );

    // DEBUG
    // DEBUG
    // DEBUG
    ExcludeRules::add( ".*/\\.git$" );
    // DEBUG
    // DEBUG
    // DEBUG

    _dirTreeModel->openUrl( ".." );
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

