/*
 *   File name: HistoryButtons.cpp
 *   Summary:	History buttons handling for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMenu>
#include <QAction>
#include <QActionGroup>

#include "HistoryButtons.h"
#include "DirInfo.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


HistoryButtons::HistoryButtons( QAction * actionGoBack,
                                QAction * actionGoForward ):
    QObject(),
    _actionGoBack   ( actionGoBack ),
    _actionGoForward( actionGoForward )
{
    _history = new History();
    CHECK_NEW( _history );

    initHistoryButtons();
}


HistoryButtons::~HistoryButtons()
{
    delete _historyMenu;
    delete _history;
}


void HistoryButtons::updateActions()
{
    _actionGoBack->setEnabled   ( _history->canGoBack()    );
    _actionGoForward->setEnabled( _history->canGoForward() );
}


void HistoryButtons::historyGoBack()
{
    emit navigateToUrl( _history->goBack() );
    updateActions();
}


void HistoryButtons::historyGoForward()
{
    emit navigateToUrl( _history->goForward() );
    updateActions();
}


void HistoryButtons::addToHistory( FileInfo * item )
{
    if ( item && ! item->isDirInfo() && item->parent() )
        item = item->parent();

    if ( item )
    {
        QString url = item->debugUrl();

        if ( url != _history->currentItem() )
        {
            _history->add( url );
            updateActions();
        }
    }
}


void HistoryButtons::initHistoryButtons()
{
    _historyMenu = new QMenu();
    _historyMenu->addAction( "Dummy 1" );

    connect( _historyMenu, SIGNAL( aboutToShow()       ),
             this,         SLOT  ( updateHistoryMenu() ) );

    connect( _historyMenu, SIGNAL( triggered        ( QAction * ) ),
             this,         SLOT  ( historyMenuAction( QAction * ) ) );

    _actionGoBack->setMenu   ( _historyMenu );
    _actionGoForward->setMenu( _historyMenu );
}


void HistoryButtons::updateHistoryMenu()
{
    _historyMenu->clear();
    QActionGroup * actionGroup = new QActionGroup( _historyMenu );

    QStringList items = _history->allItems();
    int current = _history->currentIndex();

    for ( int i = items.size() - 1; i >= 0; i-- )
    {
        QAction * action = new QAction( items.at( i ), _historyMenu );
        action->setCheckable( true );
        action->setChecked( i == current );
        action->setData( i );
        actionGroup->addAction( action );
        _historyMenu->addAction( action );
    }
}


void HistoryButtons::historyMenuAction( QAction * action )
{
    if ( action )
    {
        QVariant data = action->data();
        int index = data.toInt();

        if ( _history->setCurrentIndex( index ) )
            navigateToUrl( _history->currentItem() );
    }
}

