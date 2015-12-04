/*
 *   File name: kdirtree.h
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2006-10-25
 */


#ifndef KDirTree_h
#define KDirTree_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <kdebug.h>
#include "kdirinfo.h"
#include "kdirreadjob.h"

#ifndef NOT_USED
#    define NOT_USED(PARAM)	( (void) (PARAM) )
#endif

// Open a new name space since KDE's name space is pretty much cluttered
// already - all names that would even remotely match are already used up,
// yet the resprective classes don't quite fit the purposes required here.

namespace KDirStat
{
    // Forward declarations
    class KDirReadJob;


    /**
     * Directory read methods.
     **/
    typedef enum
    {
	KDirReadUnknown,	// Unknown (yet)
	KDirReadLocal,		// Use opendir() and lstat()
	KDirReadKIO		// Use KDE's KIO network transparent methods
    } KDirReadMethod;



    /**
     * This class provides some infrastructure as well as global data for a
     * directory tree. It acts as the glue that holds things together: The root
     * item from which to descend into the subtree, the read queue and some
     * global policies (like whether or not to cross file systems while reading
     * directories).
     *
     * @short Directory tree global data and infrastructure
     **/
    class KDirTree: public QObject
    {
	Q_OBJECT

    public:
	/**
	 * Constructor.
	 *
	 * Remember to call @ref startReading() after the constructor and
	 * setting up connections.
	 **/
	KDirTree();

	/**
	 * Destructor.
	 **/
	virtual ~KDirTree();


     public slots:

	/**
	 * Actually start reading.
	 *
	 * It's not very pretty this is required as an extra method, but this
	 * cannot simply be done in the constructor: We need to give the caller
	 * a chance to set up Qt signal connections, and for this the
	 * constructor must return before any signals are sent, i.e. before
	 * anything is read.
	 **/
	void startReading( const KURL & url );

	/**
	 * Forcefully stop a running read process.
	 **/
	void abortReading();

	/**
	 * Refresh a subtree, i.e. read its contents from disk again.
	 *
	 * The old subtree will be deleted and rebuilt from scratch, i.e. all
	 * pointers to elements within this subtree will become invalid (a
	 * @ref subtreeDeleted() signal will be emitted to notify about that
	 * fact).
	 *
	 * When 0 is passed, the entire tree will be refreshed, i.e. from the
	 * root element on.
	 **/
	void refresh( KFileInfo *subtree = 0 );

	/**
	 * Select some other item in this tree. Triggers the @ref
	 * selectionChanged() signal - even to the sender of this signal,
	 * i.e. take care not to cause endless signal ping-pong!
	 *
	 * Select nothing if '0' is passed.
	 **/
	void selectItem( KFileInfo *newSelection );

	/**
	 * Delete a subtree.
	 **/
	void deleteSubtree( KFileInfo *subtree );


    public:

	/**
	 * Returns the root item of this tree.
	 *
	 * Currently, there can only be one single root item for each tree.
	 */
	KFileInfo *	root() const { return _root; }

	/**
	 * Sets the root item of this tree.
	 **/
	void setRoot( KFileInfo *newRoot );

	/**
	 * Clear all items of this tree.
	 *
	 * 'sendSignals' indicates whether or not to send deletingChild() and childDeleted() signals.
	 * A selectionChanged() signal will be sent in any case if there was a selected item.
	 **/
	void clear( bool sendSignals = false );

	/**
	 * Locate a child somewhere in the tree whose URL (i.e. complete path)
	 * matches the URL passed. Returns 0 if there is no such child.
	 *
	 * Notice: This is a very expensive operation since the entire tree is
	 * searched recursively.
	 *
	 * 'findDotEntries' specifies if locating "dot entries" (".../<Files>")
	 * is desired.
	 *
	 * This is just a convenience method that maps to
	 *    KDirTree::root()->locate( url, findDotEntries )
	 **/
	KFileInfo *	locate( QString url, bool findDotEntries = false )
	    { return _root ? _root->locate( url, findDotEntries ) : 0; }

#if 0
	/**
	 * Notification of a finished directory read job.
	 * All read jobs are required to call this upon (successful or
	 * unsuccessful) completion.
	 **/
	void jobFinishedNotify( KDirReadJob *job );
#endif

	/**
	 * Add a new directory read job to the queue.
	 **/
	void addJob( KDirReadJob * job );

	/**
	 * Obtain the directory read method for this tree:
	 *    KDirReadLocal		use opendir() and lstat()
	 *    KDirReadKDirLister	use KDE 2.x's KDirLister
	 **/
	KDirReadMethod readMethod() const { return _readMethod; }

	/**
	 * Should directory scans cross file systems?
	 *
	 * Notice: This can only be avoided with local directories where the
	 * device number a file resides on can be obtained.
	 * Remember, that's what this KDirStat business is all about.  ;-)
	 **/
	bool	crossFileSystems() const { return _crossFileSystems; }

	/**
	 * Set or unset the "cross file systems" flag.
	 **/
	void	setCrossFileSystems( bool doCross ) { _crossFileSystems = doCross; }

