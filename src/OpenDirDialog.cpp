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
#include <QTimer>

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


// The ExistingDirCompleter is useful if there is are no other navigation tools
// that compete for widget focus and user attention, but here it's only
// confusing: The dirTreeView, the pathCompleter and the pathComboBox fire off
// a cascade of signals for every tiny change, and that makes the completer
// behave very erratic.

#define USE_COMPLETER           0

#define VERBOSE_SELECTION       0


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
    MountPoints::reload();

    initPathComboBox();
    initDirTree();

    _ui->crossFilesystemsCheckBox->setChecked( _crossFilesystems );

    _okButton = _ui->buttonBox->button( QDialogButtonBox::Ok );
    CHECK_PTR( _okButton );

    initConnections();
    readSettings();

    _ui->pathComboBox->setFocus();
    QTimer::singleShot( 200, this, SLOT( initialSelection() ) );
}


OpenDirDialog::~OpenDirDialog()
{
    delete _ui;
}


void OpenDirDialog::initPathComboBox()
{
    qEnableClearButton( _ui->pathComboBox );

#if USE_COMPLETER
    QCompleter * completer = new ExistingDirCompleter( this );
    CHECK_NEW( completer );

    _ui->pathComboBox->setCompleter( completer );
#endif

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

    connect( _ui->pathSelector, SIGNAL( pathSelected     ( QString ) ),
             this,              SLOT  ( setPathAndExpand ( QString ) ) );

    connect( _ui->pathSelector, SIGNAL( pathDoubleClicked( QString ) ),
             this,              SLOT  ( setPathAndAccept ( QString ) ) );

    QItemSelectionModel * selModel = _ui->dirTreeView->selectionModel();

    connect( selModel,  SIGNAL( currentChanged( QModelIndex, QModelIndex ) ),
             this,      SLOT  ( treeSelection ( QModelIndex, QModelIndex ) ) );

    connect( _ui->upButton, SIGNAL( clicked() ),
             this,          SLOT  ( goUp()    ) );
}


void OpenDirDialog::initialSelection()
{
    QString path = QDir::currentPath();

#if VERBOSE_SELECTION
    logDebug() << "Selecting " << path << endl;
#endif

    setPath( path );
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
    if ( _settingPath || path == _lastPath )
        return;

    // This flag is needed to avoid signal cascades from all the different
    // widgets in the dialog: The pathSelector (the "places"), the
    // pathComboBox, the dirTreeView. If they change their current path, they
    // will also fire off signals to notify all their subscribers which will
    // then change their current path and fire off their signals. We need to
    // break that cycle; that's what this flag is for.
    //
    // Unfortunately just blocking the signals sometimes has bad side effects
    // (see below).
    _settingPath = true;

#if VERBOSE_SELECTION
    logDebug() << "Selecting " << path << endl;
#endif

    SignalBlocker sigBlockerPathSelector( _ui->pathSelector );
    // Can't block signals of the dirTreeView's selection model:
    // This would mean that the dirTreeView also isn't notified,
    // so any change would not become visible in the tree.

    populatePathComboBox( path );
    qSetComboBoxText( _ui->pathComboBox, path );
    _ui->pathSelector->selectParentMountPoint( path );
    QModelIndex index = _filesystemModel->index( path );
    _ui->dirTreeView->setCurrentIndex( index );
    _ui->dirTreeView->scrollTo( index );

    _lastPath = path;
    _settingPath = false;
}


void OpenDirDialog::setPathAndExpand( const QString & path )
{
    setPath( path );

    SignalBlocker sigBlockerPathSelector( _ui->pathSelector );
    QModelIndex index = _filesystemModel->index( path );
    _ui->dirTreeView->collapseAll();
    _ui->dirTreeView->setExpanded( index, true );
    _ui->dirTreeView->scrollTo( index, QAbstractItemView::PositionAtTop );
}


void OpenDirDialog::setPathAndAccept( const QString & path )
{
    setPath( path );
    accept();
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

    if ( path != _lastPath )
    {
#if VERBOSE_SELECTION
        logDebug() << "New path:" << path << endl;
#endif
        setPath( path );
    }
}


void OpenDirDialog::treeSelection( const QModelIndex & newCurrentItem,
                                   const QModelIndex & oldCurrentItem )
{
    Q_UNUSED( oldCurrentItem );
    QString path = _filesystemModel->filePath( newCurrentItem );

    if ( path != _lastPath )
    {
#if VERBOSE_SELECTION
        logDebug() << "Selecting " << path << endl;
#endif
        setPath( path );
    }
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

    if ( path != _lastPath )
    {
#if VERBOSE_SELECTION
        logDebug() << "Navigating up to " << path << endl;
#endif
        setPath( path );
    }
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
    logDebug() << "Waiting for user selection" << endl;

    if ( dialog.exec() == QDialog::Rejected )
    {
        logInfo() << "[Cancel]" << endl;
	return QString();
    }

    _crossFilesystems = dialog.crossFilesystems();

    if ( crossFilesystems_ret )
	*crossFilesystems_ret = _crossFilesystems;

    QString path = dialog.selectedPath();
    logInfo() << "User selected path " << path << endl;

    return path;
}

