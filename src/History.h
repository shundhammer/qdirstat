/*
 *   File name: History.h
 *   Summary:	Directory navigation history for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef History_h
#define History_h

#include <QString>
#include <QStringList>

namespace QDirStat
{
    /**
     * Class for managing a string-based navigation history of limited size.
     * This is very like the "back" and "forward" buttons in any web browser.
     *
     * Items are added just like on a stack; you can go back to the previous
     * item, and then you can go forward again (for as many items as you went
     * back). As more and more items are added, from a certain point on, the
     * oldest are removed, so there is a limit how far you can go back.
     **/
    class History
    {
    public:

        /**
         * Constructor.
         **/
        History();

        /**
         * Destructor.
         **/
        virtual ~History();

        /**
         * Add an item to the history stack. If the stack's capacity is
         * reached, the oldest item is removed.
         *
         * All items after the previous current item on are removed; so if you
         * just went back one or more steps, it is no longer possible to go
         * forward again after an item was just added.
         **/
        void add( const QString & item );

        /**
         * Clear the complete history.
         *
         * In the context of QDirStat, this is necessary after a new directory
         * was opened for reading.
         **/
        void clear();

        /**
         * Go back one item in the history and return the new current item.
         *
         * Make sure to enable the history "Back" button only if this is
         * possible (check with 'canGoBack()').
         **/
        QString goBack();

        /**
         * Go forward one item in the history and return the new current item.
         *
         * Make sure to enable the history "Forward" button only if this is
         * possible (check with 'canGoBack()').
         **/
        QString goForward();

        /**
         * Check if it is possible to go one item back in the history.
         * Use this to enable or disable the history "Back" button.
         **/
        bool canGoBack() const;

        /**
         * Check if it is possible to go one item forward in the history.
         * Use this to enable or disable the history "Forward" button.
         **/
        bool canGoForward() const;

        /**
         * Return the current item in the history.
         **/
        QString currentItem() const;

        /**
         * Return the index (from 0 on) of the current history item or -1 if
         * the history is empty.
         *
         * Notice that the current index may change or remain the same when
         * items are added: As long as the history stack is not full, it will
         * change; once it is full, it will remain the same (but the oldest
         * item(s) are discarded).
         **/
        int currentIndex() const { return _current; }

        /**
         * Move to any valid history item by index directly.
         * Return 'true' if successful, 'false' if error.
         *
         * This is intended for a history menu where the user can navigate
         * directly to any item in that menu. The effect is the same as
         * multiple goBack() or goForward() calls until that item is reached.
         **/
        bool setCurrentIndex( int index );

        /**
         * Return 'true' if the history is empty, 'false' otherwise.
         **/
        bool isEmpty() const { return _items.isEmpty(); }

        /**
         * Return the history item with the specified 'index' (from 0 on) or an
         * empty string if there is no item with that index.
         **/
        QString item( int index ) const;

        /**
         * The size of the history stack, i.e. the number of items that are
         * currently in it. This is always <= capacity().
         *
         * You can iterate over the history stack from 0 to size() - 1.
         * 0 is the oldest item.
         **/
        int size() const { return _items.size(); }

        /**
         * The capacity of the history stack, i.e. the maximum number of items
         * that it will keep. Once the capacity is reached, each 'add()' first
         * discards the oldest item (i.e. item( 0 )).
         **/
        int capacity() const;

        /**
         * Return all items of the history stack at once. This is very similar
         * to
         *
         *     for ( int i=0; i < history.size(); i++ )
         *         doSomething( history.item( i );
         **/
        const QStringList & allItems() const { return _items; }

        /**
         * Dump the current history stack to the log.
         * This is meant for debugging.
         **/
        void dump() const;

    protected:

        int             _current;
        QStringList     _items;

    };  // class History
}

#endif  // History_h
