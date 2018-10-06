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
	 * Return the owning package of a file or directory with full path
	 * 'path' or an empty string if it is not owned by any package.
	 **/
	QString getOwningPackage( const QString & path );

	/**
	 * Try to add a package manager to the internal list.
	 **/
	void tryAdd( PkgManager * pkgManager );


	// Data members

	static PkgQuery *	 _instance;
	QList <PkgManager *>	 _pkgManagers;
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
	 * Check if this package manager is active on the currently running
	 * system.
	 *
	 * Derived classes are required to implement this.
	 *
	 * Remember that a system might support installing 'foreign' package
	 * managers; for example, on Debian / Ubuntu you can also install the
	 * 'rpm' package. It is strongly advised to do a more sophisticated
	 * test than just checking if a certain executable (like /usr/bin/dpkg
	 * or /usr/bin/rpm) exists.
	 *
	 * The PkgQuery class will only execute this once at its startup phase,
	 * so this information does not need to be cached.
	 **/
	virtual bool isActiveOnSystem() = 0;

	/**
	 * Return the owning package of a file or directory with full path
	 * 'path' or an empty string if it is not owned by any package.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual QString owningPkg( const QString & path ) = 0;

    protected:

	/**
	 * Try running a command and compare it against an expected result.
	 * Return 'true' if ok, 'false' if not.
	 **/
	bool tryRunCommand( const QString & commandLine,
			    const QRegExp & expectedResult ) const;

	/**
	 * Run a command line and return its output. If exitCode_ret is
	 * non-null, return the command's exit code in exitCode_ret.
	 *
	 * Notice 1: This uses a very basic command line parser; it simply
	 * splits the command up whereever whitespace might occur. If any of
	 * the arguments (no matter how sophisticated they might be quoted)
	 * possibly contains any whitespace, this is unsafe; in that case, use
	 * the overloaded version instead that accepts a QStringList as
	 * arguments.
	 *
	 * Notice 2: This does not start a shell with that command, it runs the
	 * command directly, so only binaries can be executed, no shell scripts
	 * or scripts of other interpreted languages. If that is desired, wrap
	 * the command into "/bin/sh -c".
	 **/
	QString runCommand( const QString & commandLine,
			    int *	    exitCode_ret ) const;

	/**
	 * Run a command with arguments 'args' and return its output. If
	 * exitCode_ret is non-null, return the command's exit code in
	 * exitCode_ret.
	 *
	 * Use this version to avoid any side effects due to command line
	 * parsing.
	 *
	 * Notice: This does not start a shell with that command, it runs the
	 * command directly, so only binaries can be executed, no shell scripts
	 * or scripts of other interpreted languages. If that is desired, use
	 * "/bin/sh" as the command, "-c" as the first argument and the command
	 * line to be executed as the second. Beware of shell quoting quirks!
	 **/
	QString runCommand( const QString &	command,
			    const QStringList & args,
			    int *		exitCode_ret ) const;

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
	virtual bool isActiveOnSystem() Q_DECL_OVERRIDE;

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


    class RpmPkgManager: public PkgManager
    {
    public:

	RpmPkgManager() {}
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
	virtual bool isActiveOnSystem() Q_DECL_OVERRIDE;

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

    }; // class RpmPkgManager

} // namespace QDirStat


#endif // PkgManager_h
