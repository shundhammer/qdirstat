/*
 *   File name: FindFilesDialog.cpp
 *   Summary:	QDirStat "Find Files" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Qt4Compat.h"       // qEnableClearButton()

#include "FindFilesDialog.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;

// Storing this here instead of writing it to file with writeSettings()
static QString lastPattern;


FindFilesDialog::FindFilesDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FindFilesDialog )
{
    // logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    qEnableClearButton( _ui->patternField );
    _ui->patternField->setFocus();

    connect( this, SIGNAL( accepted()	   ),
	     this, SLOT	 ( writeSettings() ) );

    readSettings();
}


FindFilesDialog::~FindFilesDialog()
{
    delete _ui;
}


FileSearchFilter FindFilesDialog::fileSearchFilter()
{
    DirInfo * subtree = 0;

    if ( _ui->wholeTreeRadioButton->isChecked() )
        subtree = 0;
    else if ( _ui->currentSubtreeRadioButton->isChecked() )
        subtree = 0;

    int mode        = _ui->filterModeComboBox->currentIndex();
    QString pattern = _ui->patternField->text();

    FileSearchFilter filter( subtree,
                             pattern,
                             (SearchFilter::FilterMode) mode );

    filter.setFindFiles( _ui->findFilesRadioButton->isChecked() ||
                         _ui->findBothRadioButton->isChecked()    );

    filter.setFindDirs ( _ui->findDirectoriesRadioButton->isChecked() ||
                         _ui->findBothRadioButton->isChecked()          );

    filter.setCaseSensitive ( _ui->caseSensitiveCheckBox->isChecked() );

    logDebug() << filter << endl;

    return filter;
}


FileSearchFilter FindFilesDialog::askFindFiles( bool    * canceled_ret,
                                                QWidget * parent )
{
    FindFilesDialog dialog( parent );
    int result = dialog.exec();

    FileSearchFilter filter( 0, "" );
    bool canceled = (result == QDialog::Rejected );

    if ( ! canceled )
        filter = dialog.fileSearchFilter();

    if ( canceled_ret )
        *canceled_ret = canceled;

    return filter;
}


void FindFilesDialog::readSettings()
{
    // logDebug() << endl;

    QDirStat::Settings settings;

    settings.beginGroup( "FindFilesDialog" );

    _ui->patternField->setText( lastPattern );

    _ui->filterModeComboBox->setCurrentText     ( settings.value( "filterMode",     "Auto" ).toString() );
    _ui->caseSensitiveCheckBox->setChecked      ( settings.value( "caseSensitive",  false  ).toBool()   );

    _ui->findFilesRadioButton->setChecked       ( settings.value( "findFiles",      false  ).toBool() );
    _ui->findDirectoriesRadioButton->setChecked ( settings.value( "findDirs",       false  ).toBool() );
    _ui->findBothRadioButton->setChecked        ( settings.value( "findBoth",       true   ).toBool() );

    _ui->wholeTreeRadioButton->setChecked       ( settings.value( "wholeTree",      true   ).toBool() );
    _ui->currentSubtreeRadioButton->setChecked  ( settings.value( "currentSubtree", false  ).toBool() );
    _ui->currentSubtreePathLabel->setText( "" );

    settings.endGroup();


    readWindowSettings( this, "FindFilesDialog" );
}


void FindFilesDialog::writeSettings()
{
    // logDebug() << endl;

    QDirStat::Settings settings;

    settings.beginGroup( "FindFilesDialog" );

    // not writing _ui->patternField->text() to the settings, i.e. to file
    lastPattern = _ui->patternField->text();

    settings.setValue( "filterMode",     _ui->filterModeComboBox->currentText()       );
    settings.setValue( "caseSensitive",  _ui->caseSensitiveCheckBox->isChecked()      );

    settings.setValue( "findFiles",      _ui->findFilesRadioButton->isChecked()       );
    settings.setValue( "findDirs",       _ui->findDirectoriesRadioButton->isChecked() );
    settings.setValue( "findBoth",       _ui->findBothRadioButton->isChecked()        );

    settings.setValue( "wholeTree",      _ui->wholeTreeRadioButton->isChecked()       );
    settings.setValue( "currentSubtree", _ui->currentSubtreeRadioButton->isChecked()  );

    // not writing _ui->currentSubtreePathLabel->text()

    settings.endGroup();

    writeWindowSettings( this, "FindFilesDialog" );
}
