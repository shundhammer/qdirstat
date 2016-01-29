/*
 *   File name: ListEditor.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef ListEditor_h
#define ListEditor_h

#include <QWidget>
#include <QListWidgetItem>

class QAbstractButton;


namespace QDirStat
{
    /**
     * This is an abstract widget base class for config pages that have a list
     * of items on the left and details for one item (the current item) on the
     * right.
     *
     * This base class manages selecting an item in the list and displaying its
     * contents (load()), saving any user changes (save()), adding and removing
     * list items, and optionally moving the current item up, down, to the
     * top, and to the bottom of the list.
     *
     * This class does not own, create or destroy any widgets; it only uses
     * widgets that have been created elsewhere, typically via a Qt Designer
     * .ui file. The widgets should be set with any of the appropriate setter
     * methods.
     *
     * This class contains pure virtual methods; derived classes are required
     * to implement them.
     *
     * This class was first designed as a template class, but since even in
     * 2016 Qt's moc still cannot handle templates, ugly void * and nightmarish
     * type casts had to be used. This is due to deficiencies of the underlying
     * tools, not by design. Yes, this is ugly. But it's the best that can be
     * done with that moc preprocessor that dates back to 1995 or so. You'd
     * think that with C++11 and whatnot those limitations could be lifted, but
     * it doesn't look like anybody cares enough.
     **/
    class ListEditor: public QWidget
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	ListEditor( QWidget * parent );

	/**
	 * Destructor.
	 **/
	virtual ~ListEditor();


    protected:

	//
	// Pure virtual methods that are required to be implemented by a
	// derived class
	//


	/**
	 * Fill the list widget: Create a ListEditorItem for each value.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual void fillListWidget() = 0;

	/**
	 * Save the contents of the widgets to the specified value.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual void save( void * value ) = 0;

	/**
	 * Load the content of the widgets from the specified value.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual void load( void * value ) = 0;

	/**
	 * Create a new Value_t item with default values and add it to the
	 * internal list.
	 *
	 * This is called when the 'Add' button is clicked.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual void * createValue() = 0;

	/**
	 * Remove a value from the internal list and delete it.
	 *
	 * This is called when the 'Remove' button is clicked and the user
	 * confirms the confirmation pop-up.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual void removeValue( void * value ) = 0;

	/**
	 * Return a text for the list item of 'value'.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual QString valueText( void * value ) = 0;

	/**
	 * Return the message for the 'really delete?' message for the current
	 * item ('value'). If this returns an empty string, the item cannot be
	 * deleted.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual QString deleteConfirmationMessage( void * value ) = 0;

	/**
	 * Move a value in the internal list. This is called from moveUp(),
	 * moveDown() etc.; 'operation' is one of 'moveUp()', moveDown()
	 * etc. that can be called with QMetaObject::invokeMethod().
	 *
	 * Derived classes that should implement this if the move operations
	 * are to be supported. They should cast 'value' to the proper type and
	 * pass it to the operation that is invoked.
	 *
	 * This is a kludge - a workaround of not being able to use C++
	 * templates.
	 **/
	virtual void moveValue( void * value, const char * operation );


	//--------------------------------------------------------------------


    public:

	/**
	 * Set the QListWidget to work on.
	 **/
	void setListWidget( QListWidget * listWidget );

	/**
	 * Return the QListWidget.
	 **/
	QListWidget * listWidget() const { return _listWidget; }

	/**
	 * Return 'true' if updates are currently locked. save() and load()
	 * should check this and do nothing if updates are locked.
	 **/
	bool updatesLocked() const { return _updatesLocked; }

	/**
	 * Lock or unlock updates.
	 **/
	void setUpdatesLocked( bool locked ) { _updatesLocked = locked; }

	/**
	 * Return the first row for moveUp() and moveToTop(). Normally this is
	 * row 0, but this can be set to another value to freeze the rows
	 * before this logical first row.
	 **/
	int firstRow() const { return _firstRow; }

	/**
	 * Set the first row for moveUp() and moveToTop().
	 **/
	void setFirstRow( int newFirstRow ) { _firstRow = newFirstRow; }


	//
	// Set the various buttons and connect them to the appropriate slot.
	//

	void setMoveUpButton	  ( QAbstractButton * button );
	void setMoveDownButton	  ( QAbstractButton * button );
	void setMoveToTopButton	  ( QAbstractButton * button );
	void setMoveToBottomButton( QAbstractButton * button );
	void setAddButton	  ( QAbstractButton * button );
	void setRemoveButton	  ( QAbstractButton * button );


    public slots:

	/**
	 * Move the current list item one position up.
	 **/
	void moveUp();

	/**
	 * Move the current list item one position down.
	 **/
	void moveDown();

	/**
	 * Move the current list item to the top of the list.
	 **/
	void moveToTop();

	/**
	 * Move the current list item to the bottom of the list.
	 **/
	void moveToBottom();

	/**
	 * Create a new list item.
	 **/
	void add();

	/**
	 * Remove the current list item.
	 **/
	void remove();


    protected slots:

	/**
	 * Enable or disable buttons depending on internal status.
	 **/
	virtual void updateActions();


	/**
	 * Notification that the current item in the list widget changed.
	 **/
	virtual void currentItemChanged( QListWidgetItem * current,
					 QListWidgetItem * previous);

    protected:
	/**
	 * Convert 'item' to a ListEditorItem<void *> and return its value.
	 **/
	void * value( QListWidgetItem * item );

	/**
	 * Enable or disable a button if it is non-null.
	 **/
	void enableButton( QAbstractButton * button, bool enabled );


    private:

	//
	// Data members
	//

	QListWidget	* _listWidget;
	int		  _firstRow;
	bool		  _updatesLocked;
	QAbstractButton * _moveUpButton;
	QAbstractButton * _moveDownButton;
	QAbstractButton * _moveToTopButton;
	QAbstractButton * _moveToBottomButton;
	QAbstractButton * _addButton;
	QAbstractButton * _removeButton;

    };	// class ListEditor


    /**
     * Item class for the QListWidget in a ListEditor. This connects the
     * QListWidgetItem with the void * pointer.
     **/
    class ListEditorItem: public QListWidgetItem
    {
    public:
	/**
	 * Create a new item with the specified text and store the value.
	 **/
	ListEditorItem( const QString & text, void * value ):
	    QListWidgetItem( text ),
	    _value( value )
	    {}

	/**
	 * Return the associated value.
	 **/
	void * value() const { return _value; }

    protected:
	void * _value;
    };

}	// namespace QDirStat

#endif	// ListEditor_h
