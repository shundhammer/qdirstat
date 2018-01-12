/*
 *   File name: ListEditor.h
 *   Summary:	QDirStat configuration dialog classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QAbstractButton>
#include <QMessageBox>

#include "ListEditor.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


ListEditor::ListEditor( QWidget * parent ):
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
{

}


ListEditor::~ListEditor()
{

}


void ListEditor::setListWidget( QListWidget * listWidget )
{
    _listWidget = listWidget;

    connect( _listWidget, SIGNAL( currentItemChanged( QListWidgetItem *,
						      QListWidgetItem *	 ) ),
	     this,	  SLOT	( currentItemChanged( QListWidgetItem *,
						      QListWidgetItem *	 ) ) );
}


#define CONNECT_BUTTON(BUTTON, RCVR_SLOT) \
    connect( (BUTTON), SIGNAL( clicked() ), this, SLOT( RCVR_SLOT ) )

void ListEditor::setMoveUpButton( QAbstractButton * button )
{
    _moveUpButton = button;
    CONNECT_BUTTON( button, moveUp() );
}


void ListEditor::setMoveDownButton( QAbstractButton * button )
{
    _moveDownButton = button;
    CONNECT_BUTTON( button, moveDown() );
}


void ListEditor::setMoveToTopButton( QAbstractButton * button )
{
    _moveToTopButton = button;
    CONNECT_BUTTON( button, moveToTop() );
}


void ListEditor::setMoveToBottomButton( QAbstractButton * button )
{
    _moveToBottomButton = button;
    CONNECT_BUTTON( button, moveToBottom() );
}


void ListEditor::setAddButton( QAbstractButton * button )
{
    _addButton = button;
    CONNECT_BUTTON( button, add() );
}


void ListEditor::setRemoveButton( QAbstractButton * button )
{
    _removeButton = button;
    CONNECT_BUTTON( button, remove() );
}


void ListEditor::moveUp()
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
        moveValue( value( currentItem ), "moveUp" );
	_updatesLocked = false;
    }
}


void ListEditor::moveDown()
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
        moveValue( value( currentItem ), "moveDown" );
	_updatesLocked = false;
    }
}


void ListEditor::moveToTop()
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
        moveValue( value( currentItem ), "moveToTop" );
	_updatesLocked = false;
    }
}


void ListEditor::moveToBottom()
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
        moveValue( value( currentItem ), "moveToBottom" );
	_updatesLocked = false;
    }
}


void ListEditor::add()
{
    void * value = createValue();
    CHECK_NEW( value );

    ListEditorItem * item = new ListEditorItem( valueText( value ), value );
    CHECK_NEW( item );

    _listWidget->addItem( item );
    _listWidget->setCurrentItem( item );
}


void ListEditor::remove()
{
    QListWidgetItem * currentItem = _listWidget->currentItem();
    int currentRow		  = _listWidget->currentRow();

    if ( ! currentItem )
	return;

    void * value = this->value( currentItem );

    //
    // Confirmation popup
    //

    QString msg = deleteConfirmationMessage( value );

    if ( ! msg.isEmpty() )
    {
	int ret = QMessageBox::question( window(),
					 tr( "Please Confirm" ), // title
					 msg );
	if ( ret == QMessageBox::Yes )
	{
	    //
	    // Delete current item
	    //

	    _updatesLocked = true;
	    _listWidget->takeItem( currentRow );
	    delete currentItem;
	    removeValue( value );
            updateActions();
	    _updatesLocked = false;

	    load( this->value( _listWidget->currentItem() ) );
	}
    }
}


void ListEditor::updateActions()
{
    if ( _listWidget->count() == 0 )
    {
        enableButton( _removeButton,       false );
        enableButton( _moveToTopButton,    false );
        enableButton( _moveUpButton,       false );
        enableButton( _moveDownButton,     false );
        enableButton( _moveToBottomButton, false );
    }
    else
    {
        int currentRow = _listWidget->currentRow();
        int lastRow    = _listWidget->count() - 1;

        enableButton( _removeButton,       true );
        enableButton( _moveToTopButton,    currentRow > _firstRow );
        enableButton( _moveUpButton,       currentRow > _firstRow );
        enableButton( _moveDownButton,     currentRow < lastRow   );
        enableButton( _moveToBottomButton, currentRow < lastRow   );
    }
}


void ListEditor::currentItemChanged( QListWidgetItem * current,
                                     QListWidgetItem * previous)
{
    save( value( previous ) );
    load( value( current  ) );
    updateActions();
}


void * ListEditor::value( QListWidgetItem * item )
{
    if ( ! item )
	return 0;

    ListEditorItem * editorItem = dynamic_cast<ListEditorItem *>( item );
    CHECK_DYNAMIC_CAST( editorItem, "ListEditorItem *" );

    return editorItem->value();
}


void ListEditor::enableButton( QAbstractButton * button, bool enabled )
{
    if ( button )
	button->setEnabled( enabled );
}


void ListEditor::moveValue( void * value, const char * operation )
{
    Q_UNUSED( value );
    Q_UNUSED( operation );
}
