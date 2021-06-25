/*
 *   File name: ShowUnpkgFilesDialog.cpp
 *   Summary:	QDirStat "show unpackaged files" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QPushButton>
#include <QMessageBox>

#include "Qt4Compat.h"

#include "ShowUnpkgFilesDialog.h"
#include "ExistingDirCompleter.h"
#include "ExistingDirValidator.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


ShowUnpkgFilesDialog::ShowUnpkgFilesDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::ShowUnpkgFilesDialog )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this );

    _okButton = _ui->buttonBox->button( QDialogButtonBox::Ok );
    CHECK_PTR( _okButton );

    QCompleter * completer = new ExistingDirCompleter( this );
    CHECK_NEW( completer );

    _ui->startingDirComboBox->setCompleter( completer );

    QValidator * validator = new ExistingDirValidator( this );
    CHECK_NEW( validator );

    _ui->startingDirComboBox->setValidator( validator );
    qEnableClearButton( _ui->startingDirComboBox );

    connect( validator, SIGNAL( isOk	  ( bool ) ),
	     _okButton, SLOT  ( setEnabled( bool ) ) );

    connect( this, SIGNAL( accepted()	   ),
	     this, SLOT	 ( writeSettings() ) );

    QPushButton * resetButton = _ui->buttonBox->button( QDialogButtonBox::RestoreDefaults );
    CHECK_PTR( resetButton );

    connect( resetButton, SIGNAL( clicked()	    ),
	     this,	  SLOT	( restoreDefaults() ) );

    readSettings();
}


ShowUnpkgFilesDialog::~ShowUnpkgFilesDialog()
{
    delete _ui;
}


QString ShowUnpkgFilesDialog::startingDir() const
{
    if ( result() == QDialog::Accepted )
	return _ui->startingDirComboBox->currentText();
    else
	return "";
}


QStringList ShowUnpkgFilesDialog::excludeDirs() const
{
    return cleanedLines( _ui->excludeDirsTextEdit );
}


QStringList ShowUnpkgFilesDialog::ignorePatterns() const
{
    return cleanedLines( _ui->ignorePatternsTextEdit );
}


QStringList ShowUnpkgFilesDialog::cleanedLines( QPlainTextEdit *widget ) const
{
    QString	text  = widget->toPlainText();
    QStringList lines = text.split( '\n', QString::SkipEmptyParts );
    QStringList result;

    foreach ( QString line, lines )
    {
	line = line.trimmed();

	if ( ! line.isEmpty() )
	    result << line;
    }

    return result;
}


void ShowUnpkgFilesDialog::restoreDefaults()
{
    QString msg = tr( "Really reset all values to default?" );
    int ret = QMessageBox::question( qApp->activeWindow(),
				     tr( "Please Confirm" ), // title
				     msg,		     // text
				     QMessageBox::Yes | QMessageBox::No );

    if ( ret == QMessageBox::Yes )
        setValues( UnpkgSettings( UnpkgSettings::DefaultValues ) );
}


UnpkgSettings ShowUnpkgFilesDialog::values() const
{
    UnpkgSettings settings( UnpkgSettings::Empty );

    settings.startingDir    = startingDir();
    settings.excludeDirs    = excludeDirs();
    settings.ignorePatterns = ignorePatterns();
    // settings.dump();

    return settings;
}


void ShowUnpkgFilesDialog::setValues( const UnpkgSettings & settings )
{
    // settings.dump();
    qSetComboBoxText( _ui->startingDirComboBox, settings.startingDir );
    _ui->excludeDirsTextEdit->setPlainText( settings.excludeDirs.join( "\n" ) );
    _ui->ignorePatternsTextEdit->setPlainText( settings.ignorePatterns.join( "\n" ) );
}


void ShowUnpkgFilesDialog::readSettings()
{
    // logDebug() << endl;

    setValues( UnpkgSettings( UnpkgSettings::ReadFromConfig ) );;
    readWindowSettings( this, "ShowUnkpgFilesDialog" );
}


void ShowUnpkgFilesDialog::writeSettings()
{
    // logDebug() << endl;

    UnpkgSettings settings = values();
    settings.write();

    writeWindowSettings( this, "ShowUnkpgFilesDialog" );
}
