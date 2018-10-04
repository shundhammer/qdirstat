/*
 *   File name: SystemFileChecker.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef SystemFileChecker_h
#define SystemFileChecker_h


#include <sys/types.h>
#include <QString>


namespace QDirStat
{
    class FileInfo;

    /**
     * Check functions to find out if a file is a system file.
     *
     * This might be a bit Linux-centric. It will work on other Unix-type
     * system, but it might not be too reliable on other systems like MacOS X.
     **/
    class SystemFileChecker
    {
    public:
        /**
         * Return 'true' if a file is clearly a system file.
         **/
        static bool isSystemFile( FileInfo * file );

        /**
         * Return 'true' if a user ID is a system user.
         **/
        static bool isSystemUid( uid_t uid );

        /**
         * Return 'true' if a path is a known system path.
         **/
        static bool isSystemPath( const QString & path );

        /**
         * Return 'true' if a path might be a known system path.
         *
         * Check with isSystemPath() first and use this only if isSystemPath()
         * returns false.
         **/
        static bool mightBeSystemPath( const QString & path );

    }; // class SystemFileChecker
}

#endif // SystemFileChecker_h
