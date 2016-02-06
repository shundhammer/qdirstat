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
    CHECK_NEW( _cleanupConfigPage );
    _ui->pagesTabWidget->addTab( _cleanupConfigPage, tr( "Cleanup Actions" ) );

    _mimeCategoryConfigPage = new MimeCategoryConfigPage();
    CHECK_NEW( _mimeCategoryConfigPage );
    _ui->pagesTabWidget->addTab( _mimeCategoryConfigPage, tr( "MIME Categories" ) );

    _excludeRulesConfigPage = new ExcludeRulesConfigPage();
    CHECK_NEW( _excludeRulesConfigPage );
    _ui->pagesTabWidget->addTab( _excludeRulesConfigPage, tr( "Exclude Rules" ) );

    connect( _ui->applyButton,	 SIGNAL( clicked() ),
	     this,		 SLOT  ( apply()   ) );

    //
    // Connect cleanup config page
    //

    connect( this,		 SIGNAL( reinit() ),
	     _cleanupConfigPage, SLOT  ( setup()  ) );

    connect( this,		 SIGNAL( applyChanges() ),
	     _cleanupConfigPage, SLOT  ( applyChanges() ) );

    connect( this,		 SIGNAL( discardChanges() ),
	     _cleanupConfigPage, SLOT  ( discardChanges() ) );

    //
    // Connect mime category config page
    //

    connect( this,		      SIGNAL( reinit() ),
	     _mimeCategoryConfigPage, SLOT  ( setup()  ) );

    connect( this,		      SIGNAL( applyChanges() ),
	     _mimeCategoryConfigPage, SLOT  ( applyChanges() ) );

    connect( this,		      SIGNAL( discardChanges() ),
	     _mimeCategoryConfigPage, SLOT  ( discardChanges() ) );
    
    //
    // Connect exclude rules config page
    //

    connect( this,		      SIGNAL( reinit() ),
	     _excludeRulesConfigPage, SLOT  ( setup()  ) );

    connect( this,		      SIGNAL( applyChanges() ),
	     _excludeRulesConfigPage, SLOT  ( applyChanges() ) );

    connect( this,		      SIGNAL( discardChanges() ),
	     _excludeRulesConfigPage, SLOT  ( discardChanges() ) );
}


ConfigDialog::~ConfigDialog()
{
    // logDebug() << "ConfigDialog destructor" << endl;
    delete _cleanupConfigPage;
    delete _mimeCategoryConfigPage;
    delete _excludeRulesConfigPage;
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
