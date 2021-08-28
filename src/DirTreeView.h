/*
 *   File name: DirTreeView.h
 *   Summary:	Tree view widget for directory tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef DirTreeView_h
#define DirTreeView_h


#include <QTreeView>

class QAction;


namespace QDirStat
{
    class DirTreePercentBarDelegate;
    class SizeColDelegate;
    class HeaderTweaker;
    class SelectionModelProxy;
    class CleanupCollection;
    class FileInfo;


    /**
     * Tree view widget for the QDirStat directory tree.
     *
     * This is a thin wrapper around TreeView that takes care about basic setup
     * and configuration of the tree view and adds support for synchronizing
     * current / selected items between the DirTree, the DirTreeMap and this
     * DirTreeView.
     *
     * The Qt model / view abstraction is kept up as good as possible, but this
     * widget is really meant to be used with a QDirStat::DirTreeModel and not
     * just any random subclass of QAbstractItemModel.
     **/
    class DirTreeView: public QTreeView
    {
	Q_OBJECT

    public:

	/**
	 * Constructor
	 **/
	DirTreeView( QWidget * parent = 0 );

	/**
	 * Destructor
	 **/
	virtual ~DirTreeView();

	/**
	 * Set the cleanup collection. If set, all cleanup actions from that
	 * collection will be added to the item context menu.
	 **/
	void setCleanupCollection( CleanupCollection * collection )
	    { _cleanupCollection = collection; }

	/**
	 * Return the cleanup collection or 0 if it is not set.
	 **/
	CleanupCollection * cleanupCollection() const { return _cleanupCollection; }

	/**
	 * Return the list of items that are currently expanded.
	 *
	 * This is well-known in the base class, but due to poor design that
	 * information is not accessible in derived classes. Well, I got
	 * creative.
	 *
	 * This method being public is more to spite the "masterminds" who in
	 * their infinite wisdom considered this information not worthy of
	 * being exported to the public because "it doesn't kneed to
	 * know". WTF?
	 **/
	QModelIndexList expandedIndexes() const;

	/**
	 * Return this view's header tweaker.
	 **/
	HeaderTweaker * headerTweaker() const { return _headerTweaker; }

        /**
         * Expand or collapse an item based on a FileInfo pointer.
         **/
        void setExpanded( FileInfo * item, bool expanded = true );


    public slots:

	/**
	 * Close (collapse) all branches except the one that 'branch' is in.
	 **/
	void closeAllExcept( const QModelIndex & branch );


    protected slots:

	/**
	 * Post a context menu for the item at 'pos'.
	 **/
	void contextMenu( const QPoint & pos );


    protected:

	/**
	 * Post the common context menu with actions (cleanup and other) for an item.
	 **/
	void actionContextMenu( const QPoint & pos );

	/**
	 * Post the context menu for the size column for the item with model
	 * index 'index'.
	 **/
	void sizeContextMenu( const QPoint & pos, const QModelIndex & index );


	/**
	 * Change the current item. Overwritten from QTreeView to make sure
	 * the branch of the new current item is expanded and scrolled to
	 * the visible area.
	 **/
	virtual void currentChanged( const QModelIndex & current,
				     const QModelIndex & oldCurrent ) Q_DECL_OVERRIDE;

        /**
         * Keyboard event handler.
         *
         * Reimplemented from QTreeView.
         **/
        virtual void keyPressEvent( QKeyEvent * event ) Q_DECL_OVERRIDE;

        /**
         * Mouse button handler.
         *
         * Don't let QTreeView steal and misappropriate the mouse back /
         * forward buttons; we want consistent history buttons throughout the
         * application.
         *
         * Reimplemented from QTreeView.
         **/
        virtual void mousePressEvent( QMouseEvent * event ) Q_DECL_OVERRIDE;


	// Data members

	DirTreePercentBarDelegate * _percentBarDelegate;
        SizeColDelegate *           _sizeColDelegate;
	HeaderTweaker *             _headerTweaker;
	CleanupCollection *         _cleanupCollection;

    };	// class DirTreeView

}	// namespace QDirStat


#endif	// DirTreeView_h
