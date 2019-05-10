/*
 *   File name: PkgQuery.h
 *   Summary:	Package manager query support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef PkgQuery_h
#define PkgQuery_h

#include <QString>
#include <QCache>

#include "PkgInfo.h"


namespace QDirStat
{
    class PkgManager;


    /**
     * Singleton class for simple queries to the system's package manager.
     **/
    class PkgQuery
    {
    public:

	/**
	 * Return the owning package of a file or directory with full path
	 * 'path' or an empty string if it is not owned by any package.
	 **/
	static QString owningPkg( const QString & path );

	/**
	 * Return the singleton instance of this class.
	 **/
	static PkgQuery * instance();

        /**
         * Return 'true' if any of the supported package managers was found.
         **/
        static bool foundSupportedPkgManager();

        /**
         * Return the (first) primary package manager if there is one or 0 if
         * not.
         **/
        static PkgManager * primaryPkgManager();

        /**
         * Return 'true' if any of the package managers has support for getting
         * the list of installed packages.
         **/
        static bool haveGetInstalledPkgSupport();

        /**
         * Return the list of installed packages.
         *
         * Ownership of the list elements is transferred to the caller.
         **/
        static PkgInfoList installedPkg();

        /**
         * Return 'true' if any of the package managers has support for getting
         * the the file list for a package.
         **/
        static bool haveFileListSupport();

        /**
         * Return the list of files and directories owned by a package.
         **/
        static QStringList fileList( PkgInfo * pkg );

	/**
	 * Return the owning package of a file or directory with full path
	 * 'path' or an empty string if it is not owned by any package.
	 **/
	QString getOwningPackage( const QString & path );

        /**
         * Return the list of installed packages.
         *
         * Ownership of the list elements is transferred to the caller.
         **/
        PkgInfoList getInstalledPkg();

        /**
         * Return the list of files and directories owned by a package.
         **/
        QStringList getFileList( PkgInfo * pkg );

        /**
         * Return 'true' if any of the package managers has support for getting
         * the list of installed packages.
         **/
        bool checkGetInstalledPkgSupport();

        /**
         * Return 'true' if any of the package managers has support for getting
         * the the file list for a package.
         **/
        bool checkFileListSupport();


    protected:

	/**
	 * Constructor. For internal use only; use the static methods instead.
	 **/
	PkgQuery();

	/**
	 * Destructor.
	 **/
	virtual ~PkgQuery();

        /**
         * Check which supported package managers are available and add them to
         * the internal list.
         **/
        void checkPkgManagers();

	/**
	 * Check if a package manager is available; add it to one of the
	 * internal lists if it is, or delete it if not.
	 **/
	void checkPkgManager( PkgManager * pkgManager );


	// Data members

	static PkgQuery *	 _instance;
	QList <PkgManager *>	 _pkgManagers;
	QList <PkgManager *>	 _secondaryPkgManagers;
	QCache<QString, QString> _cache;

    }; // class PkgQuery

} // namespace QDirStat


#endif // PkgQuery_h
