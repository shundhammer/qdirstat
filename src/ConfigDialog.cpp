/*
 *   File name: ConfigDialog.cpp
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "ConfigDialog.h"
#include "CleanupConfigPage.h"
#include "MimeCategoryConfigPage.h"
#include "ExcludeRulesConfigPage.h"
#include "GeneralConfigPage.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;

#define CONNECT_CONFIG_PAGE(PAGE)			\
							\
    connect( this,   SIGNAL( reinit() ),		\
	     (PAGE), SLOT  ( setup()  ) );		\
							\
    connect( this,   SIGNAL( applyChanges() ),		\
	     (PAGE), SLOT  ( applyChanges() ) );	\
							\
    connect( this,   SIGNAL( discardChanges() ),	\
	     (PAGE), SLOT  ( discardChanges() ) )



ConfigDialog::ConfigDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::ConfigDialog )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this );

    _cleanupConfigPage = new CleanupConfigPage( this );
    CHECK_NEW( _cleanupConfigPage );
    _ui->pagesTabWidget->addTab( _cleanupConfigPage, tr( "Cleanup Actions" ) );

    _mimeCategoryConfigPage = new MimeCategoryConfigPage( this );
    CHECK_NEW( _mimeCategoryConfigPage );
    _ui->pagesTabWidget->addTab( _mimeCategoryConfigPage, tr( "MIME Categories" ) );

    _excludeRulesConfigPage = new ExcludeRulesConfigPage( this );
    CHECK_NEW( _excludeRulesConfigPage );
    _ui->pagesTabWidget->addTab( _excludeRulesConfigPage, tr( "Exclude Rules" ) );

    _generalConfigPage = new GeneralConfigPage( this );
    CHECK_NEW( _generalConfigPage );
    _ui->pagesTabWidget->addTab( _generalConfigPage, tr( "General" ) );

    connect( _ui->applyButton,	 SIGNAL( clicked() ),
	     this,		 SLOT  ( apply()   ) );

    CONNECT_CONFIG_PAGE( _cleanupConfigPage      );
    CONNECT_CONFIG_PAGE( _mimeCategoryConfigPage );
    CONNECT_CONFIG_PAGE( _excludeRulesConfigPage );
    CONNECT_CONFIG_PAGE( _generalConfigPage      );
}


ConfigDialog::~ConfigDialog()
{
    // logDebug() << "ConfigDialog destructor" << endl;
    delete _ui;
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
