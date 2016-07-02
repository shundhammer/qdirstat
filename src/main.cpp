/*
 *   File name: main.cpp
 *   Summary:	QDirStat main program
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <unistd.h>	// getuid()
#include <sys/types.h>	// uid_t
#include <iostream>	// cerr

#include <QApplication>
#include "MainWindow.h"
#include "Logger.h"

using std::cerr;
static const char * progName = "qdirstat";
static bool fatal = false;


void usage( const QStringList & argList )
{
    cerr << "\n"
	 << "Usage: \n"
	 << "\n"
	 << "  " << progName << " [<directory-name>]\n"
	 << "  " << progName << " --cache|-c <cache-file-name>\n"
	 << "  " << progName << " --help|-h\n"
	 << std::endl;

    logError() << "FATAL: Bad command line args: " << argList.join( " " ) << endl;
    // Simply exit(1) here results in a segfault (?).
    fatal = true;
}


int main( int argc, char *argv[] )
{
    Logger logger( QString( "/tmp/qdirstat-%1.log" ).arg( getuid() ) );

    // Set org/app name for QSettings
    QCoreApplication::setOrganizationName( "QDirStat" );
    QCoreApplication::setApplicationName ( "QDirStat" );

    QApplication app( argc, argv);
    QStringList argList = QCoreApplication::arguments();
    argList.removeFirst(); // Remove program name

    MainWindow mainWin;
    mainWin.show();

    if ( argList.isEmpty() )
	mainWin.askOpenUrl();
    else
    {
	QString arg = argList.first();

	if ( arg == "--cache" || arg == "-c" )
	{
	    if ( argList.size() == 2 )
	    {
		QString cacheFileName = argList.at(1);
		logDebug() << "Reading cache file " << cacheFileName << endl;
		mainWin.readCache( cacheFileName );
	    }
	    else
		usage( argList );
	}
	else if ( arg == "--help" || arg == "-h" )
	    usage( argList );
	else if ( arg.startsWith( "-" ) )
	    usage( argList );
	else if ( ! arg.isEmpty() )
	{
	    mainWin.openUrl( arg );
	}
    }

    if ( ! fatal )
	app.exec();

    return fatal ? 1 : 0;
}
