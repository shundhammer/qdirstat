/*
 *   File name: OpenPkgDialog.h
 *   Summary:	QDirStat "open installed packages" dialog
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef OpenPkgDialog_h
#define OpenPkgDialog_h

#include <QDialog>

#include "ui_open-pkg-dialog.h"
#include "PkgFilter.h"


namespace QDirStat
{
    /**
     * Dialog to let the user select installed packages to open, very much like
     * a "get existing directory" dialog, but returning a PkgFilter instead.
     **/
    class OpenPkgDialog: public QDialog
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 *
	 * Consider using the static methods instead.
	 **/
	OpenPkgDialog( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~OpenPkgDialog();

	/**
	 * Open an "open package" dialog and wait for the user to enter
	 * values.
         *
         * 'canceled_ret' is a return parameter that (if non-null) is set to
	 * 'true' if the user canceled the dialog.
	 **/
	static PkgFilter askPkgFilter( bool    * canceled_ret,
				       QWidget * parent = 0   );

	/**
	 * The package filter the user entered.
	 **/
	PkgFilter pkgFilter();

    protected:

	Ui::OpenPkgDialog * _ui;

    };	// class OpenPkgDialog

}	// namespace QDirStat

#endif	// OpenPkgDialog_h
