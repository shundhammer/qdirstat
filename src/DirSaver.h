/*
 *   File name:	DirSaver.h
 *   Summary:	Utility object to save current working directory
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DirSaver_h
#define DirSaver_h

#include <QDir>


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
     * Destructor. Restores the original working directory.
     **/
    virtual ~DirSaver();

    /**
     * Change directory. Unlike QDir::cd(), this method really performs a
     * system chdir() so subsequent system calls will have the directory
     * specified as the new current working directory.
     **/
    void cd( const QString & newPath );

    /**
     * (Prematurely) restore the working directory. Unnecessary when this
     * object will be destroyed anyway since the destructor does exactly that.
     **/
    void restore();

protected:
    QString _oldWorkingDir;
};

#endif // DirSaver_h
