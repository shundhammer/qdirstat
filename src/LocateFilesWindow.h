/*
 *   File name: LocateFilesWindow.h
 *   Summary:	QDirStat "locate files" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef LocateFilesWindow_h
#define LocateFilesWindow_h

#include <QDialog>
#include <QTreeWidgetItem>

#include "ui_locate-files-window.h"
#include "FileInfo.h"
#include "Subtree.h"


namespace QDirStat
{
    class TreeWalker;


    /**
     * Modeless dialog to display search results for "discover" actions.
     *
     * This window shows a file with its complete path. When the user clicks on
     * a search result, that file is located in the QDirStat main window's tree
     * view; its directory branch is opened, and all other open branches of the
     * tree are closed; very much like when clicking on a treemap tile.
     *
     * As a next step, the user can then start cleanup actions on those files
     * from the main window - in the tree view or in the treemap view.
     **/
    class LocateFilesWindow: public QDialog
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 *
	 * Notice that this widget will destroy itself upon window close.
	 *
	 * It is advised to use a QPointer for storing a pointer to an instance
	 * of this class. The QPointer will keep track of this window
	 * auto-deleting itself when closed.
         *
         * This class takes over ownership of the TreeWalker and will delete it
         * when appropriate.
	 **/
	LocateFilesWindow( TreeWalker * treeWalker,
                           QWidget    *	parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~LocateFilesWindow();


        /**
         * Obtain the subtree from the last used URL or 0 if none was found.
         **/
        const Subtree & subtree() const { return _subtree; }

        /**
         * Return the TreeWalker of this window.
         **/
        TreeWalker * treeWalker() const { return _treeWalker; }

        /**
         * Set a new TreeWalker for this window. This deletes the old one.
         **/
        void setTreeWalker( TreeWalker * newTreeWalker );

        /**
         * Set a text for the window heading (the bold label above the list).
         **/
        void setHeading( const QString & text );

        /**
         * Set the sort column and sort order (Qt::AscendingOrder or
         * Qt::DescendingOrder), sort the list and select the first item.
         **/
        void sortByColumn( int col, Qt::SortOrder order );


    public slots:

	/**
	 * Populate the window: Use the TreeWalker to find matching tree items
	 * in 'subtree'.
	 *
	 * This clears the old search results first, then searches the subtree
	 * and populates the search result list with the items where
	 * TreeWalker::check() returns 'true'.
	 **/
	void populate( FileInfo * subtree = 0 );

	/**
	 * Refresh (reload) all data.
	 **/
	void refresh();

	/**
	 * Reject the dialog contents, i.e. the user clicked the "Cancel" or
	 * WM_CLOSE button. This not only closes the dialog, it also deletes
	 * it.
	 *
	 * Reimplemented from QDialog.
	 **/
	virtual void reject() Q_DECL_OVERRIDE;


    protected slots:

	/**
	 * Locate one of the items in this list results in the main window's
	 * tree and treemap widgets via their SelectionModel.
	 **/
	void locateInMainWindow( QTreeWidgetItem * item );

        /**
         * Open a context menu for an item in the results list.
         **/
        void itemContextMenu( const QPoint & pos );


    protected:

	/**
	 * Clear all data and widget contents.
	 **/
	void clear();

	/**
	 * One-time initialization of the widgets in this window.
	 **/
	void initWidgets();

        /**
         * Select the first item in the list. This will also select it in the
         * main window, open the branch where this item is in and scroll the
         * main window's tree so that item is visible tere.
         **/
        void selectFirstItem();

        /**
         * Add the hotkeys (shortcuts) of the cleanup actions to this window.
         **/
        void addCleanupHotkeys();

	/**
	 * Recursively locate directories that contain files matching the
	 * search suffix and create a search result item for each one.
	 **/
	void populateRecursive( FileInfo * dir );


	//
	// Data members
	//

	Ui::LocateFilesWindow * _ui;
        TreeWalker *            _treeWalker;
        Subtree                 _subtree;
        int                     _sortCol;
        Qt::SortOrder           _sortOrder;
    };


    /**
     * Column numbers for the file type tree widget
     **/
    enum LocateListColumns
    {
	LocateListSizeCol,
        LocateListMTimeCol,
        LocateListPathCol,
        LocateListColumnCount
    };


    /**
     * Item class for the locate list (which is really a tree widget),
     * representing one file with its path.
     *
     * Notice that this item intentionally does not store a FileInfo pointer
     * for each search result, but its path. This is more expensive to store,
     * and the corresponding DirInfo * has to be fetched again with
     * DirTree::locate() (which is an expensive operation), but it is a lot
     * safer in case the tree is modified, i.e. if the user starts cleanup
     * operations or refreshes the tree from disk: Not only are no pointers
     * stored that might become invalid, but the search result remains valid
     * even after such an operation since the strings (the paths) will still
     * match an object in the tree in most cases.
     *
     * In the worst case, the search result won't find the corresponding
     * FileInfo * anymore (if that directory branch was deleted), but for sure
     * it will not crash.
     **/
    class LocateListItem: public QTreeWidgetItem
    {
    public:

	/**
	 * Constructor.
	 **/
	LocateListItem( const QString & path,
                        FileSize	size,
                        time_t          mtime );

	//
	// Getters
	//

	QString	 path()         const { return _path;  }
	FileSize size()         const { return _size;  }
	time_t   mtime()        const { return _mtime; }

	/**
	 * Less-than operator for sorting.
	 **/
	virtual bool operator<( const QTreeWidgetItem & other ) const Q_DECL_OVERRIDE;

    protected:

	QString		_path;
	FileSize	_size;
        time_t          _mtime;
    };

} // namespace QDirStat


#endif // LocateFilesWindow_h
