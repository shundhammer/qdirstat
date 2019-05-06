/*
 *   File name: RpmPkgManager.cpp
 *   Summary:	RPM package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "RpmPkgManager.h"
#include "Logger.h"
#include "Exception.h"

#define GET_PKGLIST_TIMEOUT_SEC         30


using namespace QDirStat;


RpmPkgManager::RpmPkgManager()
{
    if ( haveCommand( "/usr/bin/rpm" ) )
	_rpmCommand = "/usr/bin/rpm";
    else
	_rpmCommand = "/bin/rpm"; // for old SUSE / Red Hat distros

    // Notice that it is not enough to rely on a symlink /bin/rpm ->
    // /usr/bin/rpm: While recent SUSE distros have that symlink (and maybe Red
    // Hat and Fedora as well?), rpm as a secondary package manager on Ubuntu
    // does not have such a link; they only have /usr/bin/rpm.
    //
    // Also intentionally never leaving _rpmCommand empty if it is not
    // available to avoid unpleasant surprises if a caller tries to use any
    // other method of this class that refers to it.
}


bool RpmPkgManager::isPrimaryPkgManager()
{
    return tryRunCommand( QString( "%1 -qf %1" ).arg( _rpmCommand ),
			  QRegExp( "^rpm.*" ) );
}


bool RpmPkgManager::isAvailable()
{
    return haveCommand( _rpmCommand );
}


QString RpmPkgManager::owningPkg( const QString & path )
{
    int exitCode = -1;
    QString output = runCommand( _rpmCommand,
				 QStringList() << "-qf" << "--queryformat" << "%{name}" << path,
				 &exitCode );

    if ( exitCode != 0 || output.contains( "not owned by any package" ) )
	return "";

    QString pkg = output;

    return pkg;
}


PkgInfoList RpmPkgManager::installedPkg()
{
    int exitCode = -1;
    QString output = runCommand( _rpmCommand,
                                 QStringList()
                                 << "-qa"
                                 << "--queryformat"
                                 << "%{name} | %{version}-%{release} | %{arch}\n",
                                 &exitCode,
                                 GET_PKGLIST_TIMEOUT_SEC );

    PkgInfoList pkgList;

    if ( exitCode == 0 )
        pkgList = parsePkgList( output );

    return pkgList;
}


PkgInfoList RpmPkgManager::parsePkgList( const QString & output )
{
    PkgInfoList pkgList;

    foreach ( const QString & line, output.split( "\n" ) )
    {
        if ( ! line.isEmpty() )
        {
            QStringList fields = line.split( " | ", QString::KeepEmptyParts );

            if ( fields.size() != 3 )
                logError() << "Invalid rpm -qa output: " << line << "\n" << endl;
            else
            {
                QString name    = fields.takeFirst();
                QString version = fields.takeFirst(); // includes release
                QString arch    = fields.takeFirst();

                if ( arch == "(none)" )
                    arch = "";

                PkgInfo * pkg = new PkgInfo( name, version, arch, this );
                CHECK_NEW( pkg );

                pkgList << pkg;
            }
        }
    }

    return pkgList;
}


QString RpmPkgManager::fileListCommand( PkgInfo * pkg )
{
    return QString( "%1 -ql %2" )
        .arg( _rpmCommand )
        .arg( queryName( pkg ) );
}


QStringList RpmPkgManager::parseFileList( const QString & output )
{
    QStringList fileList;

    fileList = output.split( "\n" );
    fileList.removeAll( "(contains no files)" );

    return fileList;
}


QString RpmPkgManager::queryName( PkgInfo * pkg )
{
    CHECK_PTR( pkg );

    QString name = pkg->baseName();

    if ( ! pkg->version().isEmpty() )
        name += "-" + pkg->version();

    if ( ! pkg->arch().isEmpty() )
        name += "." + pkg->arch();

    return name;
}
