/*
 *   File name: PkgManager.cpp
 *   Summary:	Dpkg package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DpkgPkgManager_h
#define DpkgPkgManager_h

#include "PkgManager.h"
#include "PkgInfo.h"


namespace QDirStat
{
    /**
     * Interface to 'dpkg' for all Debian-based Linux distros.
     *
     * Remember that 'apt' is based on 'dpkg' and 'dpkg' already does the
     * simple things needed here, so there is no need to create a specialized
     * version for 'apt' or any higher-level dpkg-based package managers.
     **/
    class DpkgPkgManager: public PkgManager
    {
    public:

	DpkgPkgManager() {}
	virtual ~DpkgPkgManager() {}

	/**
	 * Return the name of this package manager.
	 *
	 * Implemented from PkgManager.
	 **/
	virtual QString name() const { return "dpkg"; }

	/**
	 * Check if dpkg is active on the currently running system.
	 *
	 * Implemented from PkgManager.
	 **/
	virtual bool isPrimaryPkgManager() Q_DECL_OVERRIDE;

	/**
	 * Check if the dpkg command is available on the currently running
	 * system.
	 *
	 * Implemented from PkgManager.
	 **/
	virtual bool isAvailable() Q_DECL_OVERRIDE;

	/**
	 * Return the owning package of a file or directory with full path
	 * 'path' or an empty string if it is not owned by any package.
	 *
	 * Implemented from PkgManager.
	 *
	 * This basically executes this command:
	 *
	 *   /usr/bin/dpkg -S ${path}
	 **/
	virtual QString owningPkg( const QString & path ) Q_DECL_OVERRIDE;

        /**
         * Return the list of installed packages.
         *
         * Ownership of the list elements is transferred to the caller.
         *
	 * Implemented from PkgManager.
         **/
        virtual PkgInfoList installedPkg();

        /**
         * Return the list of files and directories owned by a package.
         *
	 * Implemented from PkgManager.
         **/
        virtual QStringList fileList( PkgInfo * pkg );

    protected:

        /**
         * Parse a package list as output by "dpkg-query --show --showformat".
         **/
        PkgInfoList parsePkgList( const QString & output );

        /**
         * Return a name suitable for a detailed queries for 'pkg'.
         *
         * This might include the architecture and the version if this is a
         * multi-arch or multi-version package.
         **/
        QString queryName( PkgInfo * pkg );


    };  // class DpkgPkgManager

}       // namespace QDirStat


#endif  // ifndef DpkgPkgManager
