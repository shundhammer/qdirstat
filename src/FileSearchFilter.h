/*
 *   File name: FileSearchFilter.h
 *   Summary:	Package manager Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef FileSearchFilter_h
#define FileSearchFilter_h

#include <QString>
#include <QRegExp>

#include "Logger.h"
#include "SearchFilter.h"


namespace QDirStat
{
    class DirInfo;

    /**
     * Filter class for searching for files and/or directories.
     **/
    class FileSearchFilter: public SearchFilter
    {
    public:

        /**
         * Constructor: Create a search filter with the specified pattern and
         * filter mode.
         *
         * 'subtree' is the directory node to start searching from.
         *
         * Filter mode "Auto" tries to guess a useful mode from the pattern:
         *
         * - If it's a fixed string without any wildcards, it uses
         *   "StartsWith".
         * - If it contains "*" wildcard characters, it uses "Wildcard".
         * - If it contains ".*" or "^" or "$", it uses "RegExp".
         * - If it starts with "=", it uses "ExactMatch".
         * - If it's empty, it uses "SelectAll".
         **/
        FileSearchFilter( DirInfo *       subtree,
                          const QString & pattern,
                          FilterMode      filterMode = Auto );

        /**
         * Flags which node types to find
         **/
        bool findFiles()    const { return _findFiles;    }
        bool findDirs()     const { return _findDirs;     }
        bool findSymLinks() const { return _findSymLinks; }
        bool findPkg()      const { return _findPkg;      }

        void setFindFiles   ( bool value ) { _findFiles    = value; }
        void setFindDirs    ( bool value ) { _findDirs     = value; }
        void setFindSymLinks( bool value ) { _findSymLinks = value; }
        void setFindPkg     ( bool value ) { _findPkg      = value; }

        /**
         * Subtree to start the search from
         **/
        DirInfo * subtree() const { return _subtree; }
        void setSubtree( DirInfo * subtree ) { _subtree = subtree; }


    protected:

        DirInfo * _subtree;
        bool      _findFiles;
        bool      _findDirs;
        bool      _findSymLinks;
        bool      _findPkg;

    };  // class FileSearchFilter


    LogStream & operator<< ( LogStream              & stream,
                             const FileSearchFilter & filter );
}

#endif  // FileSearchFilter_h
