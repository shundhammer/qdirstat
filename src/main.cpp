/*
 *   File name: main.cpp
 *   Summary:	QDirStat main program
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <unistd.h>     // getuid()
#include <sys/types.h>  // uid_t

#include <QApplication>
#include "MainWindow.h"
#include "Logger.h"


int main( int argc, char *argv[] )
{
    Logger logger( QString( "/tmp/qdirstat-%1.log" ).arg( getuid() ) );

    // Set org/app name for QSettings
    QCoreApplication::setOrganizationName( "QDirStat" );
    QCoreApplication::setApplicationName ( "QDirStat" );

    QApplication app( argc, argv);
    MainWindow mainWin;
    mainWin.show();

    if ( argc > 1 )
	mainWin.openUrl( argv[1] );
    else
	mainWin.askOpenUrl();

    app.exec();
}
