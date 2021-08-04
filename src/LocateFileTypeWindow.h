/*
 *   File name: LocateFileTypeWindow.h
 *   Summary:	QDirStat "locate files by type" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef LocateFileTypeWindow_h
#define LocateFileTypeWindow_h

#include <QDialog>
#include <QTreeWidgetItem>

#include "ui_locate-file-type-window.h"
#include "FileInfo.h"
#include "FileInfoSet.h"
#include "Subtree.h"


namespace QDirStat
{
    class DirTree;
    class FileTypeStats;
    class MimeCategory;
    class SelectionModel;


    /**
     * Modeless dialog to display search results after clicking "locate" in the
     * file type stats window.
     *
     * This window shows a directory entry for each directory that contains
     * files of the requested type (with the requested suffix). When the user
     * clicks on a search result, that directory is opened in the QDirStat main
     * window's tree view (and all other open branches of the tree are closed),
     * and the matching files in that directory are selected.
     *
     * As a next step, the user can then start cleanup actions on those files
     * from the main window - in the tree view or in the treemap view.
     **/
    class LocateFileTypeWindow: public QDialog
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
	LocateFileTypeWindow( QWidget *	parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~LocateFileTypeWindow();


    public:

        /**
         * Obtain the subtree from the last used URL or 0 if none was found.
         **/
        const Subtree & subtree() const { return _subtree; }

	/**
	 * Return the current search suffix (with leading '*.')
	 **/
	QString searchSuffix() const;


    public slots:

	/**
	 * Populate the window: Locate files with 'suffix' in 'subtree'.
	 *
	 * This clears the old search results first, then searches the subtree
	 * and populates the search result list with the directories where
	 * matching files were found.
	 **/
	void populate( const QString & suffix, FileInfo * subtree = 0 );

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
	 * Recursively locate directories that contain files matching the
	 * search suffix and create a search result item for each one.
	 **/
	void populateRecursive( FileInfo * dir );

	/**
	 * Return all direct file children matching the current search suffix.
	 **/
	FileInfoSet matchingFiles( FileInfo * dir );


	//
	// Data members
	//

	Ui::LocateFileTypeWindow * _ui;
        Subtree                    _subtree;
	QString			   _searchSuffix;
    };


    /**
     * Column numbers for the file type tree widget
     **/
    enum SuffixSearchResultColumns
    {
	SSR_CountCol = 0,
	SSR_TotalSizeCol,
	SSR_PathCol,
	SSR_ColumnCount
    };


    /**
     * Item class for the locate list (which is really a tree widget),
     * representing one directory that contains files with the desired suffix.
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
    class SuffixSearchResultItem: public QTreeWidgetItem
    {
    public:

	/**
	 * Constructor.
	 **/
	SuffixSearchResultItem( const QString & path,
				int		count,
				FileSize	totalSize );
	//
	// Getters
	//

	QString	 path()	      const { return _path; }
	int	 count()      const { return _count; }
	FileSize totalSize()  const { return _totalSize; }

	/**
	 * Less-than operator for sorting.
	 **/
	virtual bool operator<( const QTreeWidgetItem & other ) const Q_DECL_OVERRIDE;

    protected:

	QString		_path;
	int		_count;
	FileSize	_totalSize;
    };

} // namespace QDirStat


#endif // LocateFileTypeWindow_h
