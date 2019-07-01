/*
 *   File name: MimeCategoryConfigPage.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef MimeCategoryConfigPage_h
#define MimeCategoryConfigPage_h

#include "ListEditor.h"
#include "ui_mime-category-config-page.h"
#include "MimeCategorizer.h"


namespace QDirStat
{
    /**
     * Configuration page (tab) for MimeCategories:
     * Edit, add, delete categories in the MimeCategorizer.
     **/
    class MimeCategoryConfigPage: public ListEditor
    {
	Q_OBJECT

    public:

	MimeCategoryConfigPage( QWidget * parent = 0 );
	virtual ~MimeCategoryConfigPage();

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
	 * Notification that the user changed the "Name" field of the
	 * current category.
	 **/
	void nameChanged( const QString & newName );

	/**
	 * Notification that the user changed the "Color" field of the
	 * current category.
	 **/
	void colorChanged( const QString & newColor );


    protected slots:

	/**
	 * Open a color dialog and let the user pick a color for the current
	 * category.
	 **/
	void pickColor();

    protected:

	/**
	 * Fill the category list widget from the category collection.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual void fillListWidget() Q_DECL_OVERRIDE;

	/**
	 * Save the contents of the widgets to the specified category.
	 **/
	virtual void save( void * value ) Q_DECL_OVERRIDE;

	/**
	 * Load the content of the widgets from the specified category.
	 **/
	virtual void load( void * value ) Q_DECL_OVERRIDE;

	/**
	 * Create a new value with default values.
	 * This is called when the 'Add' button is clicked.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual void * createValue() Q_DECL_OVERRIDE;

	/**
	 * Remove a value from the internal list and delete it.
	 *
	 * This is called when the 'Remove' button is clicked and the user
	 * confirms the confirmation pop-up.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual void removeValue( void * value );

	/**
	 * Return a text for the list item of 'value'.
	 *
	 * Implemented from ListEditor.
	 **/

	virtual QString valueText( void * value ) Q_DECL_OVERRIDE;

	/**
	 * Return the message for the 'really delete?' message for the current
	 * item ('value'). If this returns an empty string, the item cannot be
	 * deleted.
	 *
	 * Implemented from ListEditor.
	 **/
	virtual QString deleteConfirmationMessage( void * value ) Q_DECL_OVERRIDE;

	/**
	 * Convert 'patternList' into a newline-separated string and set it as
	 * text of 'textEdit'.
	 **/
	void setPatternList( QPlainTextEdit    * textEdit,
			     const QStringList & patternList );

	/**
	 * Add demo content to the tremap view.
	 **/
	void populateTreemapView();


	// Data

	Ui::MimeCategoryConfigPage * _ui;
	MimeCategorizer		   * _categorizer;
	DirTree			   * _dirTree;

    };	// class MimeCategoryConfigPage

}	// namespace QDirStat

#endif	// MimeCategoryConfigPage_h
