/*
 *   File name: MountPoints.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef MountPoints_h
#define MountPoints_h


#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QTextStream>

#if (QT_VERSION < QT_VERSION_CHECK( 5, 4, 0 ))
#  define HAVE_Q_STORAGE_INFO 0
typedef void * QStorageInfo;
#else
#  define HAVE_Q_STORAGE_INFO 1
#  include <QStorageInfo>
#endif

#include "FileSize.h"


namespace QDirStat
{
    /**
     * Helper class to represent one mount point of a Linux/Unix filesystem.
     **/
    class MountPoint
    {
    public:
	/**
	 * Constructor.
	 **/
	MountPoint( const QString & device,
		    const QString & path,
		    const QString & filesystemType,
		    const QString & mountOptions );

        /**
         * Destructor.
         **/
        virtual ~MountPoint();

	/**
	 * Return the device that is mounted, something like "/dev/sda3",
	 * "/dev/mapper/crypto", "nas:/share/work".
	 **/
	QString device() const { return _device; }

	/**
	 * Return the path where the device is mounted to.
	 **/
	QString path() const { return _path; }

	/**
	 * Return the filesystem type as string ("ext4", "btrfs", "none".
	 **/
	QString filesystemType() const { return _filesystemType; }

	/**
	 * Return the individual mount options as a list of strings
	 * ["rw", "nosuid", "nodev", "relatime", "rsize=32768"].
	 **/
	QStringList mountOptions() const { return _mountOptions; }

	/**
	 * Return the mount options as one comma-separated string.
	 **/
	QString mountOptionsStr() const;

	/**
	 * Return 'true' if the filesystem is mounted read-only.
	 **/
	bool isReadOnly() const;

	/**
	 * Return 'true' if the filesystem type of this mount point is "btrfs".
	 **/
	bool isBtrfs() const;

	/**
	 * Return 'true' if the filesystem type of this mount point starts with
	 * "ntfs".
	 **/
        bool isNtfs() const;

	/**
	 * Return 'true' if this is a network filesystem like NFS or Samba
	 * (cifs).
	 **/
	bool isNetworkMount() const;

	/**
	 * Return 'true' if this is a system mount, i.e. one of the known
	 * system mount points like /dev, /proc, /sys, or if the device name
	 * does not start with a slash (e.g. cgroup, tmpfs, sysfs, ...)
	 **/
	bool isSystemMount() const;

        /**
         * Return 'true' if this is an autofs, i.e. a filesystem managed by the
         * automounter.
         **/
        bool isAutofs() const;

        /**
         * Return 'true' if this is an autofs that is not currently mounted.
         **/
        bool isUnmountedAutofs();

	/**
	 * Return 'true' if this is a duplicate mount, i.e. either a bind mount
	 * or a filesystem that was mounted multiple times.
	 **/
	bool isDuplicate() const { return _isDuplicate; }

        /**
         * Return 'true' if this is a snap package, i.e. it is a squashfs
         * mounted below /snap.
         **/
        bool isSnapPackage() const;

	/**
	 * Set the 'duplicate' flag. This should only be set while /proc/mounts
	 * or /etc/mtab is being read.
	 **/
	void setDuplicate( bool dup = true ) { _isDuplicate = dup; }

	/**
	 * Return 'true' if size information for this mount point is available.
	 * This may depend on the build OS and the Qt version.
	 **/
	bool hasSizeInfo() const;

	/**
	 * Total size of the filesystem of this mount point.
	 * This returns -1 if no size information is available.
	 **/
	FileSize totalSize();

	/**
	 * Total used size of the filesystem of this mount point.
	 * This returns -1 if no size information is available.
	 **/
	FileSize usedSize();

	/**
	 * Reserved size for root for the filesystem of this mount point.
	 * This returns -1 if no size information is available.
	 **/
	FileSize reservedSize();

	/**
	 * Available free size of this filesystem for non-privileged users.
	 * This returns -1 if no size information is available.
	 **/
	FileSize freeSizeForUser();

	/**
	 * Available free size of this filesystem for privileged users.
	 * This returns -1 if no size information is available.
	 **/
	FileSize freeSizeForRoot();


    protected:

#if HAVE_Q_STORAGE_INFO
        /**
         * Lazy access to the QStorageInfo for this mount.
         **/
        QStorageInfo * storageInfo();
#endif

	QString	    _device;
	QString	    _path;
	QString	    _filesystemType;
	QStringList _mountOptions;
	bool	    _isDuplicate;

#if HAVE_Q_STORAGE_INFO
	QStorageInfo * _storageInfo;
#endif
    }; // class MountPoint


    /**
     * Singleton class to access the current mount points.
     **/
    class MountPoints
    {
    public:
	/**
	 * Return the singleton object for this class. The first use will
	 * create the singleton. Notice that most of the static methods access
	 * the singleton, too, so the first call to most of those static
	 * methods will already create the singleton.
	 **/
	static MountPoints * instance();

	/**
	 * Clear the content of the singleton. This is useful whenever the
	 * mount points in the system might have changed, i.e. when a
	 * filesystem might have been mounted or unmounted.
	 *
	 * This does not create the singleton if it doesn't exist yet.
	 **/
	static void clear();

	/**
	 * Return 'true' if there are no mount points at all.
	 **/
	static bool isEmpty();

	/**
	 * Return the mount point for 'path' if there is one or 0 if there is
	 * not. Ownership of the returned object is not transferred to the
	 * caller, i.e. the caller should not delete it. The pointer remains
	 * valid until the next call to clear().
	 **/
	static MountPoint * findByPath( const QString & path );

	/**
	 * Find the nearest mount point upwards in the directory hierarchy
	 * starting from 'path'. 'path' itself might be that mount point.
	 * Ownership of the returned object is not transferred to the caller.
	 *
	 * This might return 0 if none of the files containing mount
	 * information (/proc/mounts, /etc/mtab) could be read.
	 **/
	static MountPoint * findNearestMountPoint( const QString & path );

	/**
	 * Return 'true' if any mount point has filesystem type "btrfs".
	 **/
	static bool hasBtrfs();

	/**
	 * Ensure the mount points are populated with the content of
	 * /proc/mounts, falling back to /etc/mtab if /proc/mounts cannot be
	 * read.
	 **/
	void ensurePopulated();

	/**
	 * Return a list of "normal" mount points, i.e. those that are not
	 * system mounts, bind mounts or duplicate mounts.
	 *
	 * The result is sorted by the order in which the filesystems were
	 * mounted (the same as in /proc/mounts or in /etc/mtab).
	 **/
	static QList<MountPoint *> normalMountPoints();

	/**
	 * Dump all current mount points to the log. This does not call
	 * ensurePopulated() first.
	 **/
	static void dump();

	/**
	 * Dump all normal mount points to the log, i.e. those that are not
	 * system, bind or duplicate mount points.
	 **/
	static void dumpNormalMountPoints();

	/**
	 * Return 'true' if size information for mount points is available.
	 * This may depend on the build OS and the Qt version.
	 **/
	static bool hasSizeInfo();

        /**
         * Clear all information and reload it from disk.
         * NOTICE: This invalidates ALL MountPoint pointers!
         **/
        static void reload();


    protected:

	/**
	 * Constructor. Not for public use. Use instance() or the static
	 * methods instead.
	 **/
	MountPoints();

	/**
	 * Destructor.
	 **/
	virtual ~MountPoints();

	/**
	 * Clear the content of this class.
	 **/
	void init();

	/**
	 * Read 'filename' (in /proc/mounts or /etc/mnt syntax) and populate
	 * the mount points with the content. Return 'true' on success, 'false'
	 * on failure.
	 **/
	bool read( const QString & filename );

#if HAVE_Q_STORAGE_INFO

        /**
         * Fallback method if neither /proc/mounts nor /etc/mtab is available:
         * Try using QStorageInfo. Return 'true' if any mount point was found.
         **/
        bool readStorageInfo();
#endif

        /**
         * Post-process a mount point and add it to the internal list and map.
         **/
        void postProcess( MountPoint * mountPoint );

        /**
         * Add a mount point to the internal list and map.
         **/
        void add( MountPoint * mountPoint );


	/**
	 * Check if any of the mount points has filesystem type "btrfs".
	 **/
	bool checkForBtrfs();

        /**
         * Try to check with the external "lsblk" command (if available) what
         * block devices use NTFS and populate _ntfsDevices with them.
         **/
        void findNtfsDevices();

	/**
	 * Return 'true' if 'device' is mounted.
	 **/
	bool isDeviceMounted( const QString & device );

	//
	// Data members
	//

	static MountPoints * _instance;

	QList<MountPoint *>	    _mountPointList;
	QMap<QString, MountPoint *> _mountPointMap;
        QStringList                 _ntfsDevices;
	bool			    _isPopulated;
	bool			    _hasBtrfs;
	bool			    _checkedForBtrfs;

    }; // class MountPoints


    inline QTextStream & operator<< ( QTextStream & stream, MountPoint * mp )
    {
	if ( mp )
	{
	    stream << "<mount point for " << mp->device()
		   << " at " << mp->path()
		   << " type " << mp->filesystemType()
		   << ">";
	}
	else
	    stream << "<NULL MountPoint*>";

	return stream;
    }

} // namespace QDirStat


#endif // MountPoints_h
