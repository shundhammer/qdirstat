/*
 *   File name: OpenPkgDialog.cpp
 *   Summary:	QDirStat "open installed packages" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Qt4Compat.h"

#include "OpenPkgDialog.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


OpenPkgDialog::OpenPkgDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::OpenPkgDialog )
{
    // logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    qEnableClearButton( _ui->pkgPatternField );
    _ui->pkgPatternField->setFocus();
}


OpenPkgDialog::~OpenPkgDialog()
{
    delete _ui;
}


PkgFilter OpenPkgDialog::pkgFilter()
{
    if ( _ui->allPkgRadioButton->isChecked() )
    {
        logDebug() << "SelectAll" << endl;
        return PkgFilter( "", PkgFilter::SelectAll );
    }
    else
    {
        int mode        = _ui->filterModeComboBox->currentIndex();
        QString pattern = _ui->pkgPatternField->text();
        PkgFilter filter( pattern, (PkgFilter::FilterMode) mode );
        // logDebug() << filter << endl;

        return filter;
    }
}


PkgFilter OpenPkgDialog::askPkgFilter( bool    * canceled_ret,
                                       QWidget * parent )
{
    OpenPkgDialog dialog( parent );
    int result = dialog.exec();

    PkgFilter pkgFilter( "" );
    bool canceled = (result == QDialog::Rejected );

    if ( ! canceled )
        pkgFilter = dialog.pkgFilter();

    if ( canceled_ret )
        *canceled_ret = canceled;

    return pkgFilter;
}

