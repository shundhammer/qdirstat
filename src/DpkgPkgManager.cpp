/*
 *   File name: DpkgPkgManager.cpp
 *   Summary:	Dpkg package manager support for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DpkgPkgManager.h"
#include "PkgFileListCache.h"
#include "Logger.h"
#include "Exception.h"

#define LOG_COMMANDS	true
#define LOG_OUTPUT	false
#include "SysUtil.h"


using namespace QDirStat;

using SysUtil::runCommand;
using SysUtil::tryRunCommand;
using SysUtil::haveCommand;


bool DpkgPkgManager::isPrimaryPkgManager()
{
    return tryRunCommand( "/usr/bin/dpkg -S /usr/bin/dpkg", QRegExp( "^dpkg:.*" ) );
}


bool DpkgPkgManager::isAvailable()
{
    return haveCommand( "/usr/bin/dpkg" );
}


QString DpkgPkgManager::owningPkg( const QString & path )
{
    int exitCode = -1;
    QString output = runCommand( "/usr/bin/dpkg", QStringList() << "-S" << path, &exitCode );

    if ( exitCode != 0 || output.contains( "no path found matching pattern" ) )
	return "";

    // Normal output: One line
    // dpkg -S /usr/bin/gdb  -->
    //
    // gdb: /usr/bin/gdb
    //
    //
    // Pathological cases: File diversion (See man dpkg-divert).
    //
    // File diversion for the file that is diverted away from: 3 lines
    // dpkg -S /bin/sh	-->
    //
    // diversion by dash from: /bin/sh
    // diversion by dash to: /bin/sh.distrib
    // dash: /bin/sh
    //
    // I.e. the last line still contains the package that owns the file.
    //
    // File diversion for a file that was the result of a diversion: 2 lines
    // dpkg -S /bin/sh.distrib	-->
    //
    // diversion by dash from: /bin/sh
    // diversion by dash to: /bin/sh.distrib
    //
    // I.e there is no hint WTF this file belongs to. Great job, guys.
    // We are NOT going to reimplement that brain-dead diversion stuff here.

    QStringList lines;

    foreach ( const QString & line, output.trimmed().split( "\n" ) )
    {
	if ( ! line.startsWith( "diversion by" ) && ! line.isEmpty() )
	    lines << line;
    }

    QString pkg;

    if ( ! lines.isEmpty() )
	pkg = lines.last().split( ": " ).first();

    return pkg;
}


PkgInfoList DpkgPkgManager::installedPkg()
{
    int exitCode = -1;
    QString output = runCommand( "/usr/bin/dpkg-query",
				 QStringList()
				 << "--show"
				 << "--showformat=${Package} | ${Version} | ${Architecture} | ${Status}\n",
				 &exitCode );

    PkgInfoList pkgList;

    if ( exitCode == 0 )
	pkgList = parsePkgList( output );

    return pkgList;
}


PkgInfoList DpkgPkgManager::parsePkgList( const QString & output )
{
    PkgInfoList pkgList;

    foreach ( const QString & line, output.split( "\n" ) )
    {
	if ( ! line.isEmpty() )
	{
	    QStringList fields = line.split( " | ", QString::KeepEmptyParts );

	    if ( fields.size() != 4 )
		logError() << "Invalid dpkg-query output: \"" << line << "\n" << endl;
	    else
	    {
		QString name	= fields.takeFirst();
		QString version = fields.takeFirst();
		QString arch	= fields.takeFirst();
		QString status	= fields.takeFirst();

		if ( status == "install ok installed" ||
                     status == "hold ok installed"       )
		{
		    PkgInfo * pkg = new PkgInfo( name, version, arch, this );
		    CHECK_NEW( pkg );

		    pkgList << pkg;
		}
		else
		{
		    // logDebug() << "Ignoring " << line << endl;
		}
	    }
	}
    }

    return pkgList;
}


QString DpkgPkgManager::fileListCommand( PkgInfo * pkg )
{
    return QString( "/usr/bin/dpkg-query --listfiles %1" ).arg( queryName( pkg ) );
}


QStringList DpkgPkgManager::parseFileList( const QString & output )
{
    QStringList fileList;
    QStringList lines = output.split( "\n" );

    foreach ( const QString & line, lines )
    {
	if ( line != "/." && ! line.startsWith( "package diverts" ) )
	    fileList << line;
    }

    return fileList;
}


QString DpkgPkgManager::queryName( PkgInfo * pkg )
{
    CHECK_PTR( pkg );

    QString name = pkg->baseName();

#if 0
    if ( pkg->isMultiVersion() )
	name += "_" + pkg->version();

    if ( pkg->isMultiArch() )
	name += ":" + pkg->arch();
#endif

    if ( pkg->arch() != "all" )
	name += ":" + pkg->arch();

    return name;
}


PkgFileListCache * DpkgPkgManager::createFileListCache( PkgFileListCache::LookupType lookupType )
{
    int exitCode = -1;
    QString output = runCommand( "/usr/bin/dpkg", QStringList() << "-S" << "*", &exitCode );

    if ( exitCode != 0 )
	return 0;

    QStringList lines = output.split( "\n" );
    output.clear(); // Free all that text ASAP
    logDebug() << lines.size() << " output lines" << endl;

    PkgFileListCache * cache = new PkgFileListCache( this, lookupType );
    CHECK_NEW( cache );

    // Sample output:
    //
    //	   zip: /usr/bin/zip
    //	   zlib1g-dev:amd64: /usr/include/zlib.h
    //	   zlib1g:i386, zlib1g:amd64: /usr/share/doc/zlib1g

    foreach ( const QString & line, lines )
    {
	if ( line.isEmpty() || line.startsWith( "diversion" ) )
	    continue;

	QStringList fields = line.split( ": " );

	if ( fields.size() != 2 )
	{
	    logError() << "Unexpected file list line: \"" << line << "\"" << endl;
	}
	else
	{
	    QString packages = fields.takeFirst();
	    QString path     = fields.takeFirst();

	    if ( path != "/." && ! path.isEmpty() )
	    {
		foreach ( const QString & pkgName, packages.split( ", " ) )
		{
		    if ( ! pkgName.isEmpty() )
			cache->add( pkgName, path );
		}
	    }
	}
    }

    logDebug() << "file list cache finished." << endl;

    return cache;
}

