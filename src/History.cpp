/*
 *   File name: History.cpp
 *   Summary:	Directory navigation history for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "History.h"
#include "Logger.h"
#include "Exception.h"

#define HISTORY_MAX     16

using namespace QDirStat;


History::History():
    _current( -1 )
{
    _items.reserve( HISTORY_MAX );
}


History::~History()
{
}


void History::clear()
{
    _items.clear();
    _current = -1;
}


QString History::currentItem() const
{
    return item( _current );
}


QString History::item( int index ) const
{
    if ( index >= 0 && index < _items.size() )
        return _items.at( index );
    else
        return "";
}


bool History::canGoBack() const
{
    return _current >= 1;
}


bool History::canGoForward() const
{
    return _current >= 0 && _current < _items.size() - 1;
}


int History::capacity() const
{
    return HISTORY_MAX;
}


QString History::goBack()
{
    if ( ! canGoBack() )
    {
        logWarning() << "Can't go back any more";
        return "";
    }

    _current--;
    // dump();

    return currentItem();
}


QString History::goForward()
{
    if ( ! canGoForward() )
    {
        logWarning() << "Can't go forward any more";
        return "";
    }

    _current++;
    // dump();

    return currentItem();
}


bool History::setCurrentIndex( int index )
{
    if ( index >= 0 && index < _items.size() )
    {
        _current = index;
        // dump();

        return true;
    }
    else
    {
        logWarning() << "Index " << index << " out of range" << endl;
        dump();

        return false;
    }
}


void History::add( const QString & item )
{
    // Remove all items after the current one

    while ( canGoForward() )
    {
        _items.removeLast();
        // _current remains the same!
    }


    // If the history capacity is reached, remove the oldest items
    // until there is space for one more.

    while ( _items.size() >= capacity() )
    {
        _items.removeFirst();

        // Since we removed all items after the current one in the previous
        // step, the current one is the last one.
        _current = _items.size() - 1;
    }


    // Add the new item

    _items << item;
    _current++;

    // logDebug() << "After add():" << endl;
    // dump();
}


void History::dump() const
{
    if ( _items.isEmpty() )
    {
        logDebug() << "Empty history" << endl;
        return;
    }

    logNewline();

    for ( int i = 0; i < _items.size(); ++i )
    {
        QString prefix( 6, ' ' );

        if ( i == _current )
            prefix = " ---> ";

        logDebug() << prefix
                   << "#" << i
                   << ": \"" << _items.at( i ) << "\"" << endl;
    }

    logNewline();
}

