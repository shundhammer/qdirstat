/*
 *   File name: MimeCategoryConfigPage.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef MimeCategoryConfigPage_h
#define MimeCategoryConfigPage_h

#include <QWidget>
#include <QListWidgetItem>
#include "ui_mime-category-config-page.h"
#include "MimeCategorizer.h"


namespace QDirStat
{
    /**
     * Configuration page (tab) for MimeCategories:
     * Edit, add, delete categories in the MimeCategorizer.
     **/
    class MimeCategoryConfigPage: public QWidget
    {
	Q_OBJECT

    public:

	MimeCategoryConfigPage( QWidget * parent = 0 );
	~MimeCategoryConfigPage();

	/**
	 * Set the MimeCategorizer to work on.
	 **/
	void setMimeCategorizer( MimeCategorizer * categorizer )
	    { _categorizer = categorizer; }

	/**
	 * Return the internal MimeCategorizer.
	 **/
	MimeCategorizer * mimeCategorizer() const
	    { return _categorizer; }

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
	 * Notification that the current item in the category list changed.
	 **/
	void currentItemChanged( QListWidgetItem * current,
				 QListWidgetItem * previous);

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

	/**
	 * Enable or disable buttons depending on internal status.
	 **/
	void updateActions();

	/**
	 * Create a new category.
	 **/
	void addCategory();

	/**
	 * Delete the current category.
	 **/
	void deleteCategory();

    protected slots:

	/**
	 * Open a color dialog and let the user pick a color for the current
	 * category.
	 **/
	void pickColor();

    protected:

	/**
	 * Fill the category list widget from the category collection.
	 **/
	void fillCategoryList();

	/**
	 * Convert 'item' to a CategoryListItem and return its category.
	 **/
	MimeCategory * category( QListWidgetItem * item );

	/**
	 * Save the contents of the widgets to the specified category.
	 **/
	void saveCategory( MimeCategory * category );

	/**
	 * Load the content of the widgets from the specified category.
	 **/
	void loadCategory( MimeCategory * category );

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
	QListWidget		   * _listWidget;
	DirTree			   * _dirTree;
	bool			     _updatesLocked;

    };	// class MimeCategoryConfigPage


    /**
     * Helper class to match items in the category list to a category.
     **/
    class CategoryListItem: public QListWidgetItem
    {
    public:
	/**
	 * Create a new CategoryListItem. The text will be taken from the
	 * category.
	 **/
	CategoryListItem( MimeCategory * category ):
	    QListWidgetItem( category->name() ),
	    _category( category )
	    {}

	/**
	 * Return the associated category.
	 **/
	MimeCategory * category() const { return _category; }

    protected:
	MimeCategory * _category;
    };

}	// namespace QDirStat

#endif	// MimeCategoryConfigPage_h
