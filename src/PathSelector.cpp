/*
 *   File name: PathSelector.cpp
 *   Summary:	Path selection list widget for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QDir>

#include "PathSelector.h"
#include "MountPoints.h"
#include "Logger.h"
#include "Exception.h"

#define SHOW_SIZES_IN_TOOLTIP   0

using namespace QDirStat;



PathSelector::PathSelector( QWidget * parent ):
    QListWidget( parent )
{
    setSpacing( 3 );

    connect( this, SIGNAL( currentItemChanged( QListWidgetItem *, QListWidgetItem * ) ),
	     this, SLOT	 ( slotItemSelected  ( QListWidgetItem * ) ) );

    connect( this, SIGNAL( itemActivated        ( QListWidgetItem * ) ),
	     this, SLOT	 ( slotItemDoubleClicked( QListWidgetItem * ) ) );
}


PathSelector::~PathSelector()
{
    // NOP
}


PathSelectorItem * PathSelector::addPath( const QString & path,
                                          const QIcon &   icon )
{
    PathSelectorItem * item = new PathSelectorItem( path, this );
    CHECK_NEW( item );

    if ( ! icon.isNull() )
	item->setIcon( icon );

    return item;
}


PathSelectorItem * PathSelector::addPath( const QString & path,
                                          const QString & iconName )
{
    PathSelectorItem * item = new PathSelectorItem( path, this );
    CHECK_NEW( item );

    if ( ! iconName.isEmpty() )
    {
	QIcon icon = QIcon::fromTheme( iconName );

	if ( icon.isNull() )
	    logError() << "No theme icon with name \"" << iconName << "\"" << endl;
	else
	    item->setIcon( icon );
    }

    return item;
}


PathSelectorItem * PathSelector::addHomeDir()
{
    PathSelectorItem * item = addPath( QDir::homePath(), "folder-home" );
    item->setToolTip( tr( "Your home directory" ) );

    return item;
}


PathSelectorItem * PathSelector::addMountPoint( const MountPoint * mountPoint )
{
    PathSelectorItem * item = new PathSelectorItem( mountPoint, this );
    CHECK_NEW( item );

    QString iconName = "drive-harddisk";
    QIcon icon = QIcon::fromTheme( iconName );

    if ( icon.isNull() )
	logError() << "No theme icon with name \"" << iconName << "\"" << endl;
    else
	item->setIcon( icon );

    return item;
}


PathSelectorItem * PathSelector::addMountPoint( const MountPoint * mountPoint,
                                                const QIcon &	   icon )
{
    PathSelectorItem * item = new PathSelectorItem( mountPoint, this );
    CHECK_NEW( item );

    if ( ! icon.isNull() )
	item->setIcon( icon );

    return item;
}


void PathSelector::addMountPoints( const QList<const MountPoint *> & mountPoints )
{
    foreach ( const MountPoint * mountPoint, mountPoints )
	addMountPoint( mountPoint );
}


void PathSelector::slotItemSelected( QListWidgetItem * origItem )
{
    PathSelectorItem * item = dynamic_cast<PathSelectorItem *>( origItem );

    if ( item )
    {
	// logVerbose() << "Selected path " << item->path() << endl;
	emit pathSelected( item->path() );
    }
}


void PathSelector::slotItemDoubleClicked( QListWidgetItem * origItem )
{
    PathSelectorItem * item = dynamic_cast<PathSelectorItem *>( origItem );

    if ( item )
    {
	// logVerbose() << "Double-clicked path " << item->path() << endl;
	emit pathDoubleClicked( item->path() );
    }
}




PathSelectorItem::PathSelectorItem( const QString & path,
				    PathSelector *  parent ):
    QListWidgetItem( path, parent ),
    _path( path ),
    _mountPoint( 0 )
{

}


PathSelectorItem::PathSelectorItem( const MountPoint * mountPoint,
				    PathSelector *     parent ):
    QListWidgetItem( parent ),
    _path( mountPoint->path() ),
    _mountPoint( mountPoint )
{
    QString text = _path + "\n";

    if ( _mountPoint->hasSizeInfo() )
	text += formatSize( _mountPoint->totalSize() ) + "  ";

    text += _mountPoint->filesystemType();
    setText( text );

    QString tooltip = _mountPoint->device();

#if SHOW_SIZES_IN_TOOLTIP

    if ( _mountPoint->hasSizeInfo() )
    {
        tooltip += "\n";
        tooltip += "\n" + QObject::tr( "Used: %1" )
            .arg( formatSize( _mountPoint->usedSize() ) );

        tooltip += "\n" + QObject::tr( "Free for users: %1" )
            .arg( formatSize( _mountPoint->freeSizeForUser() ) );

        tooltip += "\n" + QObject::tr( "Free for root: %1" )
            .arg( formatSize( _mountPoint->freeSizeForRoot() ) );
    }
#endif

    setToolTip( tooltip );
}


PathSelectorItem::~PathSelectorItem()
{
    // NOP
}
