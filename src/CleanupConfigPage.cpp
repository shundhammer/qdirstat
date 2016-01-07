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
}


void CleanupConfigPage::discardChanges()
{
    logDebug() << endl;
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
}


void CleanupConfigPage::loadCleanup( Cleanup * cleanup )
{
    logDebug() << cleanup << endl;

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
    _ui->outputWindowTimeoutSpinBox->setValue( cleanup->outputWindowTimeout() / 1000.0 );
    _ui->outputWindowAutoCloseCheckBox->setChecked( cleanup->outputWindowAutoClose() );

    _ui->refreshPolicyComboBox->setCurrentIndex( cleanup->refreshPolicy() );
}
