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
#include <QAbstractButton>
#include <QListWidgetItem>

#include "Exception.h"
#include "Logger.h"


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
     * The template parameter 'Value_t' is a pointer type of the class that is
     * being displayed and edited - in the context of QDirStat a Cleanup * or a
     * MimeCategory *.
     *
     * This class does not own, create or destroy any widgets; it only uses
     * widgets that have been created elsewhere, typically via a Qt Designer
     * .ui file. The widgets should be set with any of the appropriate setter
     * methods.
     *
     * This class contains pure virtual methods; derived classes are required
     * to implement them.
     **/
    template<class Value_t> class ListEditor: public QWidget
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	ListEditor( QWidget * parent ):
	    QWidget( parent ),
	    _listWidget(0),
	    _firstRow(0),
	    _updatesLocked(false),
	    _moveUpButton(0),
	    _moveDownButton(0),
	    _moveToTopButton(0),
	    _moveToBottomButton(0),
	    _addButton(0),
	    _removeButton(0)
	    {}

	/**
	 * Destructor.
	 **/
	virtual ~ListEditor() {}


    protected:

	//
	// Pure virtual methods that are required to be implemented by a
	// derived class
	//


	/**
	 * Fill the list widget.
	 * Derived classes are required to implement this.
	 *
	 * For each item, Create a ListEditorItem<Value_t> with the same
	 * Value_t as this ListEditor.
	 **/
	virtual void fillListWidget() = 0;

	/**
	 * Save the contents of the widgets to the specified value.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual void save( Value_t value ) = 0;

	/**
	 * Load the content of the widgets from the specified value.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual void load( Value_t value ) = 0;

	/**
	 * Create a new Value_t item with default values and add it to the
	 * internal list.
	 *
	 * This is called when the 'Add' button is clicked.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual Value_t createValue() = 0;

	/**
	 * Remove a value from the internal list and delete it.
	 *
	 * This is called when the 'Remove' button is clicked and the user
	 * confirms the confirmation pop-up.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual void removeValue( Value_t value ) = 0;

	/**
	 * Return a text for the list item of 'value'.
	 *
	 * Derived classes are required to implement this.
	 **/
	virtual QString valueText( Value_t value ) = 0;

	/**
	 * Return the message for the 'really delete?' message for the current
	 * item ('value'). If this returns an empty string, the item cannot be
	 * deleted.
	 **/
	virtual QString deleteConfirmationMessage( Value_t value ) = 0;


	//--------------------------------------------------------------------


    public:

	/**
	 * Set the QListWidget to work on.
	 **/
	void setListWidget( QListWidget * listWidget )
	{
	    _listWidget = listWidget;

	    connect( _listWidget, SIGNAL( currentItemChanged( QListWidgetItem *,
							      QListWidgetItem *	 ) ),
		     this,	  SLOT	( currentItemChanged( QListWidgetItem *,
							      QListWidgetItem *	 ) ) );
	}

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


#define CON( RCVR_SLOT) \
	connect( button, SIGNAL( clicked() ), this, SLOT( RCVR_SLOT ) )

	//
	// Set the various buttons and connect them to the appropriate slot.
	//

	void setMoveUpButton	  ( QAbstractButton * button ) { _moveUpButton	     = button; CON( moveUp()	   ); }
	void setMoveDownButton	  ( QAbstractButton * button ) { _moveDownButton     = button; CON( moveDown()	   ); }
	void setMoveToTopButton	  ( QAbstractButton * button ) { _moveToTopButton    = button; CON( moveToTop()	   ); }
	void setMoveToBottomButton( QAbstractButton * button ) { _moveToBottomButton = button; CON( moveToBottom() ); }
	void setAddButton	  ( QAbstractButton * button ) { _addButton	     = button; CON( add()	   ); }
	void setRemoveButton	  ( QAbstractButton * button ) { _removeButton	     = button; CON( remove()	   ); }

