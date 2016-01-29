/*
 *   File name: CleanupConfigPage.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef CleanupConfigPage_h
#define CleanupConfigPage_h

#include "ListEditor.h"
#include "ui_cleanup-config-page.h"
#include "Cleanup.h"


namespace QDirStat
{
    class CleanupCollection;


    /**
     * Configuration page (tab) for cleanups:
     * Edit, add, delete, reorder cleanups in the cleanup collection.
     **/
    class CleanupConfigPage: public ListEditor<Cleanup *>
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
	 * Notification that the user changed the "Title" field of the
	 * current cleanup.
	 **/
	void titleChanged( const QString & newTitle );


    protected:

	/**
	 * Fill the cleanup list widget from the cleanup collection.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual void fillListWidget() Q_DECL_OVERRIDE;

	/**
	 * Save the contents of the widgets to the specified value.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual void save( Value_t value ) Q_DECL_OVERRIDE;

	/**
	 * Load the content of the widgets from the specified value.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual void load( Value_t value ) Q_DECL_OVERRIDE;

	/**
	 * Create a new Value_t item with default values.
	 * This is called when the 'Add' button is clicked.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual Cleanup * createValue() Q_DECL_OVERRIDE;

	/**
	 * Remove a value from the internal list and delete it.
	 *
	 * This is called when the 'Remove' button is clicked and the user
	 * confirms the confirmation pop-up.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual void removeValue( Value_t value );

	/**
	 * Return a text for the list item of 'value'.
	 *
	 * Implemented from ListEditor.
	 **/

	virtual QString valueText( Cleanup * cleanup ) Q_DECL_OVERRIDE;

	/**
	 * Return the message for the 'really delete?' message for the current
	 * item ('value'). If this returns an empty string, the item cannot be
	 * deleted.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual QString deleteConfirmationMessage( Value_t value ) Q_DECL_OVERRIDE;


	//
	// Data members
	//

	Ui::CleanupConfigPage	* _ui;
	CleanupCollection	* _cleanupCollection;

    };	// class CleanupConfigPage

}	// namespace QDirStat

#endif	// CleanupConfigPage_h
