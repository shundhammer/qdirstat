/*
 *   File name: CleanupConfigPage.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "CleanupConfigPage.h"
#include "CleanupCollection.h"
#include "Cleanup.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"


#define DEFAULT_OUTPUT_WINDOW_SHOW_TIMEOUT	500

// This is a mess that became necessary because Qt's moc cannot handle template
// classes. Yes, this is ugly.
#define CLEANUP_CAST(VOID_PTR) (static_cast<Cleanup *>(VOID_PTR))

using namespace QDirStat;


CleanupConfigPage::CleanupConfigPage( QWidget * parent ):
    ListEditor( parent ),
    _ui( new Ui::CleanupConfigPage ),
    _cleanupCollection( 0 )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );
    setListWidget( _ui->listWidget );

    setMoveUpButton	 ( _ui->moveUpButton	   );
    setMoveDownButton	 ( _ui->moveDownButton	   );
    setMoveToTopButton	 ( _ui->moveToTopButton	   );
    setMoveToBottomButton( _ui->moveToBottomButton );
    setAddButton	 ( _ui->addButton	   );
    setRemoveButton	 ( _ui->removeButton	   );

    connect( _ui->titleLineEdit, SIGNAL( textChanged ( QString ) ),
	     this,		 SLOT  ( titleChanged( QString ) ) );
}


CleanupConfigPage::~CleanupConfigPage()
{
    // logDebug() << "CleanupConfigPage destructor" << endl;
    delete _ui;
}


void CleanupConfigPage::setCleanupCollection( CleanupCollection * collection )
{
    _cleanupCollection = collection;
}


void CleanupConfigPage::setup()
{
    fillListWidget();
    _ui->toolBox->setCurrentIndex( 0 );
    updateActions();
}


void CleanupConfigPage::applyChanges()
{
    // logDebug() << endl;

    save( value( listWidget()->currentItem() ) );
    _cleanupCollection->writeSettings();
}


void CleanupConfigPage::discardChanges()
{
    // logDebug() << endl;

    listWidget()->clear();
    _cleanupCollection->clear();
    _cleanupCollection->addStdCleanups();
    _cleanupCollection->readSettings();
}


void CleanupConfigPage::fillListWidget()
{
    CHECK_PTR( _cleanupCollection );
    listWidget()->clear();

    foreach ( Cleanup * cleanup, _cleanupCollection->cleanupList() )
    {
	QListWidgetItem * item = new ListEditorItem( cleanup->cleanTitle(), cleanup );
	CHECK_NEW( item );
	listWidget()->addItem( item );
    }

    QListWidgetItem * firstItem = listWidget()->item(0);

    if ( firstItem )
	listWidget()->setCurrentItem( firstItem );
}


void CleanupConfigPage::titleChanged( const QString & newTitle )
{
    QListWidgetItem * currentItem = listWidget()->currentItem();

    if ( currentItem )
    {
	Cleanup * cleanup = CLEANUP_CAST( value( currentItem ) );
	cleanup->setTitle( newTitle );
	currentItem->setText( cleanup->cleanTitle() );
    }
}


void CleanupConfigPage::save( void * value )
{
    Cleanup * cleanup = CLEANUP_CAST( value );
    // logDebug() << cleanup << endl;

    if ( ! cleanup || updatesLocked() )
	return;

    cleanup->setActive ( _ui->activeGroupBox->isChecked() );
    cleanup->setTitle  ( _ui->titleLineEdit->text()	  );
    cleanup->setCommand( _ui->commandLineEdit->text()	  );

    if ( _ui->shellComboBox->currentText().startsWith( "$SHELL" ) )
	cleanup->setShell( "" );
    else
	cleanup->setShell( _ui->shellComboBox->currentText() );

    cleanup->setRecurse( _ui->recurseCheckBox->isChecked() );

    cleanup->setAskForConfirmation( _ui->askForConfirmationCheckBox->isChecked() );
    cleanup->setWorksForDir	  ( _ui->worksForDirCheckBox->isChecked()	 );
    cleanup->setWorksForFile	  ( _ui->worksForFilesCheckBox->isChecked()	 );
    cleanup->setWorksForDotEntry  ( _ui->worksForDotEntriesCheckBox->isChecked() );

    int policy = _ui->outputWindowPolicyComboBox->currentIndex();
    cleanup->setOutputWindowPolicy( static_cast<Cleanup::OutputWindowPolicy>( policy ) );

    int timeout = qRound( _ui->outputWindowTimeoutSpinBox->value() * 1000.0 );

    if ( timeout == DEFAULT_OUTPUT_WINDOW_SHOW_TIMEOUT ) // FIXME: Get this from OutputWindow
	timeout = 0;

    cleanup->setOutputWindowTimeout( timeout );

    cleanup->setOutputWindowAutoClose( _ui->outputWindowAutoCloseCheckBox->isChecked() );

    policy = _ui->refreshPolicyComboBox->currentIndex();
    cleanup->setRefreshPolicy( static_cast<Cleanup::RefreshPolicy>( policy ) );
}


void CleanupConfigPage::load( void * value )
{
    Cleanup * cleanup = CLEANUP_CAST( value );
    // logDebug() << cleanup << endl;

    if ( ! cleanup || updatesLocked() )
	return;

    _ui->activeGroupBox->setChecked( cleanup->active() );
    _ui->titleLineEdit->setText( cleanup->title() );
    _ui->commandLineEdit->setText( cleanup->command() );

    if ( cleanup->shell().isEmpty() )
	_ui->shellComboBox->setCurrentIndex( 0 );
    else
	_ui->shellComboBox->setEditText( cleanup->shell() );

    _ui->recurseCheckBox->setChecked	       ( cleanup->recurse()	       );
    _ui->askForConfirmationCheckBox->setChecked( cleanup->askForConfirmation() );
    _ui->worksForDirCheckBox->setChecked       ( cleanup->worksForDir()	       );
    _ui->worksForFilesCheckBox->setChecked     ( cleanup->worksForFile()       );
    _ui->worksForDotEntriesCheckBox->setChecked( cleanup->worksForDotEntry()   );

    _ui->outputWindowPolicyComboBox->setCurrentIndex( cleanup->outputWindowPolicy() );

    int timeout = cleanup->outputWindowTimeout();

    if ( timeout == 0 )
	timeout = DEFAULT_OUTPUT_WINDOW_SHOW_TIMEOUT; // FIXME: Get this from OutputWindow

    _ui->outputWindowTimeoutSpinBox->setValue( timeout / 1000.0 );
    _ui->outputWindowAutoCloseCheckBox->setChecked( cleanup->outputWindowAutoClose() );

    _ui->refreshPolicyComboBox->setCurrentIndex( cleanup->refreshPolicy() );
}


void * CleanupConfigPage::createValue()
{
    Cleanup * cleanup = new Cleanup();
    CHECK_NEW( cleanup );
    _cleanupCollection->add( cleanup );

    return cleanup;
}


void CleanupConfigPage::removeValue( void * value )
{
    Cleanup * cleanup = CLEANUP_CAST( value );
    CHECK_PTR( cleanup );

    _cleanupCollection->remove( cleanup );
}


QString CleanupConfigPage::valueText( void * value )
{
    Cleanup * cleanup = CLEANUP_CAST( value );
    CHECK_PTR( cleanup );

    return cleanup->cleanTitle();
}


QString CleanupConfigPage::deleteConfirmationMessage( void * value )
{
    Cleanup * cleanup = CLEANUP_CAST( value );
    return tr( "Really delete cleanup \"%1\"?" ).arg( cleanup->cleanTitle() );
}


void CleanupConfigPage::moveValue( void * value, const char * operation )
{
    Cleanup * cleanup = CLEANUP_CAST( value );

    QMetaObject::invokeMethod( _cleanupCollection,
			       operation,
			       Qt::DirectConnection,
			       Q_ARG( Cleanup *, cleanup ) );
}
