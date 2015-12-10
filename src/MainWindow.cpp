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
#if 0
    _sortModel = new QSortFilterProxyModel( this );
    _sortModel->setSourceModel( _dirTreeModel );
    _sortModel->setSortRole( QDirStat::SortRole );

    _ui->dirTreeView->setModel( _sortModel );
    _ui->dirTreeView->setSortingEnabled( true );
    _ui->dirTreeView->header()->setSortIndicator( QDirStat::DirTreeModel::NameCol, Qt::AscendingOrder );
#else
    logDebug() << "No sort model" << endl;
    _ui->dirTreeView->setModel( _dirTreeModel );
#endif
    _ui->dirTreeView->setRootIsDecorated( true );


    connect( _dirTreeModel->tree(),	SIGNAL( finished()   ),
	     this,			SLOT  ( expandTree() ) );

    connect( _ui->dirTreeView,		SIGNAL( clicked	   ( QModelIndex ) ),
	     this,			SLOT  ( itemClicked( QModelIndex ) ) );


    connect( _ui->actionQuit,		SIGNAL( triggered() ),
	     qApp,			SLOT  ( quit()	    ) );

    // DEBUG
    // DEBUG
    // DEBUG
    ExcludeRules::add( ".*/\\.git$" );
    _dirTreeModel->openUrl( ".." );
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
    QString cacheName( "/tmp/test-qdirstat.cache.gz" );
    CacheWriter writer( cacheName, _dirTreeModel->tree() );
    logDebug() << "Cache file written to " << cacheName << " ok: " << writer.ok() << endl;

#if 0
    logDebug() << "Setting model" << endl;
    _ui->dirTreeView->setModel( _dirTreeModel );
#endif

    logDebug() << "Expanding tree" << endl;
    _ui->dirTreeView->expandToDepth( 3 ); // TO DO

    dumpModelTree( QModelIndex(), "" );
}


void MainWindow::itemClicked( const QModelIndex & index )
{
    logDebug() << "Clicked row #" << index.row()
	       << " col #" << index.column()
	       << " item " << (FileInfo *) index.internalPointer()
	       << " data(0): " << _dirTreeModel->data( index, 0 ).toString()
	       << endl;
}


void MainWindow::dumpModelTree( const QModelIndex & index, const QString & indent )
{
    QAbstractItemModel * model = _dirTreeModel;

    int rowCount = model->rowCount( index );
    QVariant data = model->data( index, Qt::DisplayRole );

    if ( data.isValid() )
    {
	if ( rowCount > 0 )
	    logDebug() << indent << data.toString() << ": " << rowCount << " rows" << endl;
	else
	    logDebug() << indent << data.toString() << endl;
    }
    else
    {
	logDebug() << "<No data> " << rowCount << " rows" << endl;
    }

    for ( int row=0; row < rowCount; row++ )
    {
	QModelIndex childIndex = model->index( row, 0, index );
	dumpModelTree( childIndex, indent + "	 " );
    }
}
