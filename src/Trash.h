/*
 *   File name: Trash.h
 *   Summary:	Implementation of the XDG Trash spec for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef Trash_h
#define Trash_h

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <QObject>
#include <QMap>

class TrashDir;
typedef QMap<dev_t, TrashDir *> TrashDirMap;


/**
 * This class implements the XDG Trash specification:
 *
 *     http://standards.freedesktop.org/trash-spec/trashspec-1.0.html
 *
 * Basically, this is a desktop trashcan that works just like the trashcan in
 * KDE, Gnome, Xfce and other major Linux desktops. It should integrate well
 * with any of them, i.e., files or directories moved to this trash can should
 * appear in the desktop's native trashcan implementation (the window you get
 * when you click on the trashcan icon on the desktop or in the file manager).
 *
 * Most of the important functions of this class are static. They use the
 * singleton of this class internally that can be accessed with instance().
 **/
class Trash
{
public:

    /**
     * Throw a file or directory into the trash.
     * Return 'true' on success, 'false' on error.
     **/
    static bool trash( const QString & path );

    /**
     * Restore a file or directory from the trash to its original location.
     * Return 'true' on success, 'false' on error.
     **/
    static bool restore( const QString & path );

    /**
     * Empty the trash can, i.e. delete all its contents permanently.
     *
     * This does not just affect files or directories moved to the trash with
     * this class, but everything in all known trash directories, i.e. all
     * trash directories that were used during the life time of the singleton
     * of this class.
     **/
    static void empty();

    /**
     * Return the singleton object for this class. The first use will create
     * the singleton. Notice that the static methods all access the singleton,
     * too, so the first call to any of those static methods will already
     * create the singleton.
     **/
    static Trash * instance();

    /**
     * Return the device of file or directory 'path'.
     **/
    static dev_t device( const QString & path );


protected:
    /**
     * Constructor. Not for public use. Use instance() or the static methods
     * instead.
     **/
    Trash();

    /**
     * Destructor.
     **/
    virtual ~Trash();

    /**
     * Find the toplevel directory (the mount point) for the device that 'path'
     * is on.
     **/
    static QString toplevel( const QString & path );

    /**
     * Return the trash dir for 'path'.
     **/
    TrashDir * trashDir( const QString & path );


    //
    // Data members
    //

    static Trash       * _instance;

    dev_t		_homeDevice;
    TrashDir	      * _homeTrashDir;
    TrashDirMap		_trashDirs;

};	// class Trash


/**
 * One trash directory. There might be several on a system:
 *
 * - One in the user's home directory in $XDG_DATA_HOME/Trash
 *   or ~/.local/share/Trash if $XDG_DATA_HOME is not set or empty
 *
 * - One in the toplevel directory (the mount point) of each filesystem:
 *   $TOPLEVEL/.Trash/$UID
 *
 * - If $TOPLEVEL/.Trash does not exist or does not pass some checks, one in
 *   $TOPLEVEL/.Trash-$UID
 **/
class TrashDir
{
public:
    /**
     * Constructor. This will create the trash directory and its required
     * subdirectories if it doesn't exist yet.
     *
     * This might throw a FileException if the corresponding disk directories
     * could not be created.
     **/
    TrashDir( const QString & _path, dev_t device );

    /**
     * Return the full path for this trash directory.
     **/
    QString path() const { return _path; }

    /**
     * Return the device (as returned from stat()) for this trash directory.
     **/
    dev_t device() const { return _device; }

    /**
     * Return the path of the "files" subdirectory of this trash dir.
     **/
    QString filesPath() const { return _path + "/files"; }

    /**
     * Return the path of the "info" subdirectory of this trash dir.
     **/
    QString infoPath() const { return _path + "/info"; }

    /**
     * Create a name that is unique within this trash directory.
     * If no file or directory with 'name' exists yet in Trash/files or
     * Trash/info, append a number.
     **/
    QString uniqueName( const QString & name );

    /**
     * Create a .trashinfo file for a file or directory 'path' that will be
     * named 'targetName' (the unique name) in the trash dir.
     *
     * This might throw a FileException.
     **/
    void createTrashInfo( const QString & path, const QString & targetName );

    /**
     * Move a file or directory 'path' to to targetName in the trash dir's
     * /files subdirectory. If both are on different devices, copy the file and
     * then delete the original.
     *
     * This might throw a FileException.
     **/
    void move( const QString & path, const QString & targetName );


protected:

    /**
     * Create a directory if it doesn't exist. This throws an exception if
     * 'doThrow' is 'true'.
     *
     * Return 'true' if success, 'false' if error (and doThrow is 'false').
     **/
    static bool ensureDirExists( const QString & dir,
				 mode_t		 mode,
				 bool		 doThrow = false );

    //
    // Data members
    //

    QString _path;
    dev_t   _device;
};


#endif	// Trash_h