	/**
	 * Return the tree's current selection.
	 *
	 * Even though the KDirTree by itself doesn't have a visual
	 * representation, it supports the concept of one single selected
	 * item. Views can use this to transparently keep track of this single
	 * selected item, notifying the KDirTree and thus other views with @ref
	 * KDirTree::selectItem() . Attached views should connect to the @ref
	 * selectionChanged() signal to be notified when the selection changes.
	 *
	 * NOTE: This method returns 0 if nothing is selected.
	 **/
	KFileInfo *	selection() const { return _selection; }

	/**
	 * Notification that a child has been added.
	 *
	 * Directory read jobs are required to call this for each child added
	 * so the tree can emit the corresponding @ref childAdded() signal.
	 **/
	virtual void childAddedNotify( KFileInfo *newChild );

	/**
	 * Notification that a child is about to be deleted.
	 *
	 * Directory read jobs are required to call this for each deleted child
	 * so the tree can emit the corresponding @ref deletingChild() signal.
	 **/
	virtual void deletingChildNotify( KFileInfo *deletedChild );

	/**
	 * Notification that one or more children have been deleted.
	 *
	 * Directory read jobs are required to call this when one or more
	 * children are deleted so the tree can emit the corresponding @ref
	 * deletingChild() signal. For multiple deletions (e.g. entire
	 * subtrees) this should only happen once at the end.
	 **/
	virtual void childDeletedNotify();

	/**
	 * Send a @ref progressInfo() signal to keep the user entertained while
	 * directories are being read.
	 **/
	void sendProgressInfo( const QString &infoLine );

	/**
	 * Send a @ref finalizeLocal() signal to give views a chance to
	 * finalize the display of this directory level - e.g. clean up dot
	 * entries, set the final "expandable" state etc.
	 **/
	void sendFinalizeLocal( KDirInfo *dir );

	/**
	 * Send a @ref startingReading() signal.
	 **/
	void sendStartingReading();

	/**
	 * Send a @ref finished() signal.
	 **/
	void sendFinished();

	/**
	 * Send a @ref aborted() signal.
	 **/
	void sendAborted();

	/**
	 * Returns 'true' if this tree uses the 'file:/' protocol (regardless
	 * of local or network transparent directory reader).
	 **/
	bool isFileProtocol()	{ return _isFileProtocol; }

	/**
	 * Returns 'true' if directory reading is in progress in this tree.
	 **/
	bool isBusy() { return _isBusy; }

	/**
	 * Write the complete tree to a cache file.
	 *
	 * Returns true if OK, false upon error.
	 **/
	bool writeCache( const QString & cacheFileName );

	/**
	 * Read a cache file.
	 **/
	void readCache( const QString & cacheFileName );


    signals:

	/**
	 * Emitted when a child has been added.
	 **/
	void childAdded( KFileInfo *newChild );

	/**
	 * Emitted when a child is about to be deleted.
	 **/
	void deletingChild( KFileInfo *deletedChild );

	/**
	 * Emitted after a child is deleted. If you are interested which child
	 * it was, better use the @ref deletingChild() signal.
	 * @ref childDeleted() is only useful to rebuild a view etc. completely.
	 * If possible, this signal is sent only once for multiple deletions -
	 * e.g., when entire subtrees are deleted.
	 **/
	void childDeleted();

	/**
	 * Emitted when reading is started.
	 **/
	void startingReading();

	/**
	 * Emitted when reading this directory tree is finished.
	 **/
	void finished();

	/**
	 * Emitted when reading this directory tree has been aborted.
	 **/
	void aborted();

	/**
	 * Emitted when reading a directory is finished.
	 * This does _not_ mean reading all subdirectories is finished, too -
	 * only this directory level is complete!
	 *
	 * WARNING: 'dir' may be 0 if the the tree's root could not be read.
	 *
	 * Use this signal to do similar cleanups like
	 * @ref KDirInfo::finalizeLocal(), e.g. cleaning up unused / undesired
	 * dot entries like in @ref KDirInfo::cleanupDotEntries().
	 **/
	void finalizeLocal( KDirInfo *dir );

	/**
	 * Emitted when the current selection has changed, i.e. whenever some
	 * attached view triggers the @ref selectItem() slot or when the
	 * current selection is deleted.
	 *
	 * NOTE: 'newSelection' may be 0 if nothing is selected.
	 **/
	void selectionChanged( KFileInfo *newSelection );

	/**
	 * Single line progress information, emitted when the read status
	 * changes - typically when a new directory is being read. Connect to a
	 * status bar etc. to keep the user entertained.
	 **/
	void progressInfo( const QString &infoLine );

	
    protected slots:
    
	/**
	 * Read some parameters from the global @ref KConfig object.
	 **/
	void readConfig();

	/**
	 * Notification that all jobs in the job queue are finished.
	 * This will emit the finished() signal.
	 **/
	void slotFinished();

	
    protected:

	KFileInfo *		_root;
	KFileInfo *		_selection;
	KDirReadJobQueue	_jobQueue;
	KDirReadMethod		_readMethod;
	bool			_crossFileSystems;
	bool			_enableLocalDirReader;
	bool			_isFileProtocol;
	bool			_isBusy;
	
    };	// class KDirTree

}	// namespace KDirStat


#endif // ifndef KDirTree_h


// EOF
