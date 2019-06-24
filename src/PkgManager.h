/*
 *   File name: PkgManager.h
 *   Summary:	Package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef PkgManager_h
#define PkgManager_h

#include <QString>

#include "PkgInfo.h"
#include "PkgFileListCache.h"


#define LOG_COMMANDS	true
#define LOG_OUTPUT	false
#include "SysUtil.h"


namespace QDirStat
{
    using SysUtil::runCommand;
    using SysUtil::tryRunCommand;
    using SysUtil::haveCommand;

    /**
     * Abstract base class for all package managers.
     *
     * Concrete implementation classes:
     *
     *	 - DpkgPkgManager
     *	 - RpmPkgManager
     *	 - PacManPkgManager
     **/
    class PkgManager
    {
    public:

	/**
	 * Constructor.
	 **/
	PkgManager();

	/**
	 * Destructor.
	 **/
	virtual ~PkgManager();

	/**
	 * Return the name of this package manager.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual QString name() const = 0;

	/**
	 * Check if this package manager is active as a primary package manager
	 * on the currently running system.
	 *
	 * Derived classes are required to implement this.
	 *
	 * Remember that a system might support installing 'foreign' package
	 * managers; for example, on Debian / Ubuntu you can also install the
	 * 'rpm' package. It is strongly advised to do a more sophisticated
	 * test here than just checking if a certain executable (like
	 * /usr/bin/dpkg or /usr/bin/rpm) exists.
	 *
	 * The PkgQuery class will only execute this once at its startup phase,
	 * so this information does not need to be cached.
	 **/
	virtual bool isPrimaryPkgManager() = 0;

	/**
	 * Check if this package manager is available on the currently running
	 * system, even if just as a secondary package manager. This is a
	 * weaker check than isPrimaryPkgPanager(); just checking if the
	 * relevant binary exists and is executable (use haveCommand() for
	 * that) is sufficient.
	 *
	 * This means that this can be used as a secondary package manager; it
	 * does not manage itself, but maybe it manages some other 'foreign'
	 * packages.
	 *
	 * For example, if you install rpm.deb on Ubuntu, /usr/bin/rpm belongs
	 * to the rpm.deb package, unlike on a SUSE system where it belongs to
	 * the rpm.rpm package. Still, it probably manages some packages
	 * (albeit not itself) on such an Ubuntu system which might be useful
	 * for the purposes of this PkgQuery class.
	 **/
	virtual bool isAvailable() = 0;

	/**
	 * Return the owning package of a file or directory with full path
	 * 'path' or an empty string if it is not owned by any package.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual QString owningPkg( const QString & path ) = 0;


	//-----------------------------------------------------------------
	//		       Optional Features
	//-----------------------------------------------------------------

	/**
	 * Return 'true' if this package manager supports getting the list of
	 * installed packages.
	 **/
	virtual bool supportsGetInstalledPkg() { return false; }

	/**
	 * Return the list of installed packages.
	 *
	 * Ownership of the list elements is transferred to the caller.
	 *
	 * This is an optional feature; a package manager that implements this
	 * should also return 'true' in supportsGetInstalledPkg().
	 *
	 * This default implementation returns nothing.
	 **/
	virtual PkgInfoList installedPkg() { return PkgInfoList(); }

	/**
	 * Return the list of files and directories owned by a package.
	 **/
	virtual QStringList fileList( PkgInfo * pkg );

	/**
	 * Return 'true' if this package manager supports getting the file list
	 * for a package.
	 *
	 ** See also supportsFileListCache().
	 **/
	virtual bool supportsFileList() { return false; }

	/**
	 * Return the command for getting the list of files and directories
	 * owned by a package.
	 *
	 * This is an optional feature; a package manager that implements this
	 * should also return 'true' in supportsGetFileList().
	 *
	 * This default implementation returns nothing.
	 **/
	virtual QString fileListCommand( PkgInfo * pkg )
	    { Q_UNUSED( pkg ); return ""; }

	/**
	 * Parse the output of the file list command.
	 *
	 * This default implementation does nothing.
	 **/
	virtual QStringList parseFileList( const QString & output )
	    { Q_UNUSED( output); return QStringList(); }

	/**
	 * Return 'true' if this package manager supports building a file list
	 * cache for getting all file lists for all packages.
	 **/
	virtual bool supportsFileListCache() { return false; }

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
	 **/
	virtual PkgFileListCache * createFileListCache( PkgFileListCache::LookupType lookupType = PkgFileListCache::LookupByPkg )
	    { Q_UNUSED( lookupType ); return 0; }

	/**
	 * Return a name suitable for a detailed queries for 'pkg'.
	 */
	virtual QString queryName( PkgInfo * pkg )
	    { return pkg->name(); }

    }; // class PkgManager

} // namespace QDirStat


#endif // PkgManager_h
