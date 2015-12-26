/*
 *   File name: DirTreeView.cpp
 *   Summary:	Tree view widget for directory tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QHeaderView>

#include "DirTreeView.h"
#include "PercentBar.h"
#include "DirTree.h"
#include "Exception.h"
#include "Logger.h"


using namespace QDirStat;


DirTreeView::DirTreeView( QWidget * parent ):
    QTreeView( parent )
{
    _percentBarDelegate = new PercentBarDelegate( this );
    CHECK_NEW( _percentBarDelegate );

    setItemDelegate( _percentBarDelegate );

    setRootIsDecorated( true );
    setSortingEnabled( true );

    header()->setSortIndicator( NameCol, Qt::AscendingOrder );
    header()->setStretchLastSection( false );

    // TO DO: This is too strict. But it's better than the brain-dead defaults.
    header()->setSectionResizeMode( QHeaderView::ResizeToContents );
}


DirTreeView::~DirTreeView()
{
    delete _percentBarDelegate;
}
