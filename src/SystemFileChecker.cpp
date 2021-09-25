/*
 *   File name: SystemFileChecker.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "SystemFileChecker.h"
#include "DirInfo.h"


#define MIN_NON_SYSTEM_UID      500

using namespace QDirStat;


bool SystemFileChecker::isSystemFile( FileInfo * file )
{
    if ( ! file )
        return false;

    if ( file->isPseudoDir() && file->parent() )
        file = file->parent();

    QString path = file->url();

    if ( file->isDir() )
        path += "/";

    if ( isSystemPath( path ) )
        return true;

    if ( file->hasUid() && isSystemUid( file->uid() ) &&
         mightBeSystemPath( path ) )
        return true;

    return false;
}


bool SystemFileChecker::isSystemUid( uid_t uid )
{
    return uid < (uid_t) MIN_NON_SYSTEM_UID;
}


bool SystemFileChecker::isSystemPath( const QString & path )
{
    if ( path.startsWith( "/boot/"  ) ||
         path.startsWith( "/bin/"   ) ||
         path.startsWith( "/dev/"   ) ||
         path.startsWith( "/etc/"   ) ||
         path.startsWith( "/lib/"   ) ||
         path.startsWith( "/lib32/" ) ||
         path.startsWith( "/lib64/" ) ||
         path.startsWith( "/opt/"   ) ||
         path.startsWith( "/proc/"  ) ||
         path.startsWith( "/sbin/"  ) ||
         path.startsWith( "/sys/"   )   )
    {
        return true;
    }

    if ( path.startsWith( "/usr/" ) &&
         ! path.startsWith( "/usr/local/" ) )
    {
        return true;
    }

    /**
     * Intentionally NOT considered true system paths:
     *
     *   /cdrom
     *   /home
     *   /lost+found
     *   /media
     *   /mnt
     *   /root
     *   /run
     *   /srv
     *   /tmp
     *   /var
     *
     * Some of those might be debatable: While it is true that no mere user
     * should mess with anything outside his home directory, some might work on
     * web projects below /srv, some might write or use software that does
     * things below /run, some might be in the process of cleaning up a mess
     * left behind by fsck below /lost+found, some may wish to clean up
     * accumulated logs and spool files and whatnot below /var.
     *
     * Of course many users might legitimately use classic removable media
     * mount points like /cdrom, /media, /mnt, and all users are free to use
     * /tmp and /var/tmp.
     **/

    return false;
}


bool SystemFileChecker::mightBeSystemPath( const QString & path )
{
    if ( path.contains  ( "/lost+found/" ) ||   // Also on other mounted filesystems!
         path.startsWith( "/run/"        ) ||
         path.startsWith( "/srv/"        ) ||
         path.startsWith( "/var/"        )   )
    {
        return true;
    }
    else
    {
        return false;
    }
}