#undef CON

    public slots:

	/**
	 * Move the current list item one position up.
	 **/
	void moveUp()
	{
	    QListWidgetItem * currentItem = _listWidget->currentItem();
	    int currentRow		  = _listWidget->currentRow();

	    if ( ! currentItem )
		return;

	    if ( currentRow > 0 )
	    {
		_updatesLocked = true;
		_listWidget->takeItem( currentRow );
		_listWidget->insertItem( currentRow - 1, currentItem );
		_listWidget->setCurrentItem( currentItem );
		// _listMover->moveUp( cleanup( currentItem ) );
		_updatesLocked = false;
	    }
	}


	/**
	 * Move the current list item one position down.
	 **/
	void moveDown()
	{
	    QListWidgetItem * currentItem = _listWidget->currentItem();
	    int currentRow		  = _listWidget->currentRow();

	    if ( ! currentItem )
		return;

	    if ( currentRow < _listWidget->count() - 1 )
	    {
		_updatesLocked = true;
		_listWidget->takeItem( currentRow );
		_listWidget->insertItem( currentRow + 1, currentItem );
		_listWidget->setCurrentItem( currentItem );
		// _listMover->moveDown( cleanup( currentItem ) );
		_updatesLocked = false;
	    }
	}


	/**
	 * Move the current list item to the top of the list.
	 **/
	void moveToTop()
	{
	    QListWidgetItem * currentItem = _listWidget->currentItem();
	    int currentRow		  = _listWidget->currentRow();

	    if ( ! currentItem )
		return;

	    if ( currentRow > 0 )
	    {
		_updatesLocked = true;
		_listWidget->takeItem( currentRow );
		_listWidget->insertItem( 0, currentItem );
		_listWidget->setCurrentItem( currentItem );
		// _listMover->moveToTop( cleanup( currentItem ) );
		_updatesLocked = false;
	    }
	}


	/**
	 * Move the current list item to the bottom of the list.
	 **/
	void moveToBottom()
	{
	    QListWidgetItem * currentItem = _listWidget->currentItem();
	    int currentRow		  = _listWidget->currentRow();

	    if ( ! currentItem )
		return;

	    if ( currentRow < _listWidget->count() - 1 )
	    {
		_updatesLocked = true;
		_listWidget->takeItem( currentRow );
		_listWidget->addItem( currentItem );
		_listWidget->setCurrentItem( currentItem );
		// _listMover->moveToBottom( cleanup( currentItem ) );
		_updatesLocked = false;
	    }
	}


	/**
	 * Create a new list item.
	 **/
	void add()
	{
	    Value_t value = createValue();
	    CHECK_NEW( value );

	    ListEditorItem<Value_t> item = new ListEditorItem<Value_t>( valueText( value ), value );
	    CHECK_NEW( item );

	    _listWidget->addItem( item );
	    _listWidget->setCurrentItem( item );
	}

	/**
	 * Remove the current list item.
	 **/
	void remove()
	{
	    QListWidgetItem * currentItem = _listWidget->currentItem();
	    int currentRow		  = _listWidget->currentRow();

	    if ( ! currentItem )
		return;

	    Value_t value = this->value( currentItem );

	    //
	    // Confirmation popup
	    //

	    QString msg = deleteConfirmationMessage( value );

	    if ( ! msg.isEmtpy() )
	    {
		int ret = QMessageBox::question( window(),
						 tr( "Please Confirm" ), // title
						 msg );
		if ( ret == QMessageBox::Yes )
		{
		    //
		    // Delete current cleanup
		    //

		    _updatesLocked = true;
		    _listWidget->takeItem( currentRow );
		    delete currentItem;
		    removeValue( value );
		    _updatesLocked = false;

		    load( this->value( _listWidget->currentItem() ) );
		}
	    }
	}


    protected slots:

	/**
	 * Enable or disable buttons depending on internal status.
	 **/
	virtual void updateActions()
	{
	    int currentRow = _listWidget->currentRow();
	    int lastRow	   = _listWidget->count() - 1;

	    enableButton( _moveToTopButton,    currentRow > _firstRow );
	    enableButton( _moveUpButton,       currentRow > _firstRow );
	    enableButton( _moveDownButton,     currentRow < lastRow   );
	    enableButton( _moveToBottomButton, currentRow < lastRow   );
	}


	/**
	 * Notification that the current item in the list widget changed.
	 **/
	virtual void currentItemChanged( QListWidgetItem * current,
					 QListWidgetItem * previous)
	{
	    save( value( previousItem ) );
	    load( value( currentItem  ) );
	    updateActions();
	}


    protected:
	/**
	 * Convert 'item' to a ListEditorItem<Value_t> and return its value.
	 **/
	Value_t value( QListWidgetItem * item )
	{
	    if ( ! item )
		return 0;

	    ListEditorItem<Value_t> editorItem = dynamic_cast<ListEditorItem<Value_t> >( item );
	    CHECK_DYNAMIC_CAST( editorItem, "Value_t" );

	    return editorItem->value();
	}

	/**
	 * Enable or disable a button if it is non-null.
	 **/
	void enableButton( QAbstractButton * button, bool enabled )
	{
	    if ( button )
		button->setEnabled( enabled );
	}


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
     * QListWidgetItem with the Value_t pointer.
     **/
    template<class Value_t> class ListEditorItem: public QListWidgetItem
    {
    public:
	/**
	 * Create a new item with the specified text and store the value.
	 **/
	ListEditorItem( const QString & text, Value_t value ):
	    QListWidgetItem( text ),
	    _value( value )
	    {}

	/**
	 * Return the associated value.
	 **/
	Value_t value() const { return _value; }

    protected:
	Value_t _value;
    };

}	// namespace QDirStat

#endif	// ListEditor_h
