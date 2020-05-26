/*
 *   File name: OpenDirDialog.cpp
 *   Summary:	QDirStat "open directory" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QDir>
#include <QFileDialog>

#include "OpenDirDialog.h"
#include "MountPoints.h"
#include "ExistingDirCompleter.h"
#include "ExistingDirValidator.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Qt4Compat.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


bool OpenDirDialog::_crossFilesystems = false;


OpenDirDialog::OpenDirDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::OpenDirDialog )
{
    // logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );

    // Populate pathComboBox

    QString cwd = QDir::currentPath();
    const MountPoint * currentFS = MountPoints::findNearestMountPoint( cwd );

    _ui->pathComboBox->clear();
    _ui->pathComboBox->addItem( "/" );

    if ( currentFS )
        _ui->pathComboBox->addItem( currentFS->path() );

    _ui->pathComboBox->addItem( cwd );
    setPath( cwd );


    _ui->crossFilesystemsCheckBox->setChecked( _crossFilesystems );

    _okButton = _ui->buttonBox->button( QDialogButtonBox::Ok );
    CHECK_PTR( _okButton );

    QCompleter * completer = new ExistingDirCompleter( this );
    CHECK_NEW( completer );

    _ui->pathComboBox->setCompleter( completer );

    QValidator * validator = new ExistingDirValidator( this );
    CHECK_NEW( validator );

    _ui->pathComboBox->setValidator( validator );

    connect( validator,		SIGNAL( isOk	  ( bool ) ),
	     _okButton,		SLOT  ( setEnabled( bool ) ) );

    connect( this,		SIGNAL( accepted()	),
	     this,		SLOT  ( writeSettings() ) );

    connect( _ui->browseButton, SIGNAL( clicked() ),
	     this,		SLOT  ( browseDirs() ) );

    connect( _ui->pathSelector, SIGNAL( pathSelected( QString ) ),
             this,              SLOT  ( setPath     ( QString ) ) );

    readSettings();
    _ui->pathComboBox->setFocus();
}


OpenDirDialog::~OpenDirDialog()
{

}


QString OpenDirDialog::selectedPath() const
{
    return _ui->pathComboBox->currentText();
}


bool OpenDirDialog::crossFilesystems() const
{
    return _ui->crossFilesystemsCheckBox->isChecked();
}


void OpenDirDialog::setPath( const QString & path )
{
    qSetComboBoxText( _ui->pathComboBox, path );
}


void OpenDirDialog::browseDirs()
{
    QString path = QFileDialog::getExistingDirectory( this, // parent
						      tr("Select directory") );

    if ( ! path.isEmpty() )
        setPath( path );
}


void OpenDirDialog::readSettings()
{
    Settings settings;
    // logDebug() << endl;

    // Initialize the static _crossFilesystems flag from the corresponding
    // setting from the config dialog

    settings.beginGroup( "DirectoryTree" );
    _crossFilesystems = settings.value( "CrossFileSystems", false ).toBool();
    settings.endGroup();

    readWindowSettings( this, "OpenDirDialog" );

    settings.beginGroup( "OpenDirDialog" );
    QByteArray mainSplitterState = settings.value( "MainSplitter" , QByteArray() ).toByteArray();
    settings.endGroup();

    if ( ! mainSplitterState.isNull() )
	_ui->mainSplitter->restoreState( mainSplitterState );
}


void OpenDirDialog::writeSettings()
{
    Settings settings;
    // logDebug() << endl;

    // Do NOT write _crossFilesystems back to the settings here; this is done
    // from the config dialog. The value in this dialog is just temporary for
    // the current program run.

    writeWindowSettings( this, "OpenDirDialog" );

    settings.beginGroup( "OpenDirDialog" );
    settings.setValue( "MainSplitter", _ui->mainSplitter->saveState()  );
    settings.endGroup();
}


QString OpenDirDialog::askOpenDir( bool *    crossFilesystems_ret,
				   QWidget * parent )
{
    OpenDirDialog dialog( parent );
    dialog.pathSelector()->addHomeDir();
    dialog.pathSelector()->addMountPoints( MountPoints::normalMountPoints() );

    if ( dialog.exec() == QDialog::Rejected )
	return QString();

    _crossFilesystems = dialog.crossFilesystems();

    if ( crossFilesystems_ret )
	*crossFilesystems_ret = _crossFilesystems;

    return dialog.selectedPath();
}

