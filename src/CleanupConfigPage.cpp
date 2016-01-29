/*
 *   File name: CleanupConfigPage.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMessageBox>

#include "CleanupConfigPage.h"
#include "CleanupCollection.h"
#include "Cleanup.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;

#define DEFAULT_OUTPUT_WINDOW_SHOW_TIMEOUT	500


#define CONNECT_BUTTON(BUTTON, RCVR_SLOT) \
    connect( (BUTTON), SIGNAL( clicked() ), this, SLOT( RCVR_SLOT ) )


CleanupConfigPage::CleanupConfigPage( QWidget * parent ):
    QWidget( parent ),
    _ui( new Ui::CleanupConfigPage ),
    _firstRow( 0 ),
    _updatesLocked( false )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );
    _listWidget = _ui->listWidget; // shortcut for a frequently needed widget

    connect( _listWidget, SIGNAL( currentItemChanged( QListWidgetItem *,
						      QListWidgetItem *	 ) ),
	     this,	  SLOT	( currentItemChanged( QListWidgetItem *,
						      QListWidgetItem *	 ) ) );

    connect( _ui->titleLineEdit, SIGNAL( textChanged ( QString ) ),
	     this,		 SLOT  ( titleChanged( QString ) ) );

    CONNECT_BUTTON( _ui->moveUpButton,	     moveUp()	     );
    CONNECT_BUTTON( _ui->moveDownButton,     moveDown()	     );
    CONNECT_BUTTON( _ui->moveToTopButton,    moveToTop()     );
    CONNECT_BUTTON( _ui->moveToBottomButton, moveToBottom()  );
    CONNECT_BUTTON( _ui->addButton,	     add()           );
    CONNECT_BUTTON( _ui->removeButton,	     remove()        );
}


CleanupConfigPage::~CleanupConfigPage()
{
    // logDebug() << "CleanupConfigPage destructor" << endl;
}


void CleanupConfigPage::setup()
{
    fillListWidget();
    _ui->toolBox->setCurrentIndex( 0 );
    updateActions();
}


void CleanupConfigPage::applyChanges()
{
    logDebug() << endl;

    save( cleanup( _listWidget->currentItem() ) );
    _cleanupCollection->writeSettings();
}


void CleanupConfigPage::discardChanges()
{
    logDebug() << endl;

    _listWidget->clear();
    _cleanupCollection->clear();
    _cleanupCollection->addStdCleanups();
    _cleanupCollection->readSettings();
}


void CleanupConfigPage::fillListWidget()
{
    CHECK_PTR( _cleanupCollection );

    CleanupListItem * firstItem = 0;
    _listWidget->clear();

    foreach ( Cleanup * cleanup, _cleanupCollection->cleanupList() )
    {
	CleanupListItem * item = new CleanupListItem( cleanup );
	CHECK_NEW( item );
	_listWidget->addItem( item );

	if ( ! firstItem )
	    firstItem = item;
    }

    _listWidget->setCurrentItem( firstItem );
}


Cleanup * CleanupConfigPage::cleanup( QListWidgetItem * item )
{
    if ( ! item )
	return 0;

    CleanupListItem * configListItem = dynamic_cast<CleanupListItem *>( item );
    CHECK_DYNAMIC_CAST( configListItem, "CleanupListItem *" );

    return configListItem->cleanup();
}


void CleanupConfigPage::currentItemChanged( QListWidgetItem * currentItem,
                                            QListWidgetItem * previousItem )
{
    save( cleanup( previousItem ) );
    load( cleanup( currentItem  ) );
    updateActions();
}


void CleanupConfigPage::titleChanged( const QString & newTitle )
{
    QListWidgetItem * currentItem = _listWidget->currentItem();

    if ( currentItem )
    {
	Cleanup * cleanup = this->cleanup( currentItem );
	cleanup->setTitle( newTitle );
	currentItem->setText( cleanup->cleanTitle() );
    }
}


void CleanupConfigPage::updateActions()
{
    int currentRow = _listWidget->currentRow();
    int lastRow    = _listWidget->count() - 1;

    _ui->moveToTopButton->setEnabled   ( currentRow > _firstRow );
    _ui->moveUpButton->setEnabled      ( currentRow > _firstRow );
    _ui->moveDownButton->setEnabled    ( currentRow < lastRow );
    _ui->moveToBottomButton->setEnabled( currentRow < lastRow );
}


void CleanupConfigPage::save( Cleanup * cleanup )
{
    // logDebug() << cleanup << endl;

    if ( ! cleanup || _updatesLocked )
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


void CleanupConfigPage::load( Cleanup * cleanup )
{
    // logDebug() << cleanup << endl;

    if ( ! cleanup || _updatesLocked )
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


void CleanupConfigPage::moveUp()
{
    QListWidgetItem * currentItem = _listWidget->currentItem();
    int currentRow		  = _listWidget->currentRow();

    if ( ! currentItem )
	return;

    if ( currentRow > 0 )
    {
	_updatesLocked = true;
	_listWidget->takeItem( currentRow );
	_listWidget->insertItem( currentRow - 1, currentItem );
	_listWidget->setCurrentItem( currentItem );
	_cleanupCollection->moveUp( cleanup( currentItem ) );
	_updatesLocked = false;
    }
}


void CleanupConfigPage::moveDown()
{
    QListWidgetItem * currentItem = _listWidget->currentItem();
    int currentRow		  = _listWidget->currentRow();

    if ( ! currentItem )
	return;

    if ( currentRow < _listWidget->count() - 1 )
    {
	_updatesLocked = true;
	_listWidget->takeItem( currentRow );
	_listWidget->insertItem( currentRow + 1, currentItem );
	_listWidget->setCurrentItem( currentItem );
	_cleanupCollection->moveDown( cleanup( currentItem ) );
	_updatesLocked = false;
    }
}


void CleanupConfigPage::moveToTop()
{
    QListWidgetItem * currentItem = _listWidget->currentItem();
    int currentRow		  = _listWidget->currentRow();

    if ( ! currentItem )
	return;

    if ( currentRow > 0 )
    {
	_updatesLocked = true;
	_listWidget->takeItem( currentRow );
	_listWidget->insertItem( 0, currentItem );
	_listWidget->setCurrentItem( currentItem );
	_cleanupCollection->moveToTop( cleanup( currentItem ) );
	_updatesLocked = false;
    }
}


void CleanupConfigPage::moveToBottom()
{
    QListWidgetItem * currentItem = _listWidget->currentItem();
    int currentRow		  = _listWidget->currentRow();

    if ( ! currentItem )
	return;

    if ( currentRow < _listWidget->count() - 1 )
    {
	_updatesLocked = true;
	_listWidget->takeItem( currentRow );
	_listWidget->addItem( currentItem );
	_listWidget->setCurrentItem( currentItem );
	_cleanupCollection->moveToBottom( cleanup( currentItem ) );
	_updatesLocked = false;
    }
}


void CleanupConfigPage::add()
{
    Cleanup * cleanup = new Cleanup();
    CHECK_NEW( cleanup );

    CleanupListItem * item = new CleanupListItem( cleanup );
    CHECK_NEW( item );

    _cleanupCollection->add( cleanup );
    _listWidget->addItem( item );
    _listWidget->setCurrentItem( item );
}


void CleanupConfigPage::remove()
{
    QListWidgetItem * currentItem = _listWidget->currentItem();
    int currentRow		  = _listWidget->currentRow();

    if ( ! currentItem )
	return;

    Cleanup * cleanup = this->cleanup( currentItem );

    //
    // Confirmation popup
    //

    QString msg = tr( "Really delete cleanup \"%1\"?" ).arg( cleanup->cleanTitle() );
    int ret = QMessageBox::question( window(),
				     tr( "Please Confirm" ), // title
				     msg );
    if ( ret == QMessageBox::Yes )
    {
	//
	// Delete current cleanup
	//

	_updatesLocked = true;
	_listWidget->takeItem( currentRow );
	_cleanupCollection->remove( cleanup );
	delete currentItem;
	delete cleanup;
	_updatesLocked = false;

	load( this->cleanup( _listWidget->currentItem() ) );
    }
}
