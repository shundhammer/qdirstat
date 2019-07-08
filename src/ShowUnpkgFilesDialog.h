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
#include "UnpkgSettings.h"


class QContextMenuEvent;


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
     *	   ShowUnpkgFilesDialog dialog( this );
     *
     *	   if ( dialog.exec() == QDialog::Accepted )
     *	   {
     *	       QString dir = dialog.startingDir();
     *	       QStringList excludeDirs = dialog.excludeDirs();
     *
     *	       readUnpkgFiles( dir, excludeDirs );
     *	   }
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
         * Get all values from the widgets at once.
         **/
        UnpkgSettings values() const;

	/**
	 * Get the starting directory from the dialog's widgets or an empty
	 * string if the dialog was cancelled.
	 **/
	QString startingDir() const;

	/**
	 * Get the directories to exclude from the dialog's widgets.
	 **/
	QStringList excludeDirs() const;

	/**
	 * Get the wildcard patterns of files to ignore from the dialog's
	 * widgets.
	 **/
	QStringList ignorePatterns() const;


    public slots:

	/**
	 * Read settings from the config file
	 **/
	void readSettings();

	/**
	 * Write settings to the config file
	 **/
	void writeSettings();


    protected slots:

	/**
	 * Reset the exclude directories etc. to the default values after a
	 * confirmation.
	 **/
	void restoreDefaults();


    protected:

        /**
         * Set all values at once.
         **/
        void setValues( const UnpkgSettings & settings );

	/**
	 * Get the content of a QPlainTextEdit widget as QStringList with
	 * leading and trailing whitespace removed from each line and without
	 * empty lines.
	 **/
	QStringList cleanedLines( QPlainTextEdit * widget ) const;


	// Data members

	Ui::ShowUnpkgFilesDialog * _ui;
	QPushButton *		   _okButton;

    };	// class ShowUnpkgFilesDialog

}	// namespace QDirStat

#endif	// ShowUnpkgFilesDialog_h
