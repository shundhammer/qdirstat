/*
 *   File name: FilesystemsWindow.h
 *   Summary:	QDirStat "Mounted Filesystems" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FilesystemsWindow_h
#define FilesystemsWindow_h

#include <QDialog>
#include <QTreeWidgetItem>

#include "ui_filesystems-window.h"
#include "FileInfo.h"	// FileSize



namespace QDirStat
{
    class MountPoint;

    /**
     * Modeless dialog to display details about mounted filesystems:
     *
     *	 - device
     *	 - mount point
     *	 - filesystem type
     *	 - used disk space
     *	 - free disk space for nonprivileged users
     *	 - free disk space for root
     *
     * The sizes may not be available on all platforms (no Qt 4 support!) or
     * for some filesystem types.
     **/
    class FilesystemsWindow: public QDialog
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 *
	 * Notice that this widget will destroy itself upon window close.
	 *
	 * It is advised to use a QPointer for storing a pointer to an instance
	 * of this class. The QPointer will keep track of this window
	 * auto-deleting itself when closed.
	 **/
	FilesystemsWindow( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~FilesystemsWindow();

	/**
	 * Read the path of the currently selected filesystem or an empty
	 * string if there is none.
	 **/
	QString selectedPath() const;


    signals:

	void readFilesystem( const QString & path );


    public slots:

	/**
	 * Populate the window with all normal filesystems. Bind mounts,
	 * filesystems mounted several times and Btrfs subvolumes are excluded.
	 **/
	void populate();

	/**
	 * Refresh (reload) all data.
	 **/
	void refresh();

	/**
	 * Reject the dialog contents, i.e. the user clicked the "Cancel" or
	 * WM_CLOSE button. This not only closes the dialog, it also deletes
	 * it.
	 *
	 * Reimplemented from QDialog.
	 **/
	virtual void reject() Q_DECL_OVERRIDE;


    protected slots:

	/**
	 * Enable or disable widgets such as the "Read" button.
	 **/
	void enableActions();

	/**
	 * Notification that the "Read" button was clicked:
	 * Emit the readFilesystem() signal.
	 **/
	void readSelectedFilesystem();


    protected:

	/**
	 * Clear all data and widget contents.
	 **/
	void clear();

	/**
	 * One-time initialization of the widgets in this window.
	 **/
	void initWidgets();

	/**
	 * Show panel message warning about Btrfs and how it reports free sizes
	 **/
	void showBtrfsFreeSizeWarning();


	//
	// Data members
	//

	Ui::FilesystemsWindow * _ui;

    };	// class FilesystemsWindow


    /**
     * Column numbers for the filesystems tree widget
     **/
    enum FilesystemColumns
    {
	FS_DeviceCol = 0,
	FS_MountPathCol,
	FS_TypeCol,
	FS_TotalSizeCol,
	FS_UsedSizeCol,
	FS_ReservedSizeCol,
	FS_FreeSizeCol,
	FS_FreePercentCol
    };


    /**
     * Item class for the filesystems list (which is really a tree widget).
     **/
    class FilesystemItem: public QTreeWidgetItem
    {
    public:
	/**
	 * Constructor.
	 **/
	FilesystemItem( MountPoint * mountPoint, QTreeWidget * parent );

	// Getters

	QString	 device()	  const { return _device;	  }
	QString	 mountPath()	  const { return _mountPath;	  }
	QString	 fsType()	  const { return _fsType;	  }
	FileSize totalSize()	  const { return _totalSize;	  }
	FileSize usedSize()	  const { return _usedSize;	  }
	FileSize reservedSize()	  const { return _reservedSize;	  }
	FileSize freeSize()	  const { return _freeSize;	  }
	bool	 isNetworkMount() const { return _isNetworkMount; }
	bool	 isReadOnly()	  const { return _isReadOnly;	  }

	/**
	 * Less-than operator for sorting.
	 **/
	bool operator<( const QTreeWidgetItem & rawOther ) const;


    protected:

	QString	 _device;
	QString	 _mountPath;
	QString	 _fsType;
	FileSize _totalSize;
	FileSize _usedSize;
	FileSize _reservedSize;
	FileSize _freeSize;
	bool	 _isNetworkMount;
	bool	 _isReadOnly;
    };

}

#endif	// FilesystemsWindow_h
