/*
 *   File name: GeneralConfigPage.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef GeneralConfigPage_h
#define GeneralConfigPage_h

#include "ui_general-config-page.h"


namespace QDirStat
{
    class GeneralConfigPage: public QWidget
    {
        Q_OBJECT

    public:

        /**
         * Constructor.
         **/
        GeneralConfigPage( QWidget * parent = 0 );

        /**
         * Destructor.
         **/
        virtual ~GeneralConfigPage();


    public slots:

	/**
	 * Populate the widgets.
	 **/
	void setup();

	/**
	 * Write changes back to the settings.
	 **/
	void applyChanges();

	/**
	 * Abandon changes and revert everything to the original settings.
	 **/
	void discardChanges();


    protected slots:

        /**
         * Read the values for this page from the QSettings.
         **/
        void readSettings();

        /**
         * Write the values of this page to the QSettings.
         **/
        void writeSettings();


    protected:

	//
	// Data members
	//

	Ui::GeneralConfigPage * _ui;

    }; // class GeneralConfigPage
}

#endif // GeneralConfigPage_h
