/*
 *   File name: OpenDirDialog.cpp
 *   Summary:	QDirStat "open directory" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QPushButton>
#include <QDir>
#include <QFileSystemModel>

#include "Qt4Compat.h"

#include "OpenDirDialog.h"
#include "MountPoints.h"
#include "ExistingDirCompleter.h"
#include "ExistingDirValidator.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "SignalBlocker.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


bool OpenDirDialog::_crossFilesystems = false;


OpenDirDialog::OpenDirDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::OpenDirDialog ),
    _filesystemModel( new QFileSystemModel( this ) ),
    _settingPath( false )
{
    CHECK_NEW( _ui );
    CHECK_NEW( _filesystemModel );
    _ui->setupUi( this );

    initPathComboBox();
    initDirTree();

    _ui->crossFilesystemsCheckBox->setChecked( _crossFilesystems );

    _okButton = _ui->buttonBox->button( QDialogButtonBox::Ok );
    CHECK_PTR( _okButton );

    initConnections();
    readSettings();

    setPath( QDir::currentPath() );
    _ui->pathComboBox->setFocus();
}


OpenDirDialog::~OpenDirDialog()
{
    // NOP
}


void OpenDirDialog::initPathComboBox()
{
    qEnableClearButton( _ui->pathComboBox );

    QCompleter * completer = new ExistingDirCompleter( this );
    CHECK_NEW( completer );

    _ui->pathComboBox->setCompleter( completer );

    _validator = new ExistingDirValidator( this );
    CHECK_NEW( _validator );

    _ui->pathComboBox->setValidator( _validator );
}


void OpenDirDialog::initDirTree()
{
    _filesystemModel->setRootPath( "/" );
    _filesystemModel->setFilter( QDir::Dirs       |
                                 QDir::NoDot      |
                                 QDir::NoDotDot   |
                                 QDir::NoSymLinks |
                                 QDir::Drives      );

    _ui->dirTreeView->setModel( _filesystemModel );
    _ui->dirTreeView->hideColumn( 3 );  // Date Modified
    _ui->dirTreeView->hideColumn( 2 );  // Type
    _ui->dirTreeView->hideColumn( 1 );  // Size
    _ui->dirTreeView->setHeaderHidden( true );
}


void OpenDirDialog::initConnections()
{
    connect( _validator,	SIGNAL( isOk	  ( bool ) ),
	     _okButton,		SLOT  ( setEnabled( bool ) ) );

    connect( _validator,	SIGNAL( isOk	  ( bool ) ),
	     this,		SLOT  ( pathEdited( bool ) ) );

    connect( this,		SIGNAL( accepted()	),
	     this,		SLOT  ( writeSettings() ) );

    connect( _ui->pathSelector, SIGNAL( pathSelected( QString ) ),
             this,              SLOT  ( setPath     ( QString ) ) );

    QItemSelectionModel * selModel = _ui->dirTreeView->selectionModel();

    connect( selModel,  SIGNAL( currentChanged( QModelIndex, QModelIndex ) ),
             this,      SLOT  ( treeSelection ( QModelIndex, QModelIndex ) ) );

    connect( _ui->upButton, SIGNAL( clicked() ),
             this,          SLOT  ( goUp()    ) );
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
    if ( _settingPath )
        return;

    _settingPath = true;
    // logDebug() << "Selecting " << path << endl;

    SignalBlocker sigBlockerPathSelector( _ui->pathSelector );
    // Can't block signals of the dirTreeView's selection model:
    // This would mean that the dirTreeView also isn't notified,
    // so any change would not become visible in the tree.

    populatePathComboBox( path );
    qSetComboBoxText( _ui->pathComboBox, path );
    _ui->dirTreeView->setCurrentIndex( _filesystemModel->index( path ) );
    _ui->pathSelector->selectParentMountPoint( path );

    _settingPath = false;
}


void OpenDirDialog::pathEdited( bool ok )
{
    if ( _settingPath )
        return;

    if ( ! ok )
        return;

    SignalBlocker sigBlockerComboBox ( _ui->pathComboBox );
    SignalBlocker sigBlockerValidator( _validator );

    QString path = _ui->pathComboBox->currentText();
    // logDebug() << "New path:" << path << endl;
    setPath( path );
}


void OpenDirDialog::treeSelection( const QModelIndex & newCurrentItem,
                                   const QModelIndex & oldCurrentItem )
{
    Q_UNUSED( oldCurrentItem );

    if ( _settingPath )
        return;

    QString path = _filesystemModel->filePath( newCurrentItem );
    // logDebug() << "Selecting " << path << endl;
    setPath( path );
}


void OpenDirDialog::populatePathComboBox( const QString & fullPath )
{
    QStringList pathComponents = fullPath.split( "/", QString::SkipEmptyParts );

    _ui->pathComboBox->clear();
    _ui->pathComboBox->addItem( "/" );
    QString path;

    foreach ( const QString & component, pathComponents )
    {
        path += "/" + component;
        _ui->pathComboBox->addItem( path );
    }
}


void OpenDirDialog::goUp()
{
    QStringList pathComponents = selectedPath().split( "/", QString::SkipEmptyParts );

    if ( ! pathComponents.isEmpty() )
        pathComponents.removeLast();

    QString path = "/" + pathComponents.join( "/" );
    logDebug() << "Navigating up to " << path << endl;

    setPath( path );
}


void OpenDirDialog::readSettings()
{
    Settings settings;
    // logDebug() << endl;

    // Initialize the static _crossFilesystems flag from the corresponding
    // setting from the config dialog

    settings.beginGroup( "DirectoryTree" );
    _crossFilesystems = settings.value( "CrossFilesystems", false ).toBool();
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

