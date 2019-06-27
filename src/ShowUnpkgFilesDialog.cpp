/*
 *   File name: ShowUnpkgFilesDialog.cpp
 *   Summary:	QDirStat "show unpackaged files" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QPushButton>

#include "ShowUnpkgFilesDialog.h"
#include "ExistingDirCompleter.h"
#include "ExistingDirValidator.h"
#include "Settings.h"
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

    connect( validator, SIGNAL( isOk      ( bool ) ),
             _okButton, SLOT  ( setEnabled( bool ) ) );

    connect( this, SIGNAL( accepted()      ),
             this, SLOT  ( writeSettings() ) );

    readSettings();
}


ShowUnpkgFilesDialog::~ShowUnpkgFilesDialog()
{

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
    QString     text  = _ui->excludeDirsTextEdit->toPlainText();
    QStringList lines = text.split( '\n', QString::SkipEmptyParts );
    QStringList dirs;

    foreach ( QString line, lines )
    {
        line = line.trimmed();

        if ( ! line.isEmpty() )
            dirs << line;
    }

    return dirs;
}


QStringList ShowUnpkgFilesDialog::defaultExcludeDirs()
{
    return QStringList()
        << "/home"
        << "/root"
        << "/tmp"
        << "/var"
        << "/usr/lib/sysimage/rpm";
}


void ShowUnpkgFilesDialog::readSettings()
{
    logDebug() << endl;

    QDirStat::Settings settings;
    settings.beginGroup( "ShowUnkpgFilesDialog" );

    QStringList excludeDirs =
        settings.value( "ExcludeDirs", defaultExcludeDirs() ).toStringList();
    
    settings.endGroup();
    
    _ui->excludeDirsTextEdit->setPlainText( excludeDirs.join( "\n" ) );
}


void ShowUnpkgFilesDialog::writeSettings()
{
    logDebug() << endl;

    QDirStat::Settings settings;
    
    settings.beginGroup( "ShowUnkpgFilesDialog" );
    settings.setValue( "ExcludeDirs", excludeDirs() );
    settings.endGroup();
}
