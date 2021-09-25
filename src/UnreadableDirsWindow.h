/*
 *   File name: UnreadableDirsWindow.h
 *   Summary:	QDirStat "locate files" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef UnreadableDirsWindow_h
#define UnreadableDirsWindow_h

#include <QDialog>
#include <QPointer>
#include <QTreeWidgetItem>

#include "ui_unreadable-dirs-window.h"
#include "FileInfo.h"
#include "FileInfoSet.h"
#include "Subtree.h"


namespace QDirStat
{
    class DirTree;
    class FileTypeStats;
    class MimeCategory;


    /**
     * Modeless dialog to display directories that could not be read when
     * reading a directory tree.
     *
     * This window shows an entry for each directory with a read error with:
     *
     *	 - complete path
     *	 - user name
     *	 - group name
     *	 - permissions in octal ("0750")
     *	 - symbolic permissions "drwxrw----"
     *
     * Upon click, the directory is located in the main window, i.e.  in the
     * main window's tree view all parent directories are opened, the directory
     * is selected, and the tree view is scrolled so that this directory is
     * visible. The directory is also highlighted in the tree map, and the
     * details panel (unless disabled) shows all available details for that
     * directory.
     *
     * This window is meant to be connected to a panel message's "Details"
     * hyperlink.
     **/
    class UnreadableDirsWindow: public QDialog
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
	 **/
	UnreadableDirsWindow( QWidget *	parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~UnreadableDirsWindow();

	/**
	 * Obtain the subtree from the last used URL or 0 if none was found.
	 **/
	const Subtree & subtree() const { return _subtree; }


        /**
         * Static method for using one shared instance of this class between
         * multiple parts of the application. This will create a new instance
         * if there is none yet (or anymore).
         *
         * Do not hold on to this pointer; the instance destroys itself when
         * the user closes the window, and then the pointer becomes invalid.
         *
         * After getting this shared instance, call populate() and show().
         **/
        static UnreadableDirsWindow * sharedInstance();

        /**
         * Convenience function for creating, populating and showing the shared
         * instance.
         **/
        static void populateSharedInstance( FileInfo * newSubtree );

        /**
         * Convenience function for closing and deleting the shared instance
         * if it is open.
         **/
        static void closeSharedInstance();


    public slots:

	/**
	 * Populate the window: Locate unreadable directories in 'subtree'.
	 *
	 * This clears the old search results first, then searches the subtree
	 * and populates the search result list with the directories could not
	 * be read.
	 **/
	void populate( FileInfo * newSubtree );

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
	 * Select one of the search results in the main window's tree and
	 * treemap widgets via their SelectionModel.
	 **/
	void selectResult( QTreeWidgetItem * item );


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
	 * Recursively find unreadable directories in a subtree and add an
	 * entry to the tree widget for each one.
	 **/
	void populateRecursive( FileInfo * subtree );


	//
	// Data members
	//

	Ui::UnreadableDirsWindow * _ui;
	Subtree			   _subtree;

        static QPointer<UnreadableDirsWindow> _sharedInstance;
    };



    /**
     * Item class for the directory list (which is really a tree widget),
     * representing one directory that could not be read.
     *
     * Notice that this item intentionally does not store a FileInfo or DirInfo
     * pointer for each search result, but its path. This is more expensive to
     * store, and the corresponding DirInfo * has to be fetched again with
     * DirTree::locate() (which is an expensive operation), but it is a lot
     * safer in case the tree is modified, i.e. if the user starts cleanup
     * operations or refreshes the tree from disk: Not only are no pointers
     * stored that might become invalid, but the search result remains valid
     * even after such an operation since the strings (the paths) will still
     * match an object in the tree in most cases.
     *
     * In the worst case, the search result won't find the corresponding
     * DirInfo * anymore (if that directory branch was deleted), but for sure
     * it will not crash.
     **/
    class UnreadableDirListItem: public QTreeWidgetItem
    {
    public:

	/**
	 * Constructor.
	 **/
	UnreadableDirListItem( const QString & path,
			       const QString & userName,
			       const QString & groupName,
			       const QString & symbolicPermissions,
			       const QString & octalPermissions	 );

	/**
	 * Return the path of this directory.
	 **/
	QString path() const { return _path; }

	/**
	 * Less-than operator for sorting.
	 **/
	virtual bool operator<( const QTreeWidgetItem & other ) const Q_DECL_OVERRIDE;

    protected:

	QString _path;
    };

} // namespace QDirStat


#endif // UnreadableDirsWindow_h
