/*
 *   File name: GeneralConfigPage.cpp
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "GeneralConfigPage.h"
#include "DirTreeModel.h"
#include "MainWindow.h"
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
    // NOP
}


void GeneralConfigPage::setup()
{
    // NOP
}


static DirTreeModel * dirTreeModel()
{
    DirTreeModel * result = 0;
    const MainWindow * window = MainWindow::activeWindow();
    if ( ! window )
        return result;
    return window->dirTreeModel();
}


void GeneralConfigPage::applyChanges()
{
    // logDebug() << endl;

    writeSettings();

    DirTreeModel * model = dirTreeModel();
    if ( model )
        model->readSettings();
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

    _ui->crossFileSystemsCheckBox->setChecked  ( settings.value( "CrossFileSystems"    , false       ).toBool() );
    _ui->excludeDirWithFileCheckBox->setChecked( settings.value( "ExcludeDirWithFile"  , false       ).toBool() );
    _ui->excludeDirFilenameLineEdit->setText   ( settings.value( "ExcludeDirFilename"  , ".nobackup" ).toString() );
    _ui->treeUpdateIntervalSpinBox->setValue   ( settings.value( "UpdateTimerMillisec" , 333         ).toInt() );
    QString treeIconDir = settings.value( "TreeIconDir", ":/icons/tree-medium" ).toString();

    int index = treeIconDir.endsWith( "medium" ) ? 0 : 1;
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

    settings.setValue( "CrossFileSystems"    , _ui->crossFileSystemsCheckBox->isChecked()   );
    settings.setValue( "ExcludeDirWithFile"  , _ui->excludeDirWithFileCheckBox->isChecked() );
    settings.setValue( "ExcludeDirFilename"  , _ui->excludeDirFilenameLineEdit->text()      );
    settings.setValue( "UpdateTimerMillisec" , _ui->treeUpdateIntervalSpinBox->value()      );

    switch ( _ui->treeIconThemeComboBox->currentIndex() )
    {
        default:
        case 0:  settings.setValue( "TreeIconDir", ":/icons/tree-medium" ); break;
        case 1:  settings.setValue( "TreeIconDir", ":/icons/tree-small"  ); break;
    }

    settings.endGroup();
}
