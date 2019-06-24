/*
 *   File name: ShowUnpkgFilesDialog.cpp
 *   Summary:	QDirStat "show unpackaged files" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "ShowUnpkgFilesDialog.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


ShowUnpkgFilesDialog::ShowUnpkgFilesDialog( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::ShowUnpkgFilesDialog )
{
    CHECK_NEW( _ui );
    _ui->setupUi( this );
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

