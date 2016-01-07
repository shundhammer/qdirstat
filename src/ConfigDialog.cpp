/*
 *   File name: ConfigDialog.cpp
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "ConfigDialog.h"
#include "CleanupConfigPage.h"
#include "CleanupCollection.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


ConfigDialog::ConfigDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::ConfigDialog )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this );

    _cleanupConfigPage = new CleanupConfigPage();
    _ui->pagesTabWidget->addTab( _cleanupConfigPage, tr( "Cleanup Actions" ) );

    connect( _ui->applyButton,   SIGNAL( clicked() ),
             this,               SLOT  ( apply()   ) );

    connect( this,               SIGNAL( reinit() ),
             _cleanupConfigPage, SLOT  ( setup()  ) );

    connect( this,               SIGNAL( applyChanges() ),
             _cleanupConfigPage, SLOT  ( applyChanges() ) );

    connect( this,               SIGNAL( discardChanges() ),
             _cleanupConfigPage, SLOT  ( discardChanges() ) );
}


ConfigDialog::~ConfigDialog()
{
    logDebug() << "ConfigDialog destructor" << endl;
}


void ConfigDialog::setup()
{
    emit reinit();
}


void ConfigDialog::apply()
{
    emit applyChanges();
}


void ConfigDialog::accept()
{
    apply();
    done( Accepted );
}


void ConfigDialog::reject()
{
    emit discardChanges();
    done( Rejected );
}
