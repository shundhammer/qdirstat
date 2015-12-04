/*
 *   File name:	kdirtreeview.h
 *   Summary:	High level classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifndef KDirTreeView_h
#define KDirTreeView_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <qdatetime.h>
#include <qlistview.h>
#include <qpixmap.h>
#include <klistview.h>
#include "kdirtree.h"

#define DEBUG_COUNTERS		10


// Forward declarations
class QWidget;
class QTimer;
class QPopupMenu;
class KPacManAnimation;

#define USE_KLISTVIEW 0


// Open a new name space since KDE's name space is pretty much cluttered
// already - all names that would even remotely match are already used up,
// yet the resprective classes don't quite fit the purposes required here.

namespace KDirStat
{
#define KDirTreeViewMaxFillColor	16


#if USE_KLISTVIEW
#   define KDirTreeViewParentClass		KListView
#else
#   define KDirTreeViewParentClass		QListView
#endif

    class KDirTreeViewItem;


    class KDirTreeView:	public QListView
    // Using
    //		class KDirTreeView: public KDirTreeViewParentClass
    // or some other 'ifdef' ... construct seems to confuse "moc".
    {
	Q_OBJECT

    public:
	/**
	 * Default constructor.
	 **/
	KDirTreeView( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~KDirTreeView();

	/**
	 * Locate the counterpart to an original tree item "wanted" somewhere
	 * within this view tree. Returns 0 on failure.
	 * If "lazy" is set, only the open part of the tree is searched.
	 * "doClone" specifies whether or not to (deferred) clone nodes that
	 * are not cloned yet. This is only used if "lazy" is false.
	 **/
	KDirTreeViewItem *	locate( KFileInfo *	wanted,
					bool		lazy	= true,
					bool		doClone	= true );

	/**
	 * Get the first child of this view or 0 if there is none.
	 * Use the child's next() method to get the next child.
	 * Reimplemented from @ref QListView.
	 **/
	KDirTreeViewItem *	firstChild() const
	    { return (KDirTreeViewItem *) KDirTreeViewParentClass::firstChild(); }

	/**
	 * Return the currently selected item or 0, if none is selected.
	 **/
	KDirTreeViewItem *	selection() const { return _selection; }

	/**
	 * Returns the default level until which items are opened by default
	 * (unless they are dot entries).
	 **/
	int	openLevel()		const	{ return _openLevel;		}

	/**
	 * Returns true if the view tree is to be cloned lazily, i.e. only
	 * those view tree branches that are really visible are synced with the
	 * original tree.
	 **/
	bool	doLazyClone()		const	{ return _doLazyClone;		}

	/**
	 * Enable / disable PacMan animation in this tree view during directory
	 * reading. This is disabled by default since it eats quite some
	 * performance.
	 **/
	void	enablePacManAnimation( bool enable ) { _doPacManAnimation = enable; }
	/**
	 * Returns true if the PacMan animation is to be used during directory
	 * reading.
	 **/
	bool	doPacManAnimation()	const	{ return _doPacManAnimation;	}

	/**
	 * Returns the number of open items in the entire tree.
	 **/
	int	openCount();

	/**
	 * Return the percentage bar fill color for the specified directory
	 * level (0..MaxInt). Wraps around every usedFillColors() colors.
	 **/
	const QColor &	fillColor( int level ) const;

	/**
	 * Very much like @ref fillColor(), but doesn't wrap around at @ref
	 * usedFillColors(), but at KDirTreeViewMaxFillColor.
	 **/
	const QColor &	rawFillColor( int level ) const;

	/**
	 * Set the fill color of percentage bars of the specified directory
	 * level (0..KDirTreeViewMaxFillColor-1).
	 *
	 * Calling repaint() after setting all desired colors is the
	 * caller's responsibility.
	 **/
	void setFillColor( int level, const QColor &color );

	/**
	 * Set all tree colors to default values.
	 **/
	void setDefaultFillColors();

	/**
	 * Set the number of used percentage bar fill colors
	 * (1..KDirTreeViewMaxFillColor).
	 **/
	void setUsedFillColors( int usedFillColors );

	/**
	 * Returns the number of used percentage bar fill colors.
	 **/
	int usedFillColors()	const	{ return _usedFillColors;	}

	/**
	 * Set the tree background color.
	 *
	 * Calling repaint() after setting all desired colors is the
	 * caller's responsibility.
	 **/
	void setTreeBackground( const QColor &color );

	/**
	 * Returns the tree background color.
	 **/
	const QColor &	treeBackground()		const	{ return _treeBackground;	}

	/**
	 * Returns the background color for percentage bars.
	 **/
	const QColor &	percentageBarBackground()	const	{ return _percentageBarBackground; }

	/**
	 * (Try to) ensure good contrast between the tree background and the
	 * percentage bars' 3D edges - prevent ugly 3D effects which will
	 * inevitably be the case for a white background (which unfortunately
	 * is very common): The percentage bars use white and black for 3D
	 * borders - like any other widget. But other widgets normally can
	 * assume their parent widget uses some more neutral color so white and
	 * black will result in at least some minimal contrast.
	 *
	 * This function automagically sets a reasonable default background
	 * color for the tree display: If the current color scheme's document
	 * background color (as used for input fields, lists etc.) is white or
	 * black, use the palette midlight color (the same color as "normal"
	 * widgets like push buttons etc., but brighter). For all other colors
	 * than white, the document background color (the palette base color)
	 * is used.
	 **/
	void ensureContrast();

	/**
	 * Set the sort column.
	 *
	 * Reimplemented from QListView so we can keep track of the sort column.
	 **/
	virtual void setSorting( int column, bool increasing = TRUE );

	/**
	 * Returns the internal @ref KDirTree this view works on.
	 * Handle with caution: This might be short-lived information.
	 * The view might choose to create a new tree shortly after returning
	 * this, so don't store this pointer internally.
	 **/
	KDirTree *tree()			{ return _tree; }

	int	nameCol()		const	{ return _nameCol;		}
	int	iconCol()		const	{ return _iconCol;		}
	int	percentBarCol()		const	{ return _percentBarCol;	}
	int	percentNumCol()		const	{ return _percentNumCol;	}
	int	totalSizeCol()		const	{ return _totalSizeCol;		}
	int	workingStatusCol()	const	{ return _workingStatusCol;	}
	int	ownSizeCol()		const	{ return _ownSizeCol;		}
	int	totalItemsCol()		const	{ return _totalItemsCol;	}
	int	totalFilesCol()		const	{ return _totalFilesCol;	}
	int	totalSubDirsCol()	const	{ return _totalSubDirsCol;	}
	int	latestMtimeCol()	const	{ return _latestMtimeCol;	}
	int	readJobsCol()		const	{ return _readJobsCol;		}
	int	sortCol()		const   { return _sortCol;		}

	QPixmap	openDirIcon()		const	{ return _openDirIcon;		}
	QPixmap	closedDirIcon()		const	{ return _closedDirIcon;	}
	QPixmap	openDotEntryIcon()	const	{ return _openDotEntryIcon;	}
	QPixmap	closedDotEntryIcon()	const	{ return _closedDotEntryIcon;	}
	QPixmap	unreadableDirIcon()	const	{ return _unreadableDirIcon;	}
	QPixmap mountPointIcon()	const	{ return _mountPointIcon;	}
	QPixmap	fileIcon()		const	{ return _fileIcon;		}
	QPixmap	symLinkIcon()		const	{ return _symLinkIcon;		}
	QPixmap blockDevIcon()		const 	{ return _blockDevIcon;		}
	QPixmap charDevIcon()		const 	{ return _charDevIcon;		}
	QPixmap fifoIcon()		const 	{ return _fifoIcon;		}
	QPixmap stopIcon()		const 	{ return _stopIcon;		}
	QPixmap	workingIcon()		const	{ return _workingIcon;		}
	QPixmap	readyIcon()		const	{ return _readyIcon;		}


	/**
	 * Set function name of debug function #i
	 **/
	void	setDebugFunc( int i, const QString & functionName );

	/**
	 * Increase debug counter #i
	 **/
	void	incDebugCount( int i );


    public slots:

	/**
	 * Open a directory URL. Assume "file:" protocol unless otherwise specified.
	 **/
	void openURL( KURL url );

	/**
	 * Refresh (i.e. re-read from disk) the entire tree.
	 **/
	void refreshAll();

	/**
	 * Refresh (i.e. re-read from disk) the selected subtree.
	 **/
	void refreshSelected();

	/**
	 * Forcefully stop a running read process.
	 **/
	void abortReading();

	/**
	 * Clear this view's contents.
	 **/
	void clear();

        /**
	 * Select a (QListViewItem) item. Triggers selectionChanged() signals.
	 **/
        void selectItem( QListViewItem *item );

        /**
	 * Select an item. Triggers selectionChanged() signals.
	 * Overloaded for convenience.
	 **/
        void selectItem( KDirTreeViewItem *item ) { selectItem( (QListViewItem *) item ); }

	/**
	 * Select a KDirTree item. Used for connecting the @ref
	 * KDirTree::selectionChanged() signal.
	 **/
	void selectItem( KFileInfo *item );

	/**
	 * Clear the current selection. Triggers selectionChanged() signals.
	 **/
	void clearSelection();

	/**
	 * Close all tree branches except the one specified.
	 **/
	void closeAllExcept( KDirTreeViewItem *except );

	/**
	 * Send a standardized mail to the owner of the selected branch.
	 * The user will get a mailer window where he can edit that mail all he
	 * likes before deciding to send or discard it.
	 *
	 * The mail includes all currently open branches from the selected
	 * branch on.
	 **/
	void sendMailToOwner();

	/**
	 * Notification of a change in the KDE palette, i.e. the user selected
	 * and applied different colors in the KDE control center.
	 **/
	void paletteChanged();

	/**
	 * Read configuration and initialize variables accordingly.
	 * Will be called automatically in the constructor.
	 **/
	void readConfig();

	/**
	 * Save configuraton.
	 **/
	void saveConfig() const;

	/**
	 * Emit a @ref userActivity() signal worth 'points' activity points.
	 **/
	void logActivity( int points );

	/**
	 * Returns the minimum recommended size for this widget.
	 * Reimplemented from QWidget.
	 **/
	virtual QSize minimumSizeHint() const { return QSize( 0, 0 ); }

	/**
	 * Write the current tree to a cache file.
	 *
	 * Returns true if OK, false upon error.
	 **/
	bool writeCache( const QString & cacheFileName );

	/**
	 * Read a cache file.
	 **/
	void readCache( const QString & cacheFileName );


    protected slots:

	/**
	 * Add a child as a clone of original tree item "newChild" to this view
	 * tree.
	 **/
	void	addChild	( KFileInfo *newChild );

	/**
	 * Delete a cloned child.
	 **/
	void	deleteChild	( KFileInfo *newChild );

	/**
	 * Recursively update the visual representation of the summary fields.
	 * This update is as lazy as possible for optimum performance since it
	 * is called very frequently as a cyclic update.
	 **/
	void	updateSummary();

	/**
	 * Signal end of all read jobs, finalize display and terminate pending
	 * cyclic visual update.
	 **/
	void	slotFinished();

	/**
	 * Signal abortion of all read jobs, finalize display and terminate pending
	 * cyclic visual update.
	 **/
	void	slotAborted();

	/**
	 * Signal end of one read job at this level and finalize display of
	 * this level.
	 **/
	void	finalizeLocal( KDirInfo *dir );

	/**
	 * Display progress information in the status bar. Automatically adds
	 * the elapsed time of a directory scan.
	 **/
	void	sendProgressInfo( const QString & currentDir = "" );


#if QT_VERSION < 300
	/**
	 * "moc" doesnt't seem to handle default arguments well, so this is an
	 * overloaded slot that uses the internally stored current directory.
	 **/
	void	sendProgressInfo();
#endif

        /**
	 * Set up everything prior to reading: Cyclic update timer, display
	 * busy state, default sorting, stopwatch.
	 **/
        void	prepareReading();

	/**
	 * Change the tree display to "busy" state, i.e. add a column to
	 * display the number of pending read jobs for each level.
	 **/
	void	busyDisplay();

	/**
	 * Change the tree display back to "idle" state, i.e. remove columns
	 * that are useful only while directories are being read, like the
	 * pending read jobs column.
	 **/
	void	idleDisplay();

	/**
	 * Pop up context menu (i.e. emit the contextMenu() signal) or open a
	 * small info popup with exact information, depending on 'column'.
	 **/
	void 	popupContextMenu	( QListViewItem *	listViewItem,
					  const QPoint &	pos,
					  int 			column );

	/**
	 * Pop up info window with exact byte size.
	 **/
	void 	popupContextSizeInfo	( const QPoint &	pos,
					  KFileSize		size );

	/**
	 * Pop up info window with arbitrary one-line text.
	 **/
	void 	popupContextInfo	( const QPoint &	pos,
					  const QString & 	info );


	/**
	 * Notification that a column has just been resized, thus may need
	 * repaining.
	 **/
        void columnResized( int column, int oldSize, int newSize );


    signals:

	/**
	 * Single line progress information, emitted when the read status
	 * changes - typically when a new directory is being read. Connect to a
	 * status bar etc. to keep the user busy.
	 **/
	void progressInfo( const QString &infoLine );

	/**
	 * Emitted when reading is started.
	 **/
	void startingReading();

	/**
	 * Emitted when reading this tree is finished.
	 **/
	void finished();

	/**
	 * Emitted when reading this tree has been aborted.
	 **/
	void aborted();

	/**
	 * Emitted when the currently selected item changes.
	 * Caution: 'item' may be 0 when the selection is cleared.
	 **/
	void selectionChanged( KDirTreeViewItem *item );

	/**
	 * Emitted when the currently selected item changes.
	 * Caution: 'item' may be 0 when the selection is cleared.
	 **/
	void selectionChanged( KFileInfo *item );

	/**
	 * Emitted when a context menu for this item should be opened.
	 * (usually on right click). 'pos' contains the click's mouse
	 * coordinates.
	 *
	 * NOTE:
	 *
	 * This is _not_ the same as @ref QListView::rightButtonClicked():
	 * The context menu may not open on a right click on every column,
	 * usually only in the nameCol().
	 **/
	void contextMenu( KDirTreeViewItem *item, const QPoint &pos );

	/**
	 * Emitted at user activity. Some interactive actions are assigned an
	 * amount of "activity points" that can be used to judge whether or not
	 * the user is actually using this program or if it's just idly sitting
	 * around on the desktop. This is intended for use together with a @ref
	 * KActivityTracker.
	 **/
	void userActivity( int points );


    protected:

	/**
	 * Create a new tree (and delete the old one if there is one)
	 **/
	void createTree();

	//
	// Data members
	//

	KDirTree *		_tree;
	QTimer *		_updateTimer;
	QTime			_stopWatch;
	QString			_currentDir;
	KDirTreeViewItem *	_selection;
	QPopupMenu *		_contextInfo;
	int			_idContextInfo;

	int	_openLevel;
	bool	_doLazyClone;
	bool	_doPacManAnimation;
	int	_updateInterval;	// millisec
	int	_usedFillColors;
	QColor	_fillColor [ KDirTreeViewMaxFillColor ];
	QColor	_treeBackground;
	QColor	_percentageBarBackground;


	// The various columns in which to display information

	int	_nameCol;
	int	_iconCol;
	int	_percentNumCol;
	int	_percentBarCol;
	int	_totalSizeCol;
	int	_workingStatusCol;
	int	_ownSizeCol;
	int	_totalItemsCol;
	int	_totalFilesCol;
	int	_totalSubDirsCol;
	int	_latestMtimeCol;
	int	_readJobsCol;
	int	_sortCol;

	int 	_debugCount[ DEBUG_COUNTERS ];
	QString	_debugFunc [ DEBUG_COUNTERS ];


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
	QPixmap	_readyIcon;
    };



    class KDirTreeViewItem: public QListViewItem
    {
    public:
	/**
	 * Constructor for the root item.
	 **/
	KDirTreeViewItem	( KDirTreeView *	view,
				  KFileInfo *		orig );

	/**
	 * Constructor for all other items.
	 **/
	KDirTreeViewItem	( KDirTreeView *	view,
				  KDirTreeViewItem *	parent,
				  KFileInfo *		orig );

	/**
	 * Destructor.
	 **/
	virtual ~KDirTreeViewItem();

	/**
	 * Locate the counterpart to an original tree item "wanted" somewhere
	 * within this view tree. Returns 0 on failure.
	 *
	 * When "lazy" is set, only the open part of the tree is searched.
	 * "doClone" specifies whether or not to (deferred) clone nodes that
	 * are not cloned yet. This is only used if "lazy" is false.
	 * "Level" is just a hint for the current tree level for better
	 * performance. It will be calculated automatically if omitted.
	 **/
	KDirTreeViewItem *	locate( KFileInfo *	wanted,
					bool 		lazy	= true,
					bool		doClone	= true,
					int 		level	= -1 );

	/**
	 * Recursively update the visual representation of the summary fields.
	 * This update is as lazy as possible for optimum performance.
	 **/
	void			updateSummary();

	/**
	 * Bring (the top level of) this branch of the view tree in sync with
	 * the original tree. Does _not_ recurse into subdirectories - only
	 * this level of this branch is processed. Called when lazy tree
	 * cloning is in effect and this branch is about to be opened.
	 **/
	void			deferredClone();


	/**
	 * Finalize this level - clean up unneeded / undesired dot entries.
	 **/
	void			finalizeLocal();

	/**
	 * Returns the corresponding view.
	 **/
	KDirTreeView *		view()		{ return _view; 	}


	/**
	 * Returns the parent view item or 0 if this is the root.
	 **/
	KDirTreeViewItem *	parent()	{ return _parent;	}

	/**
	 * Returns the corresponding original item of the "real" (vs. view)
	 * tree where all the important information resides.
	 **/
	KFileInfo *		orig()		{ return _orig;	}

	/**
	 * Returns the first child of this item or 0 if there is none.
	 * Use the child's next() method to get the next child.
	 * Reimplemented from @ref QListViewItem.
	 **/
	KDirTreeViewItem * 	firstChild() const
	    { return (KDirTreeViewItem *) QListViewItem::firstChild(); }

	/**
	 * Returns the next sibling of this item or 0 if there is none.
	 * (Kind of) reimplemented from @ref QListViewItem.
	 **/
	KDirTreeViewItem * 	next() const
	    { return (KDirTreeViewItem *) QListViewItem::nextSibling(); }

	/**
	 * Comparison function used for sorting the list.
	 *
	 * Using this function is much more efficient than overwriting
	 * QListViewItem::key() which operates on QStrings.
	 *
	 * Returns:
	 * -1 if this <  other
	 *  0 if this == other
	 * +1 if this >  other
	 *
	 * Reimplemented from QListViewItem
	 **/
	virtual int compare( QListViewItem *	other,
			     int		col,
			     bool		ascending ) const;

	/**
	 * Perform any necessary pending updates when a branch is opened.
	 * Reimplemented from @ref QListViewItem.
	 **/
	virtual void setOpen( bool open );

	/**
	 * Notification that a branch in this subtree has been opened or close
	 * somewhere. Don't call this if the state hasn't really changed!
	 **/
	void openNotify( bool open );

	/**
	 * Recursively open this subtree and all its ancestors up to the root.
	 **/
	void openSubtree();

	/**
	 * Recursively close all tree branches from here on downwards.
	 **/
	void closeSubtree();

	/**
	 * Close all tree branches except this one from the root on.
	 **/
	void closeAllExceptThis();

	/**
	 * Returns the number of open items in this subtree.
	 **/
	int openCount()		const	{ return _openCount; }

	/**
	 * Recursively return an ASCII representation of all open items from
	 * here on.
	 **/
	QString asciiDump();


    protected:

	/**
	 * Set the appropriate icon depending on this item's type and open /
	 * closed state.
	 **/
	void	setIcon();

	/**
	 * Remove dot entry if it doesn't have any children.
	 * Reparent all of the dot entry's children if there are no
	 * subdirectories on this level.
	 **/
	void	cleanupDotEntries();

	/**
	 * Find this entry's dot entry (clone).
	 *
	 * This doesn't create one if deferred cloning is in effect (which is
	 * not a drawback since cloning directory nodes create a dot entry
	 * clone along with the directory clone).
	 *
	 * Returns 0 if there is no dot entry clone.
	 **/
	KDirTreeViewItem * findDotEntry() const;


	/**
	 * Paint method. Reimplemented from @ref QListViewItem so different
	 * colors can be used - and of course for painting percentage bars.
	 *
	 * Reimplemented from @ref QListViewItem.
	 **/
	virtual void paintCell 	( QPainter *		painter,
				  const QColorGroup &	colorGroup,
				  int			column,
				  int			width,
				  int			alignment );

	/**
	 * Paint a percentage bar into a @ref QListViewItem cell.
	 * 'width' is the width of the entire cell.
	 * 'indent' is the number of pixels to indent the bar.
	 **/
	void paintPercentageBar	( float			percent,
				  QPainter *		painter,
				  int			indent,
				  int			width,
				  const QColor &	fillColor,
				  const QColor &	barBackground	);

	/**
	 * Generic comparison function.
	 **/
	template<typename T> inline
	int compare( T a, T b ) const
	{
	    if ( a < b ) return -1;
	    if ( a > b ) return  1;
	    return 0;
	}

    private:

	/**
	 * Initializations common to all constructors.
	 **/
	void init	( KDirTreeView *	view,
			  KDirTreeViewItem *	parent,
			  KFileInfo *		orig );

    protected:

	// Data members

	KDirTreeView *		_view;
	KDirTreeViewItem *	_parent;
	KFileInfo *		_orig;
	KPacManAnimation *	_pacMan;
	float			_percent;
	int			_openCount;

    };


    inline kdbgstream & operator<< ( kdbgstream & stream, KDirTreeViewItem * item )
    {
	if ( item )
	{
	    if ( item->orig() )
	    {
		stream << item->orig()->debugUrl();
	    }
	    else
	    {
		stream << "<NULL orig()> " << endl;
	    }
	}
	else
	    stream << "<NULL>";

	return stream;
    }



    //----------------------------------------------------------------------
    //			       Static Functions
    //----------------------------------------------------------------------

    /**
     * Format a file size with all digits, yet human readable using the current
     * locale's thousand separator, i.e. 12,345,678 rather than 12345678
     **/
    QString formatSizeLong( KFileSize size );

    /**
     * Format a file size for use within a QListView::key() function:
     * Right-justify and fill with leading zeroes.
     **/
    QString hexKey( KFileSize size );

    /**
     * Format a millisecond granularity time human readable.
     * Milliseconds will only be inluded if 'showMilliSeconds' is true.
     **/
    QString formatTime ( long	millisec,
			 bool	showMilliSeconds = false );

    /**
     * Format counters of any kind.
     *
     * Returns an empty string if 'suppressZero' is 'true' and the value of
     * 'count' is 0.
     **/
    QString formatCount( int count, bool suppressZero = false );

    /**
     * Format percentages.
     **/
    QString formatPercent( float percent );

    /**
     * Format time and date human-readable as "yyyy-mm-dd hh:mm:ss"
     * - unlike that ctime() crap that is really useless.
     * See the source for more about why this format.
     **/
    QString formatTimeDate( time_t rawTime );

    /**
     * Format time and date according to the current locale for those who
     * really must have that brain-dead ctime() format.
     **/
    QString localeTimeDate( time_t rawTime );

    /**
     * Return a color that contrasts to 'contrastColor'.
     **/
    QColor contrastingColor ( const QColor &desiredColor,
			      const QColor &contrastColor );

}	// namespace KDirStat


#endif // ifndef KDirTreeView_h


// EOF
