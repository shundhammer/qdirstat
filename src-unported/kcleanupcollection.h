/*
 *   File name:	kcleanupcollection.h
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifndef KCleanupCollection_h
#define KCleanupCollection_h


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "kcleanup.h"

// Forward declarations
class KActionCollection;


namespace KDirStat
{
    typedef QDict<KCleanup>		KCleanupDict;
    typedef QDictIterator<KCleanup>	KCleanupDictIterator;

    typedef QPtrList<KCleanup>		KCleanupList;
    typedef QPtrListIterator<KCleanup>	KCleanupListIterator;

    
    /**
     * Set of @ref KCleanup actions to be performed for @ref KDirTree items,
     * consisting of a number of predefined and a number of user-defined
     * cleanups. The prime purpose of this is to make save/restore operations
     * with a number of cleanups easier. Thus, it provides a copy constructor,
     * an assignment operator and various methods to directly access individual
     * cleanups.
     *
     * @short KDirStat cleanup action collection
     **/

    class KCleanupCollection: public QObject
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 *
	 * Most applications will want to pass KMainWindow::actionCollection()
	 * for 'actionCollection' so the menus and toolbars can be created
	 * using the XML UI description ('kdirstatui.rc' for KDirStat).
	 *
	 * All @ref KCleanup actions ever added to this collection will get
	 * this as their parent.
	 **/
	KCleanupCollection( KActionCollection *	actionCollection = 0 );

	/**
	 * Copy Constructor.
	 *
	 * Makes a deep copy of this collection with 'actionCollection' set to
	 * 0 for all copied cleanups. Please note that since there is no
	 * complete copy constructor for @ref KCleanup, all restrictions to the
	 * @ref KCleanup copy constructor apply to the KCleanupCollection, too:
	 * This copy constructor is intended for save/restore operations only,
	 * not for general use.  In particular, DO NOT connect an object thus
	 * constructed with signals. The results will be undefined (at best).
	 **/
	KCleanupCollection( const KCleanupCollection &src );

	/**
	 * Assignment operator.
	 *
	 * This operator has the same restrictions as the copy constructor:
	 * Just like the copy constructor, this is intended for save/restore
	 * operations, not for general use.
	 *
	 * For details, see the extensive comments in the source file.
	 **/
	KCleanupCollection &	operator= ( const KCleanupCollection &src );

	/**
	 * Destructor
	 **/
	virtual ~KCleanupCollection();

	/**
	 * Add the standard cleanups to this collection.
	 **/
	void addStdCleanups();

	/**
	 * Add 'number' user-defined cleanups to this collection.
	 **/
	void addUserCleanups( int number );

	/**
	 * Add one single cleanup to this collection. The collection assumes
	 * ownerwhip of this cleanup - don't delete it!
	 **/
	void add( KCleanup *cleanup );

	/**
	 * Retrieve a cleanup by its ID (internal name).
	 * Returns 0 if there is no such cleanup.
	 **/
	KCleanup * cleanup( const QString & id );

	/**
	 * An alias to @ref cleanup() for convenience: Thus, you can use
	 * collection[ "cleanup_id" ] to access any particular cleanup.
	 **/
	KCleanup * operator[] ( const QString & id )
	    { return cleanup( id ); }
	
	/**
	 * Remove all cleanups from this collection.
	 **/
	void clear();

	/**
	 * Return (a shallow copy of) the internal cleanup list.
	 *
	 * Use this and a KCleanupListIterator to iterate over all cleanups in
	 * this collection. Remember to keep the list until you no longer need
	 * the iterator! 
	 *
	 *	KCleanupCollection *coll = ...
	 *	KCleanupList cleanup_list = coll->cleanupList();
	 *	KCleanupListIterator it( cleanup_list );
	 *
	 *	while ( *it )
	 *	{
	 *	    kdDebug() << "Found cleanup " << *it << endl;
	 *	    ++it;
	 *	}
	 **/
	KCleanupList cleanupList() const { return _cleanupList; }

	/**
	 * Return the number of cleanup actions in this collection.
	 **/
	int size() const { return _cleanupList.count(); }

	/**
	 * For internal use only: Returns the number to be assigned to the next
	 * user cleanup that may be added.
	 **/
	int nextUserCleanupNo() const { return _nextUserCleanupNo; }

    public slots:

        /**
	 * Emit the readConfig() signal for all cleanups.
	 **/
        void slotReadConfig();

	
    signals:
	
	/**
	 * Emitted when the currently selected item changes.
	 * 'item' may be 0 when the selection is cleared.
	 *
	 * Connect a view's selectionChanged() signal to this
	 * selectionChanged() signal to have the cleanup collection pass this
	 * signal to its cleanups.
	 **/
	void selectionChanged( KFileInfo *item );

        /**
	 * Read collection for all cleanups.
	 **/
        void readConfig();

	/**
	 * Save configuration for all cleanups.
	 **/
	void saveConfig();

	/**
	 * Emitted at user activity, i.e. when the user executes a cleanup.
	 * This is intended for use together with a @ref KActivityTracker.
	 **/
	void userActivity( int points );


    protected slots:

        /**
	 * Connected to each cleanup's @ref executed() signal to track user
	 * activity. 
	 **/
	void cleanupExecuted();


    protected:

	/**
	 * Internal implementation of copy constructor and assignment operator:
	 * Make a deep copy of the collection.
	 **/
	void deepCopy( const KCleanupCollection &src );

	
	// Data members
	
        KActionCollection *	_actionCollection;
	int			_nextUserCleanupNo;
	KCleanupList		_cleanupList;
	KCleanupDict		_cleanupDict;
    };
}	// namespace KDirStat


#endif // ifndef KCleanupCollection_h


// EOF
