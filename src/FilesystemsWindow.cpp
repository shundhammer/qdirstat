/*
 *   File name: FilesystemsWindow.cpp
 *   Summary:	QDirStat "Mounted Filesystems" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QFileIconProvider>

#include "FilesystemsWindow.h"
#include "MountPoints.h"
#include "SettingsHelpers.h"
#include "HeaderTweaker.h"
#include "PanelMessage.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


FilesystemsWindow::FilesystemsWindow( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FilesystemsWindow )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this );
    initWidgets();
    readWindowSettings( this, "FilesystemsWindow" );
}


FilesystemsWindow::~FilesystemsWindow()
{
    writeWindowSettings( this, "FilesystemsWindow" );
}


void FilesystemsWindow::populate()
{
    QFileIconProvider iconProvider;
    clear();

    foreach ( MountPoint * mountPoint, MountPoints::normalMountPoints() )
    {
	CHECK_PTR( mountPoint);

	FilesystemItem * item = new FilesystemItem( mountPoint, _ui->fsTree );
	CHECK_NEW( item );

	QIcon icon = iconProvider.icon( mountPoint->isNetworkMount() ?
					 QFileIconProvider::Network :
					 QFileIconProvider::Drive      );
	item->setIcon( 0, icon );
    }

    if ( MountPoints::hasBtrfs() )
	showBtrfsFreeSizeWarning();
}


void FilesystemsWindow::showBtrfsFreeSizeWarning()
{
    PanelMessage * msg = new PanelMessage( _ui->messagePanel );
    CHECK_NEW( msg );

    msg->setHeading( tr( "Btrfs free size information is misleading!" ) );
    msg->setText( tr( "Snapshots and copy-on-write may consume additional disk space." ) );
    msg->setDetailsUrl( "https://github.com/shundhammer/qdirstat/blob/master/doc/Btrfs-Free-Size.md" );
    _ui->messagePanel->add( msg );
}


void FilesystemsWindow::refresh()
{
    populate();
}


void FilesystemsWindow::reject()
{
    deleteLater();
}


void FilesystemsWindow::clear()
{
    _ui->fsTree->clear();
    _ui->messagePanel->clear();
}


void FilesystemsWindow::initWidgets()
{
    QStringList headers;
    headers << tr( "Device" )
	    << tr( "Mount Point" )
	    << tr( "Type" );

    if ( MountPoints::hasSizeInfo() )
    {
	headers << tr( "Size" )
		<< tr( "Used" )
		<< ""
		<< tr( "Free for Users" )
		<< ""
		<< tr( "Free for Root"	)
		<< ""
	    ;
    }

    _ui->fsTree->setHeaderLabels( headers );
    _ui->fsTree->header()->setStretchLastSection( false );

    // Right-align the numeric columns in the header

    QTreeWidgetItem * hItem = _ui->fsTree->headerItem();

    for ( int col = FS_TotalSizeCol; col < headers.size(); ++col )
	hItem->setTextAlignment( col, Qt::AlignRight );

    HeaderTweaker::resizeToContents( _ui->fsTree->header() );
    _ui->fsTree->sortItems( FS_DeviceCol, Qt::AscendingOrder );
}




FilesystemItem::FilesystemItem( MountPoint  * mountPoint,
				QTreeWidget * parent ):
    QTreeWidgetItem( parent ),
    _device	    ( mountPoint->device()	    ),
    _mountPath	    ( mountPoint->path()	    ),
    _fsType	    ( mountPoint->filesystemType()  ),
    _totalSize	    ( mountPoint->totalSize()	    ),
    _usedSize	    ( mountPoint->usedSize()	    ),
    _freeSizeForUser( mountPoint->freeSizeForUser() ),
    _freeSizeForRoot( mountPoint->freeSizeForRoot() ),
    _isNetworkMount ( mountPoint->isNetworkMount()  )
{
    QString blanks = QString( 3, ' ' );

    setText( FS_DeviceCol,    _device + blanks );
    setText( FS_MountPathCol, _mountPath );
    setText( FS_TypeCol,      _fsType	 );

    if ( parent->columnCount() >= FS_TotalSizeCol )
    {
	blanks = QString( 8, ' ' ); // Enforce left margin

	setText( FS_TotalSizeCol,	blanks + formatSize( _totalSize	      ) );
	setText( FS_UsedSizeCol,	blanks + formatSize( _usedSize	      ) );
	setText( FS_FreeSizeForUserCol, blanks + formatSize( _freeSizeForUser ) );
	setText( FS_FreeSizeForRootCol, blanks + formatSize( _freeSizeForRoot ) );

	setText( FS_UsedPercentCol,	   formatPercentOfTotal( _usedSize	  ) );
	setText( FS_FreePercentForUserCol, formatPercentOfTotal( _freeSizeForUser ) );
	setText( FS_FreePercentForRootCol, formatPercentOfTotal( _freeSizeForRoot ) );

	for ( int col = FS_TotalSizeCol; col < parent->columnCount(); ++col )
	    setTextAlignment( col, Qt::AlignRight );
    }
}


bool FilesystemItem::operator<( const QTreeWidgetItem & rawOther ) const
{
    const FilesystemItem & other = dynamic_cast<const FilesystemItem &>( rawOther );

    int col = treeWidget() ? treeWidget()->sortColumn() : FS_DeviceCol;

    switch ( col )
    {
	case FS_DeviceCol:
	    if ( ! isNetworkMount() &&	 other.isNetworkMount() ) return true;
	    if ( isNetworkMount()   && ! other.isNetworkMount() ) return false;
	    return device() < other.device();

	case FS_MountPathCol:		return mountPath()	 < other.mountPath();
	case FS_TypeCol:		return fsType()		 < other.fsType();
	case FS_TotalSizeCol:		return totalSize()	 < other.totalSize();
	case FS_UsedSizeCol:		return usedSize()	 < other.usedSize();
	case FS_FreeSizeForUserCol:	return freeSizeForUser() < other.freeSizeForUser();
	case FS_FreeSizeForRootCol:	return freeSizeForRoot() < other.freeSizeForRoot();
	default:			return QTreeWidgetItem::operator<( rawOther );
    }
}


QString FilesystemItem::formatPercentOfTotal( FileSize partialSize ) const
{
    return formatPercent( (100.0 * partialSize) / _totalSize );
}


QString FilesystemItem::formatPercent( float percent )
{
    if ( percent < 0.0 )	// Invalid percentage?
	return "";

    QString text;
    text.setNum( percent, 'f', 1 );
    text += "%";

    return text;
}

