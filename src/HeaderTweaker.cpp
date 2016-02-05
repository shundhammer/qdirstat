/*
 *   File name: HeaderTweaker.cpp
 *   Summary:	Helper class for DirTreeView
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QHeaderView>
#include <QMenu>
#include <QAction>

#include "HeaderTweaker.h"
#include "DirTreeView.h"
#include "DataColumns.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


HeaderTweaker::HeaderTweaker( QHeaderView * header, DirTreeView * parent ):
    QObject( parent ),
    _treeView( parent ),
    _header( header ),
    _allColumnsAutoSize(true)
{
    CHECK_PTR( _header );
    
    _header->setSortIndicator( NameCol, Qt::AscendingOrder );
    _header->setStretchLastSection( false );
    _header->setContextMenuPolicy( Qt::CustomContextMenu );
    
    setAllColumnsAutoSize( true );
    createActions();

    connect( _header, SIGNAL( sectionCountChanged( int, int ) ),
             this,    SLOT  ( initHeader()                    ) );
    
    connect( _header, SIGNAL( customContextMenuRequested( const QPoint & ) ),
	     this,    SLOT  ( contextMenu	        ( const QPoint & ) ) );
}


HeaderTweaker::~HeaderTweaker()
{
    
}


void HeaderTweaker::initHeader() const
{
    // Initialize stuff when the header actually has sections.

    logDebug() << "Header count: " << _header->count() << endl;
}


void HeaderTweaker::createActions()
{
    _actionAllColumnsAutoSize = new QAction( tr( "Auto Size for all Columns" ), this );
    _actionAllColumnsAutoSize->setCheckable( true );
    _actionAllColumnsAutoSize->setChecked( allColumnsAutoSize() );

    connect( _actionAllColumnsAutoSize, SIGNAL( toggled		     ( bool ) ),
	     this,			SLOT  ( setAllColumnsAutoSize( bool ) ) );
}


void HeaderTweaker::contextMenu( const QPoint & pos )
{
    int section = _header->logicalIndexAt( pos );
    DataColumn col = DataColumns::instance()->reverseMappedCol( static_cast<DataColumn>( section ) );
    QString colName = _treeView->model()->headerData( col,
                                                      Qt::Horizontal,
                                                      Qt::DisplayRole ).toString();
    QMenu menu;
    menu.addAction( QString( "Column \"%1\"" ).arg( colName ) );
    menu.addSeparator();
    menu.addSeparator();
    menu.addAction( _actionAllColumnsAutoSize );

    menu.exec( _header->mapToGlobal( pos ) );
}



void HeaderTweaker::setAllColumnsAutoSize( bool autoSize )
{
    _allColumnsAutoSize = autoSize;
    QHeaderView::ResizeMode resizeMode = autoSize ?
	QHeaderView::ResizeToContents :
	QHeaderView::Interactive;

#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
    _header->setResizeMode( resizeMode );
#else
    _header->setSectionResizeMode( resizeMode );
#endif
}


bool HeaderTweaker::allColumnsAutoSize() const
{
    return _allColumnsAutoSize;
}

