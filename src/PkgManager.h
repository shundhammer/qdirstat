/*
 *   File name: PkgManager.h
 *   Summary:	Simple package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef PkgManager_h
#define PkgManager_h

#include <QString>
#include <QCache>


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
	 * Return the owning package of a file or directory with full path
	 * 'path' or an empty string if it is not owned by any package.
	 **/
	QString getOwningPackage( const QString & path );

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


    /**
     * Abstract base class for all package managers.
     **/
    class PkgManager
    {
    public:

	/**
	 * Constructor.
	 **/
	PkgManager() {}

	/**
	 * Destructor.
	 **/
	virtual ~PkgManager() {}

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

    }; // class PkgManager


    /**
     * Simple interface to 'dpkg' for all Debian-based Linux distros.
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

    }; // class DpkgPkgManager


    /**
     * Simple interface to 'rpm' for all RPM-based Linux distros such as SUSE,
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


    protected:

	QString _rpmCommand;

    }; // class RpmPkgManager


    /**
     * Simple interface to 'pacman' for Manjaro / Arch Linux.
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

    }; // class PacManPkgManager

} // namespace QDirStat


#endif // PkgManager_h
