/*
 *   File name: CleanupConfigPage.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef CleanupConfigPage_h
#define CleanupConfigPage_h

#include <QWidget>
#include <QListWidgetItem>
#include "ui_cleanup-config-page.h"
#include "Cleanup.h"


namespace QDirStat
{
    class CleanupCollection;


    /**
     * Configuration page (tab) for cleanups:
     * Edit, add, delete, reorder cleanups in the cleanup collection.
     **/
    class CleanupConfigPage: public QWidget
    {
	Q_OBJECT

    public:

	CleanupConfigPage( QWidget * parent = 0 );
	~CleanupConfigPage();

        /**
         * Set the CleanupCollection to work on.
         **/
	void setCleanupCollection( CleanupCollection * collection )
            { _cleanupCollection = collection; }

        /**
         * Return the internal CleanupCollection.
         **/
        CleanupCollection * cleanupCollection() const
            { return _cleanupCollection; }

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
         * Notification that the current item in the cleanup list changed.
         **/
        void currentItemChanged( QListWidgetItem * current,
                                 QListWidgetItem * previous);

    protected:

	/**
	 * Fill the cleanup list widget from the cleanup collection.
	 **/
	void fillCleanupList();

        /**
         * Save the contents of the widgets to the specified cleanup.
         **/
        void saveCleanup( Cleanup * cleanup );

        /**
         * Load the content of the widgets from the specified cleanup.
         **/
        void loadCleanup( Cleanup * cleanup );


	// Data

	Ui::CleanupConfigPage	* _ui;
	CleanupCollection	* _cleanupCollection;

    };	// class CleanupConfigPage


    /**
     * Helper class to match items in the cleanups list to a cleanup.
     **/
    class CleanupListItem: public QListWidgetItem
    {
    public:
	/**
	 * Create a new CleanupListItem. The text will be taken from the
	 * cleanup.
	 **/
	CleanupListItem( Cleanup * cleanup ):
            QListWidgetItem( cleanup->cleanTitle() ),
            _cleanup( cleanup )
            {}

	/**
	 * Return the associated cleanup.
	 **/
	Cleanup * cleanup() const { return _cleanup; }

    protected:
	Cleanup * _cleanup;
    };

}	// namespace QDirStat

#endif	// CleanupConfigPage_h
