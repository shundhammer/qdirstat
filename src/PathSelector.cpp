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
#include "FormatUtil.h"
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

    connect( this, SIGNAL( itemClicked       ( QListWidgetItem * ) ),
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


PathSelectorItem * PathSelector::addHomeDir()
{
    QIcon icon = _iconProvider.icon( QFileIconProvider::Folder );
    PathSelectorItem * item = addPath( QDir::homePath(), icon );
    item->setToolTip( tr( "Your home directory" ) );

    return item;
}


PathSelectorItem * PathSelector::addMountPoint( MountPoint * mountPoint )
{
    CHECK_PTR( mountPoint );

    PathSelectorItem * item = new PathSelectorItem( mountPoint, this );
    CHECK_NEW( item );

    QIcon icon = _iconProvider.icon( mountPoint->isNetworkMount() ?
                                     QFileIconProvider::Network :
                                     QFileIconProvider::Drive      );
    item->setIcon( icon );

    return item;
}


PathSelectorItem * PathSelector::addMountPoint( MountPoint *  mountPoint,
                                                const QIcon & icon )
{
    CHECK_PTR( mountPoint );

    PathSelectorItem * item = new PathSelectorItem( mountPoint, this );
    CHECK_NEW( item );

    if ( ! icon.isNull() )
	item->setIcon( icon );

    return item;
}


void PathSelector::addMountPoints( const QList<MountPoint *> & mountPoints )
{
    foreach ( MountPoint * mountPoint, mountPoints )
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


void PathSelector::selectParentMountPoint( const QString & wantedPath )
{
    PathSelectorItem * bestMatch = 0;

    for ( int i=0; i < count(); ++i )
    {
        PathSelectorItem * current = dynamic_cast<PathSelectorItem *>( this->item( i ) );

        if ( current && wantedPath.startsWith( current->path() ) )
        {
            if ( ! bestMatch ||
                 current->path().length() > bestMatch->path().length() )
            {
                bestMatch = current;
            }
        }
    }

    if ( bestMatch )
    {
        // logDebug() << "Best match: " << bestMatch->path() << endl;;
        setCurrentItem( bestMatch );
    }
}





PathSelectorItem::PathSelectorItem( const QString & path,
				    PathSelector *  parent ):
    QListWidgetItem( path, parent ),
    _path( path ),
    _mountPoint( 0 )
{

}


PathSelectorItem::PathSelectorItem( MountPoint *   mountPoint,
				    PathSelector * parent ):
    QListWidgetItem( parent ),
    _path( mountPoint->path() ),
    _mountPoint( mountPoint )
{
    QString text = _path + "\n";

    if ( _mountPoint->hasSizeInfo() && _mountPoint->totalSize() > 0 )
    {
	text += formatSize( _mountPoint->totalSize() ) + "  ";
    }

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
