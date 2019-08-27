/*
 *   File name: RpmPkgManager.cpp
 *   Summary:	RPM package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <iostream>	// cerr

#include <QElapsedTimer>
#include <QPointer>

#include "RpmPkgManager.h"
#include "PkgFileListCache.h"
#include "Settings.h"
#include "MessagePanel.h"
#include "PanelMessage.h"
#include "Logger.h"
#include "Exception.h"

#define LONG_CMD_TIMEOUT_SEC		30


using std::cerr;
using std::endl;
using namespace QDirStat;


RpmPkgManager::RpmPkgManager():
    _getPkgListWarningSec( 7 )
{
    readSettings();

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
    QElapsedTimer timer;
    timer.start();

    QString output = runCommand( _rpmCommand,
				 QStringList()
				 << "-qa"
				 << "--queryformat"
				 << "%{name} | %{version}-%{release} | %{arch}\n",
				 &exitCode,
				 LONG_CMD_TIMEOUT_SEC );

    if ( timer.hasExpired( _getPkgListWarningSec * 1000 ) )
	rebuildRpmDbWarning();

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
		QString name	= fields.takeFirst();
		QString version = fields.takeFirst(); // includes release
		QString arch	= fields.takeFirst();

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


PkgFileListCache * RpmPkgManager::createFileListCache( PkgFileListCache::LookupType lookupType )
{
    int exitCode = -1;
    QString queryFormat = "[%{=NAME}-%{=VERSION}-%{=RELEASE}.%{=ARCH} | %{FILENAMES}\n]";

    QString output = runCommand( _rpmCommand,
				 QStringList() << "-qa" << "--qf" << queryFormat,
				 &exitCode,
				 LONG_CMD_TIMEOUT_SEC );

    if ( exitCode != 0 )
	return 0;

    QStringList lines = output.split( "\n" );
    output.clear(); // Free all that text ASAP
    logDebug() << lines.size() << " output lines" << endl;

    PkgFileListCache * cache = new PkgFileListCache( this, lookupType );
    CHECK_NEW( cache );

    // Sample output:
    //
    //	   zsh-5.6-lp151.1.3.x86_64 | /bin/zsh
    //	   zsh-5.6-lp151.1.3.x86_64 | /etc/zprofile
    //	   zsh-5.6-lp151.1.3.x86_64 | /etc/zsh_completion.d

    foreach ( const QString & line, lines )
    {
	if ( line.isEmpty() )
	    continue;

	QStringList fields = line.split( " | " );

	if ( fields.size() != 2 )
	{
	    logError() << "Unexpected file list line: \"" << line << "\"" << endl;
	}
	else
	{
	    QString pkgName = fields.takeFirst();
	    QString path    = fields.takeFirst();

	    if ( ! pkgName.isEmpty() && ! path.isEmpty() )
		cache->add( pkgName, path );
	}
    }

    logDebug() << "file list cache finished." << endl;

    return cache;
}


void RpmPkgManager::readSettings()
{
    Settings settings;
    settings.beginGroup( "Pkg" );
    _getPkgListWarningSec = settings.value( "GetRpmPkgListWarningSec", 7 ).toInt();

    // Write the value right back to the settings if it isn't there already:
    // Since package manager objects are never really destroyed, this can't
    // reliably be done in the destructor.

    settings.setDefaultValue( "GetRpmPkgListWarningSec", _getPkgListWarningSec );
    settings.endGroup();
}


void RpmPkgManager::rebuildRpmDbWarning()
{
    static bool issuedWarning = false;

    if ( ! issuedWarning )
    {
	cerr << "WARNING: rpm is very slow. Run	  sudo rpm --rebuilddb\n" << endl;
	logWarning()  << "rpm is very slow. Run	  sudo rpm --rebuilddb"	  << endl;
    }

    // Add a panel message so the user is sure to see this message.
    //
    // This is a bit out of place in this class, but a full-fledged user
    // messaging system seemed to be overkill just for this one message.

    // Need a guarded pointer because the [x] close button in the PanelMessage
    // deletes the instance.

    static QPointer<PanelMessage> panelMessage;

    if ( MessagePanel::haveInstance() && ! panelMessage )
    {
	panelMessage = new PanelMessage();
	CHECK_NEW( panelMessage );

	panelMessage->setHeading( QObject::tr( "RPM is very slow." ) );
	panelMessage->setText( QObject::tr( "Open a shell window and run:<br>%1" )
                               .arg( "<tt>sudo rpm --rebuilddb</tt>" ) );
	panelMessage->setIcon( QPixmap( ":/icons/dialog-warning.png" ) );

	MessagePanel::firstInstance()->add( panelMessage );
    }

    issuedWarning = true;
}
