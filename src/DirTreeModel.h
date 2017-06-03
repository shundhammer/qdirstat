/*
 *   File name: DirTreeModel.h
 *   Summary:	Qt data model for directory tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef DirTreeModel_h
#define DirTreeModel_h


#include <QAbstractItemModel>
#include <QPixmap>
#include <QSet>
#include <QTimer>
#include <QTextStream>

#include "DataColumns.h"
#include "FileInfo.h"


namespace QDirStat
{
    class DirTree;
    class DirInfo;
    class SelectionModel;

    enum CustomRoles
    {
	RawDataRole = Qt::UserRole
    };


    class DirTreeModel: public QAbstractItemModel
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 *
	 * 'parent' is just the parent in the QObject / QWidget hierarchy that
	 * will take of deleting this model upon shutdown. It is typically NOT
	 * the corresponding view. As a matter of fact, there might be any
	 * number of views connected.
	 **/
	DirTreeModel( QObject * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~DirTreeModel();

	/**
	 * Returns the internal @ref DirTree this view works on.
	 *
	 * Handle with caution: This might be short-lived information.	The
	 * model might choose to create a new tree shortly after returning
	 * this, so don't store this pointer internally.
	 **/
	DirTree *tree()	{ return _tree; }

	/**
	 * Set the column order and what columns to display.
	 *
	 * Example:
	 *
	 *   DataColumnList col;
	 *   col << QDirStat::NameCol,
	 *	 << QDirStat::PercentBarCol,
	 *	 << QDirStat::PercentNumCol,
	 *	 << QDirStat::TotalSizeCol;
	 *   dirTreeModel->setColumns( col );
	 */
	void setColumns( const DataColumnList & columns );

	/**
	 * This is protected in the base class, but it's the only reasonable
	 * way for the view to figure out what items are expanded: The
	 * QTreeViewPrivate knows, but IT DOESN'T TELL that secret.
	 *
	 * Trolltech, you fucked up big time here. Seriously, that QTreeView is
	 * the most limiting tree widget you ever made. It's a far cry of the
	 * Qt3 QListView: It can't do shit. No wonder all apps using it suffer
	 * from poor usability! No way to set a separate icon for expanded
	 * vs. collapsed items, no way to close all branches except the current
	 * one.
	 *
	 * I am not going to reimplement all that stuff the private class
	 * already does. Seriously, life is too short for that kind of
	 * bullshit. I am not going to do my own bookkeeping based on
	 * expanded() and collapsed() signals and always suffer from not
	 * receiving the odd one and being out of sync with lists that are kept
	 * in the private class. No fucking way.
	 *
	 * Trolls, if you ever actually used your own stuff like you did in the
	 * old days before you abandoned the X11 widgets and spent all your
	 * time and energy on that QML crap, you'd know that this QTreeView is
	 * a sorry excuse for the old QListView.
	 *
	 * Didn't it ever occur to you that if you are constantly using
	 * d->expandedIndexes in your QTreeView.cpp, derived classes might have
	 * the same need to know about that? Why hide it in the private class
	 * if it's that obvious that this is frequently needed information?
	 *
	 * It doesn't happen often in Qt - in all other aspects, it's a great,
	 * well thought-out and very practical oriented framework. But
	 * QTreeView is poorly designed in many aspects; hiding this
	 * d->expandedIndexes is just one example.
	 *
	 * OK, I'm fed up with working around this poor design. Let's make our
	 * own kludge to find out what items are expanded: Check the persisten
	 * model indexes: The private class creates a persistend model index
	 * for each item that is expanded.
	 **/
	QModelIndexList persistentIndexList() const
	    { return QAbstractItemModel::persistentIndexList(); }


    public slots:
	/**
	 * Open a directory URL.
	 **/
	void openUrl( const QString &url );

	/**
	 * Clear this view's contents.
	 **/
	void clear();

	/**
	 * Debugging: Dump list of persistent model indexes to the log.
	 **/
	void dumpPersistentIndexList() const;

	/**
	 * Read parameters from settings file.
	 **/
	void readSettings();

	/**
	 * Write parameters to settings file.
	 **/
	void writeSettings();

	/**
	 * Return this data model's selection model.
	 *
	 * The data model doesn't strictly need a selection model, but certain
	 * operations it provides (like refreshSelected()) do.
	 **/
	SelectionModel * selectionModel() const { return _selectionModel; }

	/**
	 * Set the selection model. This is required for all methods with
	 * '..Selected()' in their name.
	 **/
	void setSelectionModel( SelectionModel * selModel )
	    { _selectionModel = selModel; }

	/**
	 * Refresh the selected items: Re-read their contents from disk.
	 * This requires a selection model to be set.
	 **/
	void refreshSelected();

        /**
         * Set the update speed to slow (3 sec instead of 333 millisec).
         **/
        void setSlowUpdate( bool slow = true );

        /**
         * Return the slow update flag.
         **/
        bool slowUpdate() const { return _slowUpdate; }

    public:

	// Mapping of tree items to model rows and vice versa.
	//
	// This is in the model and not in the FileInfo class to encapsulate
	// handling of the "dot entry". In this case, it is handled as a
	// separate subdirectory. But this might change in the future, or it
	// might even become configurable.

	/**
	 * Find the child number 'childNo' among the children of 'parent'.
	 * Return 0 if not found.
	 **/
	FileInfo * findChild( DirInfo * parent, int childNo ) const;

	/**
	 * Find the row number (the index, starting with 0) of 'child' among
	 * its parent's children.
	 **/
	int rowNumber( FileInfo * child ) const;

	/**
	 * Count the direct children (including the "dot entry") of 'parent'.
	 **/
	int countDirectChildren( FileInfo * parent ) const;

	/**
	 * Return a model index for 'item' and 'column'.
	 **/
	QModelIndex modelIndex( FileInfo * item, int column = 0 ) const;

	/**
	 * Return the current sort column.
	 **/
	DataColumn sortColumn() const { return _sortCol; }

	/**
	 * Return the current sort order
	 * (Qt::AscendingOrder or Qt::DescendingOrder).
	 **/
	Qt::SortOrder sortOrder() const { return _sortOrder; }

	//
	// Reimplented from QAbstractItemModel:
	//

	/**
	 * Return the number of rows (direct tree children) for 'parent'.
	 **/
	virtual int rowCount   ( const QModelIndex & parent ) const Q_DECL_OVERRIDE;

	/**
	 * Return the number of columns for 'parent'.
	 **/
	virtual int columnCount( const QModelIndex & parent ) const Q_DECL_OVERRIDE;

	/**
	 * Return data to be displayed for the specified model index and role.
	 **/
	virtual QVariant data( const QModelIndex & index, int role ) const Q_DECL_OVERRIDE;

	/**
	 * Return header data (in this case: column header texts) for the
	 * specified section (column number).
	 **/
	virtual QVariant headerData( int	     section,
				     Qt::Orientation orientation,
				     int	     role ) const Q_DECL_OVERRIDE;

	/**
	 * Return item flags for the specified model index. This specifies if
	 * the item can be selected, edited etc.
	 **/
	virtual Qt::ItemFlags flags( const QModelIndex &index ) const Q_DECL_OVERRIDE;

	/**
	 * Return the model index for the specified row (direct tree child
	 * number) and column of item 'parent'.
	 **/
	virtual QModelIndex index( int row,
				   int column,
				   const QModelIndex & parent = QModelIndex() ) const Q_DECL_OVERRIDE;

	/**
	 * Return the parent model index of item 'index'.
	 **/
	virtual QModelIndex parent( const QModelIndex & index ) const Q_DECL_OVERRIDE;

	/**
	 * Sort the model.
	 **/
	virtual void sort( int column,
			   Qt::SortOrder order = Qt::AscendingOrder ) Q_DECL_OVERRIDE;

    protected slots:

	/**
	 * Fix up sort order while reading: Sort by read jobs if the sort
	 * column is the PercentBarCol.
	 **/
	void busyDisplay();

	/**
	 * Fix up sort order after reading is finished: No longer Sort by read
	 * jobs if the sort column is the PercentBarCol.
	 **/
	void idleDisplay();

	/**
	 * Process notification that the read job for 'dir' is finished.
	 * Other read jobs might still be pending.
	 **/
	void readJobFinished( DirInfo *dir );

	/**
	 * Process notification that reading the dir tree is completely
	 * finished.
	 **/
	void readingFinished();

	/**
	 * Delayed update of the data fields in the view for 'dir':
	 * Store 'dir' and all its ancestors in _pendingUpdates.
	 *
	 * The updates will be sent several times per second to the views with
	 * 'sendPendingUpdates()'.
	 **/
	void delayedUpdate( DirInfo * dir );

	/**
	 * Send all pending updates to the connected views.
	 * This is triggered by the update timer.
	 **/
	void sendPendingUpdates();

	/**
	 * Notification that a subtree is about to be deleted.
	 **/
	void deletingChild( FileInfo * child );

	/**
	 * Notification that deleting a subtree is done.
	 **/
	void childDeleted();

	/**
	 * Notification that a subtree is about to be cleared.
	 **/
	void clearingSubtree( DirInfo * subtree );

	/**
	 * Notification that clearing a subtree is done.
	 **/
	void subtreeCleared( DirInfo * subtree );

	/**
	 * Invalidate all persistent indexes in 'subtree'. 'includeParent'
         * indicates if 'subtree' itself will become invalid.
	 **/
	void invalidatePersistent( FileInfo * subtree,
                                   bool includeParent );


    protected:
	/**
	 * Create a new tree (and delete the old one if there is one)
	 **/
	void createTree();

	/**
	 * Load all required icons.
	 **/
	void loadIcons();

	/**
	 * Notify the view (with beginInsertRows() and endInsertRows()) about
	 * new children (all the children of 'dir'). This might become
	 * recursive if any of those children in turn are already finished.
	 **/
	void newChildrenNotify( DirInfo * dir );

	/**
	 * Notify the view about changed data of 'dir'.
	 **/
	void dataChangedNotify( DirInfo * dir );

	/**
	 * Update the persistent indexes with current row after sorting etc.
	 **/
	void updatePersistentIndexes();

	/**
	 * Return 'true' if 'item' or any ancestor (parent or parent's parent
	 * etc.) is still busy, i.e. the read job for the directory itself (not
	 * any children!) is still queued or currently reading.
	 **/
	bool anyAncestorBusy( FileInfo * item ) const;

	/**
	 * Return the text for (model) column 'col' for 'item'.
	 **/
	QVariant columnText( FileInfo * item, int col ) const;

	/**
	 * Return the icon for (model) column 'col' for 'item'.
	 **/
	QVariant columnIcon( FileInfo * item, int col ) const;

	/**
	 * Return the text for the own size column for 'item'
	 **/
	QVariant ownSizeColText( FileInfo * item ) const;

	/**
	 * Format a percentage value as string if it is non-negative.
	 * Return QVariant() if it is negative.
	 **/
	QVariant formatPercent( float percent ) const;


	//
	// Data members
	//

	DirTree *	 _tree;
	SelectionModel * _selectionModel;
	QString		 _treeIconDir;
	int		 _readJobsCol;
	QSet<DirInfo *>	 _pendingUpdates;
	QTimer		 _updateTimer;
	int		 _updateTimerMillisec;
        int              _slowUpdateMillisec;
        bool             _slowUpdate;
	DataColumn	 _sortCol;
	Qt::SortOrder	 _sortOrder;


	// The various icons

	QPixmap	_dirIcon;
	QPixmap	_dotEntryIcon;
	QPixmap	_fileIcon;
	QPixmap _symlinkIcon;
	QPixmap	_unreadableDirIcon;
	QPixmap	_unreadableFileIcon;
	QPixmap _mountPointIcon;
	QPixmap _stopIcon;
	QPixmap _excludedIcon;
	QPixmap _blockDeviceIcon;
	QPixmap _charDeviceIcon;
	QPixmap _specialIcon;

    };	// class DirTreeModel


    /**
     * Print a QModelIndex of this model in text form to a debug stream.
     **/
    inline QTextStream & operator<< ( QTextStream & stream, const QModelIndex & index )
    {
	if ( ! index.isValid() )
	    stream << "<Invalid QModelIndex>";
	else
	{
	    FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
	    stream << "<QModelIndex row: " << index.row()
		   << " col: " << index.column();

	    if ( item && ! item->checkMagicNumber() )
		stream << " <INVALID FileInfo *>";
	    else
		stream << " " << item;

	    stream << " >";
	}

	return stream;
    }


}	// namespace QDirStat

#endif // DirTreeModel_h
