/*
 *   File name: BookmarksManager.h
 *   Summary:	Bookmarks Manager for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef BookmarksManager_h
#define BookmarksManager_h

#include <QObject>
#include <QString>
#include <QStringList>

class QMenu;


namespace QDirStat
{
    /**
     * Manager class for a collection of bookmarks that each point to a
     * directory in the filesystem.
     *
     * A bookmark is just a string with the URL. It can be used to navigate
     * quickly to that location in the tree, or to select a number of
     * directories for a cleanup action such as clearing the contents of each
     * one of them.
     *
     * One example are the plethora of cache directories that all major
     * Internet browsers leave behind in the user's home directory in dozens of
     * locations, even if the user uses the "clear history" or "empty caches"
     * functions.
     *
     * This class is a collection of bookmarks plus a number of methods to
     * handle them; to read and write them from and to file, to add one, to
     * check which of them are inside the current DirTree.
     *
     * It also has a number of signals and slots to use them from menus and
     * widgets like the bookmarksButton next to the BreadcrumbNavigator where
     * the user can toggle the bookmark status (on/off) of the current URL,
     * and to indicate the status of the current URL as it changes in the
     * SelectionModel.
     *
     * The bookmarks are always kept in alphabetical order. Adding or removing
     * a bookmark
     **/
    class BookmarksManager: public QObject
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
         *
         * Remember to call setBookmarksMenu().
	 **/
	BookmarksManager( QObject * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~BookmarksManager();

        /**
         * One-time initialization: Set the associated bookmarks menu.
         *
         * That menu will then be managed by this class: The menu will contain
         * a list of bookmarks that the user can activate to navigate to that
         * URL.
         *
         * See also 'rebuildBookmarksMenu()'.
         **/
        void setBookmarksMenu( QMenu * bookmarksMenu )
            { _bookmarksMenu = bookmarksMenu; }


    signals:

        /**
         * Sent when the user clicks on a bookmark in the bookmark menu.
         * This should be connected to an action that navigates to that URL,
         * i.e. which makes that URL the current URL.
         **/
	void navigateToUrl( const QString & url );


    public:

        /**
         * Return the complete list of bookmarks as a const reference.
         **/
        const QStringList & bookmarks() const { return _bookmarks; }

        /**
         * Return 'true' when the bookmarks collection is empty.
         **/
        bool isEmpty() const { return _bookmarks.isEmpty(); }

        /**
         * Returns the size of the bookmarks collection, i.e. the number of
         * bookmarks.
         **/
        int size() const { return _bookmarks.size(); }

        /**
         * Return 'true' if the bookmarks contain the specified URL.
         **/
        bool contains( const QString & url ) { return _bookmarks.contains( url ); }

        /**
         * Add a bookmark to the collection, unless it's already there.
         *
         * If 'update' is 'true' (the default), this will also sort the
         * bookmarks and rebuild the bookmarks menu.
         **/
        void add( const QString & bookmark, bool update = true );

        /**
         * Remove a bookmark from the collection if it's there.
         *
         * If 'update' is 'true' (the default), this will also sort the
         * bookmarks and rebuild the bookmarks menu.
         **/
        void remove( const QString & bookmark, bool update = true );

        /**
         * Notification that the base URL of the current DirTree has changed,
         * i.e. that a new directory tree is being read. This rebuilds the
         * bookmarks menu.
         **/
        void setBaseUrl( const QString & newBaseUrl );

        /**
         * Clear the bookmarks collection.
         * This does not rebuild the menu.
         **/
        void clear();

        /**
         * Read the bookmarks from a plain text file
         * ~/.config/QDirStat/bookmarks.txt
         *
         * This does not rebuild the menu.
         **/
        void read();

        /**
         * Write the bookmarks to a plain text file
         * ~/.config/QDirStat/bookmarks.txt
         **/
        void write();

        /**
         * Rebuild the bookmarks menu from the current bookmarks.
         **/
        void rebuildBookmarksMenu();

        /**
         * Return the full path of the bookmarks file.
         *
         * Unfortunately, Qt's QFile does not understand standard
         * Linux paths like "~/.config/QDirStat/bookmarks.txt",
         * it needs a full path for "~".
         **/
        static QString bookmarksFileName();


    protected slots:

        /**
         * Notification that the user triggered a bookmark action.
         **/
        void menuActionTriggered();


    protected:

        /**
         * Sort the bookmarks alphabetically ascending.
         **/
        void sort();


        //
        // Data members
        //

        QStringList _bookmarks;
        QString     _baseUrl;
        QMenu *     _bookmarksMenu;
        bool        _dirty;
    };

} // namespace QDirStat

#endif // BookmarksManager_h
