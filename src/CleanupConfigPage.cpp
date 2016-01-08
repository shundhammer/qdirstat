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


using namespace QDirStat;

#define DEFAULT_OUTPUT_WINDOW_SHOW_TIMEOUT      500


CleanupConfigPage::CleanupConfigPage( QWidget * parent ):
    QWidget( parent ),
    _ui( new Ui::CleanupConfigPage )
{
    CHECK_NEW( _ui );

    _ui->setupUi( this );

    connect( _ui->cleanupListWidget, SIGNAL( currentItemChanged( QListWidgetItem *,
                                                                 QListWidgetItem *  ) ),
             this,                   SLOT  ( currentItemChanged( QListWidgetItem *,
                                                                 QListWidgetItem *  ) ) );
}


CleanupConfigPage::~CleanupConfigPage()
{

}


void CleanupConfigPage::setup()
{
    fillCleanupList();
}


void CleanupConfigPage::applyChanges()
{
    logDebug() << endl;
    _cleanupCollection->writeSettings();
}


void CleanupConfigPage::discardChanges()
{
    logDebug() << endl;

    _ui->cleanupListWidget->clear();
    _cleanupCollection->clear();
    _cleanupCollection->addStdCleanups();
    _cleanupCollection->readSettings();
}


void CleanupConfigPage::fillCleanupList()
{
    CHECK_PTR( _cleanupCollection );
    QListWidget * listWidget = _ui->cleanupListWidget;

    CleanupListItem * firstItem = 0;
    listWidget->clear();

    foreach ( Cleanup * cleanup, _cleanupCollection->cleanupList() )
    {
        CleanupListItem * item = new CleanupListItem( cleanup );
        CHECK_NEW( item );
        listWidget->addItem( item );

        if ( ! firstItem )
            firstItem = item;
    }

    listWidget->setCurrentItem( firstItem );
}


void CleanupConfigPage::currentItemChanged(QListWidgetItem * currentItem,
                                           QListWidgetItem * previousItem )
{
    CleanupListItem * previous = static_cast<CleanupListItem *>( previousItem );
    CleanupListItem * current  = static_cast<CleanupListItem *>( currentItem  );

    if ( previous )
        saveCleanup( previous->cleanup() );

    if ( current )
        loadCleanup( current->cleanup() );
}


void CleanupConfigPage::saveCleanup( Cleanup * cleanup )
{
    logDebug() << cleanup << endl;

    if ( ! cleanup )
        return;

    cleanup->setActive ( _ui->activeGroupBox->isChecked() );
    cleanup->setTitle  ( _ui->titleLineEdit->text()       );
    cleanup->setCommand( _ui->commandLineEdit->text()     );

    if ( _ui->shellComboBox->currentIndex() == 0 )
        cleanup->setShell( "" );
    else
        cleanup->setShell( _ui->shellComboBox->currentText() );

    cleanup->setRecurse( _ui->recursiveCheckBox->isChecked() );

    cleanup->setAskForConfirmation( _ui->askForConfirmationCheckBox->isChecked() );
    cleanup->setWorksForDir       ( _ui->worksForDirCheckBox->isChecked()        );
    cleanup->setWorksForFile      ( _ui->worksForFilesCheckBox->isChecked()      );
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


void CleanupConfigPage::loadCleanup( Cleanup * cleanup )
{
    // logDebug() << cleanup << endl;

    if ( ! cleanup )
        return;

    _ui->activeGroupBox->setChecked( cleanup->active() );
    _ui->titleLineEdit->setText( cleanup->title() );
    _ui->commandLineEdit->setText( cleanup->command() );

    if ( cleanup->shell().isEmpty() )
        _ui->shellComboBox->setCurrentIndex( 0 );
    else
        _ui->shellComboBox->setCurrentText( cleanup->shell() );

    _ui->recursiveCheckBox->setChecked         ( cleanup->recurse()            );
    _ui->askForConfirmationCheckBox->setChecked( cleanup->askForConfirmation() );
    _ui->worksForDirCheckBox->setChecked       ( cleanup->worksForDir()        );
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
