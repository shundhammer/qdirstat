/*
 *   File name: DPkgPkgManager.cpp
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


	//-----------------------------------------------------------------
	//		       Optional Features
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
	virtual PkgInfoList installedPkg() Q_DECL_OVERRIDE;

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

	/**
	 * Return 'true' if this package manager supports building a file list
	 * cache for getting all file lists for all packages.
	 *
	 * Reimplemented from PkgManager.
	 **/
	virtual bool supportsFileListCache() Q_DECL_OVERRIDE
	    { return true; }

	/**
	 * Create a file list cache with the specified lookup type for all
	 * installed packages. This is an expensive operation.
	 *
	 * This is a best-effort approach; the cache might still not contain
	 * all desired packages. Check with PkgFileListCache::contains() and
	 * use PkgManager::fileList() as a fallback.
	 *
	 * Ownership of the cache is transferred to the caller; make sure to
	 * delete it when you are done with it.
	 *
	 * Reimplemented from PkgManager.
	 **/
	virtual PkgFileListCache * createFileListCache( PkgFileListCache::LookupType lookupType = PkgFileListCache::LookupByPkg ) Q_DECL_OVERRIDE;

	/**
	 * Return a name suitable for a detailed queries for 'pkg'.
	 *
	 * This might include the architecture and the version if this is a
	 * multi-arch or multi-version package.
	 *
	 * Reimplemented from PkgManager.
	 **/
	virtual QString queryName( PkgInfo * pkg ) Q_DECL_OVERRIDE;


    protected:

	/**
	 * Parse a package list as output by "dpkg-query --show --showformat".
	 **/
	PkgInfoList parsePkgList( const QString & output );

    };	// class DpkgPkgManager

}	// namespace QDirStat


#endif	// ifndef DpkgPkgManager
