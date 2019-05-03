/*
 *   File name: PacManPkgManager.h
 *   Summary:	PacMan package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef PacManPkgManager_h
#define PacManPkgManager_h

#include <QString>

#include "PkgManager.h"


namespace QDirStat
{
    /**
     * Interface to 'pacman' for Manjaro / Arch Linux.
     **/
    class PacManPkgManager: public PkgManager
    {
    public:

	PacManPkgManager() {}
	virtual ~PacManPkgManager() {}

	/**
	 * Return the name of this package manager.
	 *
	 * Implemented from PkgManager.
	 **/
	virtual QString name() const { return "pacman"; }

	/**
	 * Check if RPM is active on the currently running system.
	 *
	 * Implemented from PkgManager.
	 **/
	virtual bool isPrimaryPkgManager() Q_DECL_OVERRIDE;

	/**
	 * Check if the rpm command is available on the currently running
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
	 *   /usr/bin/rpm -qf ${path}
	 **/
	virtual QString owningPkg( const QString & path ) Q_DECL_OVERRIDE;


        //-----------------------------------------------------------------
        //                     Optional Features
        //-----------------------------------------------------------------

        /**
         * Return 'true' if this package manager supports getting the list of
         * installed packages.
         *
	 * Reimplemented from PkgManager.
         **/
        virtual bool supportsGetInstalledPkg() Q_DECL_OVERRIDE
            { return true; }

        /**
         * Return the list of installed packages.
         *
         * Ownership of the list elements is transferred to the caller.
         *
	 * Reimplemented from PkgManager.
         **/
        virtual PkgInfoList installedPkg();

        /**
         * Return 'true' if this package manager supports getting the file list
         * for a package.
         *
	 * Reimplemented from PkgManager.
         **/
        virtual bool supportsFileList() Q_DECL_OVERRIDE
            { return true; }

        /**
         * Return the command for getting the list of files and directories
         * owned by a package.
         *
	 * Reimplemented from PkgManager.
         **/
        virtual QString fileListCommand( PkgInfo * pkg ) Q_DECL_OVERRIDE;

        /**
         * Parse the output of the file list command.
         *
	 * Reimplemented from PkgManager.
         **/
        virtual QStringList parseFileList( const QString & output ) Q_DECL_OVERRIDE;


    protected:

        /**
         * Parse a package list as output by "dpkg-query --show --showformat".
         **/
        PkgInfoList parsePkgList( const QString & output );

    }; // class PacManPkgManager

} // namespace QDirStat


#endif // PacManPkgManager_h
