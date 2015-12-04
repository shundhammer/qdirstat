/*
 *   File name:	DirSaver.h
 *   Summary:	Utility object to save current working directory
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirSaver_h
#define DirSaver_h


#include <kurl.h>
#include <qdir.h>


/**
 * Helper class to change directories without losing the current context.
 * Will change back to the old working directory when destroyed.
 *
 * @short Directory changer with automatic restore
 **/
class DirSaver
{
public:
    /**
     * Constructor. Will save the current working directory and change to the
     * path supplied. The old working directory will be restored when this
     * object is destroyed.
     **/
    DirSaver( const QString & newPath = "" );

    /**
     * Constructor from a KURL. Will issue error messages on stdout for
     * non-local objects.
     **/
    DirSaver( const KURL & url );
    
    /**
     * Destructor. Restores the original working directory.
     **/
    virtual ~DirSaver();

    /**
     * Change directory. Unlike @ref QDir::cd(), this method really performs a
     * system chdir() so subsequent system calls will have the directory
     * specified as the new current working directory.
     **/
    void cd( const QString & newPath );

    /**
     * Obtain the current working directory's absolute path.
     * This is useful for resolving/simplifying relative paths.
     **/
    QString currentDirPath() const;
    
    /**
     * (Prematurely) restore the working directory. Unnecessary when this
     * object will be destroyed anyway since the destructor does exactly that.
     **/
    void restore();

protected:
    QDir oldWorkingDir;
};

#endif // DirSaver_h


// EOF
