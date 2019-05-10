/*
 *   File name: PkgFileListCache.h
 *   Summary:	Package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef PkgFileListCache_h
#define PkgFileListCache_h

#include <QString>
#include <QMultiMap>


namespace QDirStat
{
    // Forward declarations

    class PkgManager;


    /**
     * Cache class for a package file lists.
     *
     * This is useful when file lists for many packages need to be fetched;
     * some package managers (albeit not all) have a command to return all file
     * lists for all packages at once with one single command which is
     * typically much faster than invoking a separate external command for each
     * installed package.
     *
     * Use PkgManager::createFileListCache() to create and fill such a cache.
     **/
    class PkgFileListCache
    {
    public:
        /**
         * Constructor.
         **/
        PkgFileListCache( PkgManager * pkgManager );

        /**
         * Destructor.
         **/
        virtual ~PkgFileListCache();

        /**
         * Return the sorted file list for a package.
         **/
        QStringList fileList( const QString & pkgName );

        /**
         * Return 'true' if the cache contains any information about a package,
         * 'false' if not.
         **/
        bool contains( const QString & pkgName ) const;

        /**
         * Return the number of key/value pairs in the cache.
         **/
        bool size() const { return _fileNames.size(); }

        /**
         * Return 'true' if the cache is empty, 'false' if not.
         **/
        bool isEmpty() const { return _fileNames.isEmpty(); }

        /**
         * Clear the cache.
         **/
        void clear();

        /**
         * Add one file for one package.
         **/
        void add( const QString & pkgName, const QString & fileName );

        /**
         * Return the package manager parent of this cache.
         **/
        PkgManager * pkgManager() const { return _pkgManager; }


    protected:

        PkgManager *                _pkgManager;
        QMultiMap<QString, QString> _fileNames;
    };
}       // namespace QDirStat

#endif  // PkgFileListCache_h
