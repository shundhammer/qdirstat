/*
 *   File name: PathSelector.h
 *   Summary:	Path selection list widget for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef PathSelector_h
#define PathSelector_h


#include <QListWidget>
#include <QFileIconProvider>


namespace QDirStat
{
    class MountPoint;
    class PathSelectorItem;

    /**
     * List widget for selecting a path, very much like the common "places"
     * list in file selection boxes. This widget also supports mount points
     * (see MountPoints.h) with more information than just the path.
     **/
    class PathSelector: public QListWidget
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 * Don't forget to populate the widget after creating it!
	 **/
	PathSelector( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~PathSelector();

	/**
	 * Add a path with the specified icon.
	 *
	 * Don't use this for mount points; use addMountPoint() instead which
	 * displays more information.
	 **/
	PathSelectorItem * addPath( const QString & path,
				    const QIcon &   icon = QIcon() );

	/**
	 * Convenience function: Add the current user's home directory.
	 **/
	PathSelectorItem * addHomeDir();

	/**
	 * Add a mount point with an appropriate icon.
	 *
	 * This displays not only the mount point's path, but also some
	 * additional information like the filesystem type and, if available,
	 * the size of the partition / filesystem.
	 **/
	PathSelectorItem * addMountPoint( MountPoint * mountPoint );

	/**
	 * Add a mount point with the specified icon.
	 * Notice that you can also specify QIcon() to suppress any icon.
	 **/
	PathSelectorItem * addMountPoint( MountPoint *	mountPoint,
					  const QIcon & icon );

	/**
	 * Add a list of mount points with the appropriate icons.
	 **/
	void addMountPoints( const QList<MountPoint *> & mountPoints );


    signals:

	/**
	 * Emitted when the user selects one of the paths.
	 *
	 * Do not use any of the inherited QListWidget signals; the returned
	 * QListWidgetItem might have a multi-line text from which you would
	 * have to extract the path.
	 **/
	void pathSelected( const QString & path );

	/**
	 * Emitted when the user double-clicks a path.
	 **/
	void pathDoubleClicked( const QString & path );


    public slots:

	/**
	 * Select the item representing the parent mount point of 'path'.
	 **/
	void selectParentMountPoint( const QString & path );


    protected slots:

	/**
	 * Signal forwarder to translate a selected item into a path.
	 **/
	void slotItemSelected( QListWidgetItem * item );

	/**
	 * Signal forwarder to translate a selected item into a path.
	 **/
	void slotItemDoubleClicked( QListWidgetItem * item );


    protected:

	QFileIconProvider _iconProvider;

    };	// class PathSelector




    /**
     * Item for a PathSelector widget.
     **/
    class PathSelectorItem: public QListWidgetItem
    {
    public:
	/**
	 * Constructor for a simple path.
	 * Use QListWidgetItem::setIcon() to set an icon.
	 **/
	PathSelectorItem( const QString & path,
			  PathSelector *  parent = 0 );

	/**
	 * Constructor for a mount point.
	 * Use QListWidgetItem::setIcon() to set an icon.
	 **/
	PathSelectorItem( MountPoint *	 mountPoint,
			  PathSelector * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~PathSelectorItem();

	/**
	 * Return the path for this item.
	 **/
	QString path() const { return _path; }

	/**
	 * Return the mount point or 0 if this is a plain path without a mount
	 * point.
	 **/
	MountPoint * mountPoint() { return _mountPoint; }


    protected:

	QString	     _path;
	MountPoint * _mountPoint;
    };

}	// namespace QDirStat

#endif	// PathSelector_h
