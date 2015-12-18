/*
 *   File name: main.cpp
 *   Summary:	QDirStat main program
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QApplication>
#include "MainWindow.h"
#include "Logger.h"


int main( int argc, char *argv[] )
{
    Logger logger( "/tmp/qdirstat.log" );

    QApplication app( argc, argv);
    MainWindow mainWin;
    mainWin.show();

    if ( argc > 1 )
        mainWin.openUrl( argv[1] );

    app.exec();
}
