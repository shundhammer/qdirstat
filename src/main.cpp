/*
 *   File name: main.cpp
 *   Summary:	QDirStat main program
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <iostream>	// cerr

#include <QApplication>
#include "QDirStatApp.h"
#include "MainWindow.h"
#include "DirTreeModel.h"
#include "PkgFilter.h"
#include "Settings.h"
#include "Logger.h"
#include "Exception.h"
#include "Version.h"


using std::cerr;
static const char * progName = "qdirstat";
static bool fatal = false;


void usage( const QStringList & argList )
{
    cerr << "\n"
	 << "Usage: \n"
	 << "\n"
	 << "  " << progName << " [--slow-update|-s] [<directory-name>]\n"
	 << "  " << progName << " pkg:/pkgpattern\n"
	 << "  " << progName << " unpkg:/dir\n"
	 << "  " << progName << " --dont-ask|-d\n"
	 << "  " << progName << " --cache|-c <cache-file-name>\n"
	 << "  " << progName << " --help|-h\n"
	 << "\n"
	 << "\n"
         << "Supported pkg patterns:\n"
	 << "\n"
         << "- Default: \"Starts with\" \"pkg:/mypkg\"\n"
         << "- Wildcards with '*' and '?'\n"
         << "- Full regexps with \".*\" and/or \"^\" and/or \"$\"\n"
         << "- Exact match: \"pkg:/=mypkg\"\n"
         << "- All packages: \"pkg:/\"\n"
	 << "\n"
         << "See also   man qdirstat"
	 << "\n"
	 << std::endl;

    logError() << "FATAL: Bad command line args: " << argList.join( " " ) << endl;
    // Simply exit(1) here results in a segfault (?).
    fatal = true;
}


void logVersion()
{
    logInfo() << "QDirStat-" << QDIRSTAT_VERSION
              << " built with Qt " << QT_VERSION_STR
              << endl;

#if (QT_VERSION < QT_VERSION_CHECK( 5, 2, 0 ))
    logWarning() << "WARNING: You are using Qt " << QT_VERSION_STR
                 << ". This may or may not work." << endl;
    logWarning() << "The supported Qt version for QDirStat is Qt 5.2 or newer." << endl;
#endif
}


/**
 * Extract a command line switch (a command line argument without any
 * additional parameter) from the command line and remove it from 'argList'.
 **/
bool commandLineSwitch( const QString & longName,
			const QString & shortName,
			QStringList   & argList )
{
    if ( argList.contains( longName  ) ||
	 argList.contains( shortName )	 )
    {
	argList.removeAll( longName  );
	argList.removeAll( shortName );
        logDebug() << "Found " << longName << endl;
	return true;
    }
    else
    {
        // logDebug() << "No " << longName << endl;
	return false;
    }
}


int main( int argc, char *argv[] )
{
    Logger logger( "/tmp/qdirstat-$USER", "qdirstat.log" );
    logVersion();

    // Set org/app name for QSettings
    QCoreApplication::setOrganizationName( "QDirStat" );
    QCoreApplication::setApplicationName ( "QDirStat" );

    QApplication qtApp( argc, argv);
    QStringList argList = QCoreApplication::arguments();
    argList.removeFirst(); // Remove program name

    MainWindow * mainWin = new MainWindow();
    CHECK_PTR( mainWin );
    mainWin->show();

    bool dont_ask = commandLineSwitch( "--dont-ask", "-d", argList );

    if ( commandLineSwitch( "--slow-update", "-s", argList ) )
        QDirStat::app()->dirTreeModel()->setSlowUpdate();

    if ( argList.isEmpty() )
    {
        if ( ! dont_ask )
            mainWin->askOpenDir();
    }
    else
    {
	QString arg = argList.first();

	if ( arg == "--cache" || arg == "-c" )
	{
	    if ( argList.size() == 2 )
	    {
		QString cacheFileName = argList.at(1);
		logDebug() << "Reading cache file " << cacheFileName << endl;
		mainWin->readCache( cacheFileName );
	    }
	    else
		usage( argList );
	}
	else if ( arg == "--help" || arg == "-h" )
	    usage( argList );
	else if ( arg.startsWith( "-" ) || argList.size() > 1 )
	    usage( argList );
	else if ( ! arg.isEmpty() )
	{
            mainWin->openUrl( arg );
	}
    }

    if ( ! fatal )
	qtApp.exec();

    delete mainWin;

    // If running with 'sudo', this would leave all config files behind owned
    // by root which means that the real user can't write to those files
    // anymore if once invoking QDirStat with 'sudo'. Fixing the file owner for
    // our config files if possible.
    QDirStat::Settings::fixFileOwners();

    return fatal ? 1 : 0;
}
