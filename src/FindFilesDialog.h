/*
 *   File name: FindFilesDialog.h
 *   Summary:	QDirStat "Find Files" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FindFilesDialog_h
#define FindFilesDialog_h

#include <QDialog>

#include "ui_find-files-dialog.h"
#include "FileSearchFilter.h"


namespace QDirStat
{
    /**
     * Dialog for searching files in the scanned directory tree.
     **/
    class FindFilesDialog: public QDialog
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 *
	 * Consider using the static methods instead.
	 **/
	FindFilesDialog( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~FindFilesDialog();

	/**
	 * Open an "open package" dialog and wait for the user to enter
	 * values.
         *
         * 'canceled_ret' is a return parameter that (if non-null) is set to
	 * 'true' if the user canceled the dialog.
	 **/
	static FileSearchFilter askFindFiles( bool    * canceled_ret,
                                              QWidget * parent = 0   );

	/**
	 * The package filter the user entered.
	 **/
	FileSearchFilter fileSearchFilter();


    public slots:

	/**
	 * Read settings from the config file
	 **/
	void readSettings();

	/**
	 * Write settings to the config file
	 **/
	void writeSettings();


    protected:

	Ui::FindFilesDialog * _ui;

    };	// class FindFilesDialog

}	// namespace QDirStat

#endif	// FindFilesDialog_h
