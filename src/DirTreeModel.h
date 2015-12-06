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


namespace QDirStat
{
    class DirTree;
    class FileInfo;

    class DirTreeModel: public QAbstractItemModel
    {
	Q_OBJECT

    public:
	/**
	 * Model columns
	 **/
	enum Column
	{
	    NameCol	   = 0, // File / directory name
	    PercentBarCol,	// Graphical percentage bar
	    PercentNumCol,	// Numeric percentage Value
	    TotalSizeCol,	// Total subtree size
	    OwnSizeCol,		// Own size of file / directory
	    TotalItemsCol,	// Total number of items    in subtree
	    TotalFilesCol,	// Total number of files    in subtree
	    TotalSubDirsCol,	// Total number of subdirs  in subtree
	    LatestMTimeCol,	// Latest modification time in subtree
	    ColCount
	};


	// Not yet handled:
	//   WorkingStatusCol,
	//   ReadJobsCol,
	//   SortCol

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
	 * Handle with caution: This might be short-lived information.
	 * The model might choose to create a new tree shortly after returning
	 * this, so don't store this pointer internally.
	 **/
	DirTree *tree()	{ return _tree; }

	/**
	 * Set the column order and what columns to display.
	 *
	 * Example:
	 *
	 *   QList<DirTreeModel::Column> col;
	 *   col << DirTreeModel::NameCol,
	 *	 << DirTreeModel::PercentBarCol,
	 *	 << DirTreeModel::PercentNumCol,
	 *	 << DirTreeModel::TotalSizeCol;
	 *   model->setColumns( col );
	 */
	void setColumns( QList<Column> columns );

	/**
	 * Return the current colums that are displayed.
	 **/
	QList<Column> columns() const { return _colMapping; }

	/**
	 * Return the number of columns that are curently displayed.
	 **/
	int colCount() const { return _colMapping.size(); }


    public slots:
	/**
	 * Open a directory URL. Assume "file:" protocol unless otherwise specified.
	 **/
	void openUrl( const QString &url );

	/**
	 * Clear this view's contents.
	 **/
	void clear();

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
        FileInfo * findChild( FileInfo * parent, int childNo ) const;

        /**
         * Find the index (the number, starting with 0) of 'child' among its
         * parent's children.
         **/
        int childIndex( FileInfo * child ) const;

        /**
         * Count the direct children (including the "dot entry") of 'parent'.
         **/
        int countDirectChildren( FileInfo * parent ) const;


        //
        // Icons
        //

	QPixmap	openDirIcon()		const	{ return _openDirIcon;		}
	QPixmap	closedDirIcon()		const	{ return _closedDirIcon;	}
	QPixmap	openDotEntryIcon()	const	{ return _openDotEntryIcon;	}
	QPixmap	closedDotEntryIcon()	const	{ return _closedDotEntryIcon;	}
	QPixmap	unreadableDirIcon()	const	{ return _unreadableDirIcon;	}
	QPixmap mountPointIcon()	const	{ return _mountPointIcon;	}
	QPixmap	fileIcon()		const	{ return _fileIcon;		}
	QPixmap	symLinkIcon()		const	{ return _symLinkIcon;		}
	QPixmap blockDevIcon()		const	{ return _blockDevIcon;		}
	QPixmap charDevIcon()		const	{ return _charDevIcon;		}
	QPixmap fifoIcon()		const	{ return _fifoIcon;		}
	QPixmap stopIcon()		const	{ return _stopIcon;		}
	QPixmap	workingIcon()		const	{ return _workingIcon;		}

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
	virtual QVariant data  ( const QModelIndex & index, int role ) const Q_DECL_OVERRIDE;

        /**
         * Return header data (in this case: column header texts) for the
         * specified section (column number).
         **/
	virtual QVariant headerData( int             section,
                                     Qt::Orientation orientation,
                                     int             role ) const Q_DECL_OVERRIDE;

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
	 * Look up a column in the view in the column mapping to get the
	 * corresponding model column: Column #3 in the view might or might not
	 * be TotalSizeCol, depending on how the model is configured (what
	 * columns are to be displayed and in what order).
	 *
	 * See also setColumns().
	 **/
	int mappedCol( unsigned viewCol ) const;

        /**
         * Return the text for (model) column 'col' for dir tree item 'item'.
         **/
        QVariant columnText( FileInfo * item, int col ) const;


	//
	// Data members
	//

	DirTree *	_tree;
	QList<Column>	_colMapping;

	// The various icons

	QPixmap	_openDirIcon;
	QPixmap	_closedDirIcon;
	QPixmap	_openDotEntryIcon;
	QPixmap	_closedDotEntryIcon;
	QPixmap	_unreadableDirIcon;
	QPixmap _mountPointIcon;
	QPixmap	_fileIcon;
	QPixmap	_symLinkIcon;
	QPixmap _blockDevIcon;
	QPixmap _charDevIcon;
	QPixmap _fifoIcon;
	QPixmap _stopIcon;
	QPixmap	_workingIcon;

    };	// class DirTreeModel

}	// namespace QDirStat

#endif // DirTreeModel_h
