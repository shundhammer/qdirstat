/*
 *   File name: GeneralConfigPage.cpp
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "GeneralConfigPage.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


GeneralConfigPage::GeneralConfigPage( QWidget * parent ):
    QWidget( parent ),
    _ui( new Ui::GeneralConfigPage )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );
}


GeneralConfigPage::~GeneralConfigPage()
{
    logDebug() << endl;

}


void GeneralConfigPage::setup()
{
    logDebug() << endl;

}


void GeneralConfigPage::applyChanges()
{
    logDebug() << endl;

}


void GeneralConfigPage::discardChanges()
{
    logDebug() << endl;
}


void GeneralConfigPage::readSettings()
{
    logDebug() << endl;
}


void GeneralConfigPage::writeSettings()
{
    logDebug() << endl;
}
