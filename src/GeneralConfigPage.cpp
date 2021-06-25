/*
 *   File name: GeneralConfigPage.cpp
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "GeneralConfigPage.h"
#include "Settings.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


GeneralConfigPage::GeneralConfigPage( QWidget * parent ):
    QWidget( parent ),
    _ui( new Ui::GeneralConfigPage )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );
    readSettings();
}


GeneralConfigPage::~GeneralConfigPage()
{
    delete _ui;
}


void GeneralConfigPage::setup()
{
    // NOP
}


void GeneralConfigPage::applyChanges()
{
    // logDebug() << endl;

    writeSettings();
}


void GeneralConfigPage::discardChanges()
{
    // logDebug() << endl;

    readSettings();
}


void GeneralConfigPage::readSettings()
{
    // logDebug() << endl;

    QDirStat::Settings settings;
    settings.beginGroup( "MainWindow" );

    _ui->urlInWindowTitleCheckBox->setChecked( settings.value( "UrlInWindowTitle" , false ).toBool() );
    _ui->useTreemapHoverCheckBox->setChecked ( settings.value( "UseTreemapHover"  , false ).toBool() );

    int statusBarTimeoutMillisec = settings.value( "StatusBarTimeoutMillisec" , 3000 ).toInt();
    _ui->statusBarTimeoutSpinBox->setValue( statusBarTimeoutMillisec / 1000.0 );

    settings.endGroup();


    settings.beginGroup( "DirectoryTree" );

    _ui->crossFilesystemsCheckBox->setChecked( settings.value( "CrossFilesystems"    , false ).toBool() );
    _ui->treeUpdateIntervalSpinBox->setValue ( settings.value( "UpdateTimerMillisec" ,   333 ).toInt()  );
    QString treeIconDir = settings.value( "TreeIconDir", ":/icons/tree-medium/" ).toString();

    int index = treeIconDir.contains( "/tree-small" ) ? 1 : 0;
    _ui->treeIconThemeComboBox->setCurrentIndex( index );

    settings.endGroup();
}


void GeneralConfigPage::writeSettings()
{
    // logDebug() << endl;

    QDirStat::Settings settings;
    settings.beginGroup( "MainWindow" );

    settings.setValue( "UrlInWindowTitle"        , _ui->urlInWindowTitleCheckBox->isChecked() );
    settings.setValue( "UseTreemapHover"         , _ui->useTreemapHoverCheckBox->isChecked()  );
    settings.setValue( "StatusBarTimeoutMillisec", (int) ( 1000 * _ui->statusBarTimeoutSpinBox->value() ) );

    settings.endGroup();


    settings.beginGroup( "DirectoryTree" );

    settings.setValue( "CrossFilesystems"    , _ui->crossFilesystemsCheckBox->isChecked() );
    settings.setValue( "UpdateTimerMillisec" , _ui->treeUpdateIntervalSpinBox->value()    );

    switch ( _ui->treeIconThemeComboBox->currentIndex() )
    {
        default:
        case 0:  settings.setValue( "TreeIconDir", ":/icons/tree-medium/" ); break;
        case 1:  settings.setValue( "TreeIconDir", ":/icons/tree-small/"  ); break;
    }

    settings.endGroup();
}
