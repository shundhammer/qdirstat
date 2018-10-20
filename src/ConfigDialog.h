/*
 *   File name: ConfigDialog.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef ConfigDialog_h
#define ConfigDialog_h

#include <QDialog>
#include "ui_config-dialog.h"


namespace QDirStat
{
    class CleanupConfigPage;
    class MimeCategoryConfigPage;
    class ExcludeRulesConfigPage;
    class GeneralConfigPage;


    /**
     * Configuration dialog for QDirStat.
     *
     * This class is only the wrapper for the individual config pages; it
     * maintains the tab widget to switch between the pages (the tabs) and the
     * dialog buttons ("OK", "Apply", "Cancel").
     *
     * Each page is pretty much self-sufficient.
     **/
    class ConfigDialog: public QDialog
    {
	Q_OBJECT

    public:
	/**
	 * Constructor. Create the dialog and all pages.
	 **/
	ConfigDialog( QWidget * parent );

	/**
	 * Destructor.
	 **/
	virtual ~ConfigDialog();

	/**
	 * Return the cleanup config page.
	 **/
	CleanupConfigPage * cleanupConfigPage() const
	    { return _cleanupConfigPage; }

	/**
	 * Return the mime category config page.
	 **/
	MimeCategoryConfigPage * mimeCategoryConfigPage() const
	    { return _mimeCategoryConfigPage; }

    public slots:

	/**
	 * Notification to reinitialize and populate the widgets of config
	 * pages.
	 **/
	void setup();

	/**
	 * Apply the changes in dialog contents, but leave the dialog open.
	 **/
	void apply();

	/**
	 * Accept the dialog contents, i.e. the user clicked the "OK" button.
	 *
	 * Reimplemented from QDialog.
	 **/
	virtual void accept() Q_DECL_OVERRIDE;

	/**
	 * Reject the dialog contents, i.e. the user clicked the "Cancel"
	 * button.
	 *
	 * Reimplemented from QDialog.
	 **/
	virtual void reject() Q_DECL_OVERRIDE;

    signals:

	/**
	 * Emitted in setup().
	 *
	 * This is the signal to reinitialize and populate the widgets of
	 * config pages.
	 **/
	void reinit();

	/**
	 * Emitted when the "OK" or the "Apply" button was clicked:
	 *
	 * This is the signal to apply all changes to the settings and/or the
	 * widgets.
	 **/
	void applyChanges();

	/**
	 * Emitted when the "Cancel" button was clicked:
	 *
	 * This is the signal to discard all changes and re-apply the old
	 * settings.
	 **/
	void discardChanges();

    protected:

	//
	// Data members
	//

	Ui::ConfigDialog	* _ui;
	CleanupConfigPage	* _cleanupConfigPage;
	MimeCategoryConfigPage	* _mimeCategoryConfigPage;
	ExcludeRulesConfigPage  * _excludeRulesConfigPage;
	GeneralConfigPage	* _generalConfigPage;

    };	// class ConfigDialog

}	// namespace QDirStat

#endif	// ConfigDialog_h
