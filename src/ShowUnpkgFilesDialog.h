/*
 *   File name: ShowUnpkgFilesDialog.h
 *   Summary:	QDirStat "show unpackaged files" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef ShowUnpkgFilesDialog_h
#define ShowUnpkgFilesDialog_h


#include <QDialog>
#include <QString>
#include <QStringList>

#include "ui_show-unpkg-files-dialog.h"


namespace QDirStat
{
    class ExistingDirCompleter;
    
    /**
     * Dialog to let the user select parameters for showing unpackaged
     * files. This is very much like a "get existing directory" dialog with
     * some more widgets.
     *
     * Usage:
     *
     *     ShowUnpkgFilesDialog dialog( this );
     *
     *     if ( dialog.exec() == QDialog::Accepted )
     *     {
     *         QString dir = dialog.startingDir();
     *         QStringList excludeDirs = dialog.excludeDirs();
     *
     *         readUnpkgFiles( dir, excludeDirs );
     *     }
     **/
    class ShowUnpkgFilesDialog: public QDialog
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
        ShowUnpkgFilesDialog( QWidget * parent = 0 );

        /**
         * Destructor.
         **/
        virtual ~ShowUnpkgFilesDialog();

        /**
         * Get the starting directory from the dialog's widgets or an empty
         * string if the dialog was cancelled.
         **/
        QString startingDir() const;

        /**
         * Get the directories to exclude from the dialog's widgets.
         **/
        QStringList excludeDirs() const;


    protected:

        Ui::ShowUnpkgFilesDialog * _ui;
        QPushButton *              _okButton;

    };  // class ShowUnpkgFilesDialog

}       // namespace QDirStat

#endif  // ShowUnpkgFilesDialog_h
