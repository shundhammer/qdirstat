
#include <QApplication>
#include "MainWindow.h"
#include "Logger.h"


int main( int argc, char *argv[] )
{
    Logger logger( "/tmp/qdirstat.log" );

    QApplication app( argc, argv);
    MainWindow mainWin;
    mainWin.show();

    app.exec();
}
