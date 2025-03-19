/*
 *   File name: FindFilesDialog.cpp
 *   Summary:	QDirStat "Find Files" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FindFilesDialog.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "QDirStatApp.h"
#include "DirTree.h"
#include "DirInfo.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;

// Values that should be persistent for one program run,
// but not written to the settings / config file

static QString lastPattern;
static QString lastPath;


FindFilesDialog::FindFilesDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FindFilesDialog )
{
    // logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );

    if ( lastPath.isEmpty() && app()->root() )
        lastPath = app()->root()->url();

    qEnableClearButton( _ui->patternField );
    _ui->patternField->setFocus();

    connect( this, SIGNAL( accepted()	   ),
	     this, SLOT	 ( saveValues() ) );

    loadValues();
}


FindFilesDialog::~FindFilesDialog()
{
    delete _ui;
}


FileSearchFilter FindFilesDialog::fileSearchFilter()
{
    FileInfo * subtree = 0;

    if ( _ui->wholeTreeRadioButton->isChecked() )
    {
        subtree = app()->root();
    }
    else if ( _ui->currentSubtreeRadioButton->isChecked() )
    {
        subtree = currentSubtree();
    }

    int mode        = _ui->filterModeComboBox->currentIndex();
    QString pattern = _ui->patternField->text();

    FileSearchFilter filter( subtree ? subtree->toDirInfo() : 0,
                             pattern,
                             (SearchFilter::FilterMode) mode );

    filter.setFindFiles( _ui->findFilesRadioButton->isChecked() ||
                         _ui->findBothRadioButton->isChecked()    );

    filter.setFindDirs( _ui->findDirectoriesRadioButton->isChecked() ||
                        _ui->findBothRadioButton->isChecked()          );

    filter.setFindSymLinks( _ui->findSymLinksCheckBox->isChecked() );

    filter.setFindPkg( filter.findDirs() );
    filter.setCaseSensitive ( _ui->caseSensitiveCheckBox->isChecked() );

    logDebug() << filter << ENDL;

    return filter;
}


DirInfo * FindFilesDialog::currentSubtree()
{
    FileInfo * subtree = app()->selectedDirInfo();

    if ( subtree )
        lastPath = subtree->url();
    else
    {
        subtree = app()->dirTree()->locate( lastPath,
                                            true     ); // findPseudoDirs

        if ( ! subtree ) // lastPath outside of this tree?
        {
            if ( app()->root() )
            {
                subtree  = app()->root();
                lastPath = subtree->url();
            }
            else
            {
                lastPath.clear();
            }
        }
    }

    return subtree ? subtree->toDirInfo() : 0;
}


FileSearchFilter FindFilesDialog::askFindFiles( bool    * canceled_ret,
                                                QWidget * parent )
{
    FindFilesDialog dialog( parent );
    int result = dialog.exec();

    FileSearchFilter filter( 0, "" );
    bool canceled = ( result == QDialog::Rejected );

    if ( ! canceled )
        filter = dialog.fileSearchFilter();

    if ( filter.pattern().isEmpty() )
        canceled = true;

    if ( canceled_ret )
        *canceled_ret = canceled;

    return filter;
}


void FindFilesDialog::loadValues()
{
    readSettings();

    //
    // Restore those values from static variables
    //

    _ui->patternField->setText( lastPattern );

    QString    path = lastPath;
    FileInfo * sel  = currentSubtree();

    if ( sel )
    {
        path     = sel->url();
        lastPath = path;
    }

    _ui->currentSubtreePathLabel->setText( path );
    _ui->currentSubtreeRadioButton->setEnabled( ! path.isEmpty() );
}


void FindFilesDialog::saveValues()
{
    writeSettings();

    //
    // Values that should not be written to the settings / the config file:
    // Save to static variables just for the duration of this program run as
    // the dialog is created, destroyed and created every time the user starts
    // the "Find Files" action (Ctrl-F).
    //

    lastPattern = _ui->patternField->text();
    lastPath    = _ui->currentSubtreePathLabel->text();
}


void FindFilesDialog::readSettings()
{
    // logDebug() << endl;

    QDirStat::Settings settings;

    settings.beginGroup( "FindFilesDialog" );

    _ui->filterModeComboBox->setCurrentText     ( settings.value( "filterMode",     "Auto" ).toString() );
    _ui->caseSensitiveCheckBox->setChecked      ( settings.value( "caseSensitive",  false  ).toBool()   );

    _ui->findFilesRadioButton->setChecked       ( settings.value( "findFiles",      false  ).toBool() );
    _ui->findDirectoriesRadioButton->setChecked ( settings.value( "findDirs",       false  ).toBool() );
    _ui->findBothRadioButton->setChecked        ( settings.value( "findBoth",       true   ).toBool() );
    _ui->findSymLinksCheckBox->setChecked       ( settings.value( "findSymLinks",   true   ).toBool() );

    _ui->wholeTreeRadioButton->setChecked       ( settings.value( "wholeTree",      true   ).toBool() );
    _ui->currentSubtreeRadioButton->setChecked  ( settings.value( "currentSubtree", false  ).toBool() );

    settings.endGroup();

    // Intentionally NOT reading from the settings / the config file:
    //
    // _ui->patternField->setText(...);
    // _ui->currentSubtreePathLabel->setText(...);

    readWindowSettings( this, "FindFilesDialog" );
}


void FindFilesDialog::writeSettings()
{
    // logDebug() << endl;

    QDirStat::Settings settings;

    settings.beginGroup( "FindFilesDialog" );

    settings.setValue( "filterMode",     _ui->filterModeComboBox->currentText()       );
    settings.setValue( "caseSensitive",  _ui->caseSensitiveCheckBox->isChecked()      );

    settings.setValue( "findFiles",      _ui->findFilesRadioButton->isChecked()       );
    settings.setValue( "findDirs",       _ui->findDirectoriesRadioButton->isChecked() );
    settings.setValue( "findBoth",       _ui->findBothRadioButton->isChecked()        );
    settings.setValue( "findSymLinks",   _ui->findSymLinksCheckBox->isChecked()       );

    settings.setValue( "wholeTree",      _ui->wholeTreeRadioButton->isChecked()       );
    settings.setValue( "currentSubtree", _ui->currentSubtreeRadioButton->isChecked()  );

    settings.endGroup();

    // Intentionally NOT writing to the settings / the config file:
    //
    // _ui->patternField->text();
    // _ui->currentSubtreePathLabel->text();

    writeWindowSettings( this, "FindFilesDialog" );
}
