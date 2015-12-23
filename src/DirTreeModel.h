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

#include "FileInfoIterator.h"


namespace QDirStat
{
    class DirTree;
    class FileInfo;

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
	 *   col << DirTreeModel::NameCol,
	 *	 << DirTreeModel::PercentBarCol,
	 *	 << DirTreeModel::PercentNumCol,
	 *	 << DirTreeModel::TotalSizeCol;
	 *   model->setColumns( col );
	 */
	void setColumns( const DataColumnList & columns );

	/**
	 * Return the current colums that are displayed.
	 **/
	const DataColumnList & columns() const { return _colMapping; }

	/**
	 * Return the number of columns that are curently displayed.
	 **/
	int colCount() const { return _colMapping.size(); }

	/**
	 * Look up a model column in the column mapping to get the
	 * corresponding view column. This is the reverse operation to
	 * 'mappedCol()'.
	 **/
	int viewCol( int modelCol ) const;


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
	 * Debugging: Dump list of persistent model indices to the log.
	 **/
	void dumpPersistentIndexList() const;

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
         * Process notification that the sort order of a directory actually
         * changed, i.e. it had to give up its sort cache.
         **/
        void sortingChanged( DirInfo * dir );

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
	 * Return a model index for 'item' and 'column'.
	 **/
	QModelIndex modelIndex( FileInfo * item, int column = 0 ) const;

	/**
	 * Look up a column in the view in the column mapping to get the
	 * corresponding model column: Column #3 in the view might or might not
	 * be TotalSizeCol, depending on how the model is configured (what
	 * columns are to be displayed and in what order).
	 *
	 * See also setColumns().
	 **/
	int mappedCol( int viewCol ) const;

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
	 * Update the persistent indices with current row after sorting etc.
	 **/
	void updatePersistentIndices();

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

	DirTree *	_tree;
	DotEntryPolicy	_dotEntryPolicy;
	QString		_treeIconDir;
	DataColumnList	_colMapping;
	int		_readJobsCol;
	QSet<DirInfo *> _pendingUpdates;
	QTimer		_updateTimer;
	DataColumn	_sortCol;
	Qt::SortOrder	_sortOrder;


	// The various icons

	QPixmap	_dirIcon;
	QPixmap	_dotEntryIcon;
	QPixmap	_unreadableDirIcon;
	QPixmap _mountPointIcon;
	QPixmap	_fileIcon;
	QPixmap _stopIcon;
	QPixmap _excludedIcon;

    };	// class DirTreeModel

}	// namespace QDirStat

#endif // DirTreeModel_h
