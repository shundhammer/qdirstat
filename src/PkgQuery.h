/*
 *   File name: PkgQuery.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef PkgQuery_h
#define PkgQuery_h

#include <QString>

namespace QDirStat
{
    /**
     * Class for simple queries to the system's package manager.
     **/
    class PkgQuery
    {
    public:

        /**
         * Return the owning package of a file or directory with full path
         * 'path' or an empty string if it is not owned by any package.
         **/
        static QString owningPackage( const QString & path );

        /**
         * Return the singleton instance of this class.
         **/
        static PkgQuery * instance();

    protected:

        PkgQuery();
        virtual ~PkgQuery();


        // Data members

        static PkgQuery *     _instance;
        QList <PkgManager *>  _pkgManagers;

    }; // class PkgQuery


    /**
     * Abstract base class for all package managers.
     **/
    class PkgManager
    {
    public:
        PkgManager();
        virtual ~PkgManager();

        /**
         * Check if this package manager is active on the currently running system.
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
        virtual QString owningPackage( const QString & path ) = 0;
    };


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

        /**
         * Check if dpkg is active on the currently running system.
         *
         * Implemented from PkgManager.
         *
         * This basically checks with this command:
         *
         *   test -x /usr/bin/dpkg && /usr/bin/dpkg -S /usr/bin/dpkg | grep -q '^dpkg:'
         **/
        virtual bool isActiveOnSystem() Q_OVERRIDE;

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
        virtual QString owningPackage( const QString & path ) Q_OVERRIDE;
    };


    class RpmPkgManager: public PkgManager
    {
    public:

        /**
         * Check if RPM is active on the currently running system.
         *
         * Implemented from PkgManager.
         *
         * This basically checks with this command:
         *
         *   test -x /usr/bin/rpm && /usr/bin/rpm -qf /usr/bin/rpm | grep -q '????'
         **/
        virtual bool isActiveOnSystem() Q_OVERRIDE;

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
        virtual QString owningPackage( const QString & path ) Q_OVERRIDE;
    };

} // namespace QDirStat


#endif // PkgQuery_h
