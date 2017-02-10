/*
 *   File name: FileTypeStatsWindow.cpp
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileTypeStatsWindow.h"
#include "DirTree.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


FileTypeStatsWindow::FileTypeStatsWindow( DirTree * tree,
                                          QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FileTypeStatsWindow ),
    _tree( tree )
{
    logDebug() << "init" << endl;
    _ui->setupUi( this );
    calc();
}


FileTypeStatsWindow::~FileTypeStatsWindow()
{
    logDebug() << "dtor" << endl;
}


void FileTypeStatsWindow::calc()
{
    logDebug() << "Calculating" << endl;
}


void FileTypeStatsWindow::reject()
{
    deleteLater();
}

