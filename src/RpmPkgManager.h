/*
 *   File name: RpmPkgManager.h
 *   Summary:	RPM package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef RpmPkgManager_h
#define RpmPkgManager_h

#include <QString>

#include "PkgManager.h"
#include "PkgInfo.h"


namespace QDirStat
{
    /**
     * Interface to 'rpm' for all RPM-based Linux distros such as SUSE,
     * openSUSE, Red Hat, Fedora.
     *
     * Remember that 'zypper' and 'yum' are based on 'rpm' and 'rpm' already
     * does the simple things needed here, so there is no need to create a
     * specialized version for 'zypper' or 'yum' or any higher-level rpm-based
     * package managers.
     **/
    class RpmPkgManager: public PkgManager
    {
    public:

	RpmPkgManager();
	virtual ~RpmPkgManager() {}

	/**
	 * Return the name of this package manager.
	 *
	 * Implemented from PkgManager.
	 **/
	virtual QString name() const { return "rpm"; }

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
	 * Read parameters from the settings file.
	 **/
	void readSettings();

	/**
	 * Parse a package list as output by "dpkg-query --show --showformat".
	 **/
	PkgInfoList parsePkgList( const QString & output );

	/**
	 * Show a warning that the RPM database should be rebuilt
	 * ("sudo rpm --rebuilddb").
	 **/
	void rebuildRpmDbWarning();


	// Data members

	QString _rpmCommand;
	int	_getPkgListWarningSec;

    }; // class RpmPkgManager

} // namespace QDirStat


#endif // RpmPkgManager_h
