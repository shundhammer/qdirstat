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
#include <QMap>
#include <QTextStream>


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
         * Return the device that is mounted, someting like "/dev/sda3",
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
         * Return 'true' if the filesystem type of this mount point is "btrfs".
         **/
        bool isBtrfs() const;

        /**
         * Return 'true' if this is a system mount, i.e. one of the known
         * system mount points like /dev, /proc, /sys, or if the device name
         * does not start with a slash (e.g. cgroup, tmpfs, sysfs, ...)
         **/
        bool isSystemMount() const;

        /**
         * Return 'true' if this is a duplicate mount, i.e. either a bind mount
         * or a filesystem that was mounted multiple times.
         **/
        bool isDuplicate() const { return _isDuplicate; }

        /**
         * Set the 'duplicate' flag. This should only be set while /proc/mounts
         * or /etc/mtab is being read.
         **/
        void setDuplicate( bool dup = true ) { _isDuplicate = dup; }


    protected:
        QString     _device;
        QString     _path;
        QString     _filesystemType;
        QStringList _mountOptions;
        bool        _isDuplicate;

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
        static const MountPoint * findByPath( const QString & path );

        /**
         * Find the nearest mount point upwards in the directory hierarchy
         * starting from 'path'. 'path' itself might be that mount point.
         * Ownership of the returned object is not transferred to the caller.
         *
         * This might return 0 if none of the files containing mount
         * information (/proc/mounts, /etc/mtab) could be read.
         **/
        static const MountPoint * findNearestMountPoint( const QString & path );

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
         * Dump all current mount points to the log. This does not call
         * ensurePopulated() first.
         **/
        static void dump();


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

        /**
         * Check if any of the mount points has filesystem type "btrfs".
         **/
        bool checkForBtrfs();

        /**
         * Return 'true' if 'device' is mounted.
         **/
        bool isDeviceMounted( const QString & device );

        //
        // Data members
        //

        static MountPoints * _instance;

        QMap<QString, MountPoint *> _mountPointMap;
        bool                        _isPopulated;
        bool                        _hasBtrfs;
        bool                        _checkedForBtrfs;

    }; // class MountPoints


    inline QTextStream & operator<< ( QTextStream & stream, const MountPoint * mp )
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
