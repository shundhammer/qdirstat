/*
 *   File name: FindFilesDialog.cpp
 *   Summary:	QDirStat "Find Files" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Qt4Compat.h"

#include "FindFilesDialog.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


FindFilesDialog::FindFilesDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FindFilesDialog )
{
    // logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    qEnableClearButton( _ui->patternField );
    _ui->patternField->setFocus();
}


FindFilesDialog::~FindFilesDialog()
{
    delete _ui;
}


FileSearchFilter FindFilesDialog::fileSearchFilter()
{
    DirInfo * subtree = 0;

    if ( _ui->wholeTreeRadioButton->isChecked() )
        subtree = 0;
    else if ( _ui->currentSubtreeRadioButton->isChecked() )
        subtree = 0;

    int mode        = _ui->filterModeComboBox->currentIndex();
    QString pattern = _ui->patternField->text();

    FileSearchFilter filter( subtree,
                             pattern,
                             (SearchFilter::FilterMode) mode );

    filter.setFindFiles( _ui->findFilesRadioButton->isChecked() ||
                         _ui->findBothRadioButton->isChecked()    );

    filter.setFindDirs ( _ui->findDirectoriesRadioButton->isChecked() ||
                         _ui->findBothRadioButton->isChecked()          );

    filter.setCaseSensitive ( _ui->caseSensitiveCheckBox->isChecked() );

    logDebug() << filter << endl;

    return filter;
}


FileSearchFilter FindFilesDialog::askFindFiles( bool    * canceled_ret,
                                                QWidget * parent )
{
    FindFilesDialog dialog( parent );
    int result = dialog.exec();

    FileSearchFilter filter( 0, "" );
    bool canceled = (result == QDialog::Rejected );

    if ( ! canceled )
        filter = dialog.fileSearchFilter();

    if ( canceled_ret )
        *canceled_ret = canceled;

    return filter;
}

