/*
 *   File name:	CleanupCollection.h
 *   Summary:	QDirStat classes to reclaim disk space
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef CleanupCollection_h
#define CleanupCollection_h

#include "QList"
#include "Cleanup.h"
#include "FileInfo.h"


class QMenu;


namespace QDirStat
{
    class SelectionModel;

    /**
     * Set of Cleanup actions to be performed for DirTree items, consisting of
     * a number of predefined and a number of user-defined cleanups.
     **/
    class CleanupCollection: public QObject
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	CleanupCollection( SelectionModel * selectionModel,
			   QObject	  * parent = 0 );

	/**
	 * Destructor
	 **/
	virtual ~CleanupCollection();

	/**
	 * Add the standard cleanups to this collection.
	 **/
	void addStdCleanups();

	/**
	 * Add a cleanup to this collection. The collection assumes ownerwhip
	 * of this cleanup.
	 **/
	void add( Cleanup * cleanup );

	/**
	 * Remove a cleanup from this collection and delete it.
	 **/
	void remove( Cleanup * cleanup );

	/**
	 * Add all actions to the specified menu.
	 **/
	void addToMenu( QMenu * menu );

	/**
	 * Return the index of a cleanup or -1 if it is not part of this
	 * collection.
	 **/
	int indexOf( Cleanup * cleanup ) const;

	/**
	 * Return the cleanup with the specified index or 0 if the index is out
	 * of range.
	 **/
	Cleanup * at( int index ) const;

	/**
	 * Return the number of cleanup actions in this collection.
	 **/
	int size() const { return _cleanupList.size(); }

	/**
	 * Return 'true' if this collection is empty.
	 **/
	bool isEmpty() const { return _cleanupList.isEmpty(); }

	/**
	 * Remove all cleanups from this collection.
	 **/
	void clear();

	/**
	 * Return the internal cleanup list.
	 **/
	const CleanupList & cleanupList() const { return _cleanupList; }

    signals:

	/**
	 * Emitted when a cleanup is started.
	 **/
	void startingCleanup( const QString & cleanupName );

	/**
	 * Emitted when the last process of a cleanup is finished.
	 *
	 * 'errorCount' is the total number of errors reported by all processes
	 * that were started.
	 **/
	void cleanupFinished( int errorCount );

    public slots:

	/**
	 * Update the enabled/disabled state of all cleanup actions depending
	 * on the SelectionModel.
	 **/
	void updateActions();

	/**
	 * Move a cleanup one position up in the list.
	 **/
	void moveUp( Cleanup * cleanup );

	/**
	 * Move a cleanup one position down in the list.
	 **/
	void moveDown( Cleanup * cleanup );

	/**
	 * Move a cleanup to the top of the list.
	 **/
	void moveToTop( Cleanup * cleanup );

	/**
	 * Move a cleanup to the bottom of the list.
	 **/
	void moveToBottom( Cleanup * cleanup );


	/**
	 * Read configuration for all cleanups.
	 **/
	void readSettings();

	/**
	 * Write configuration for all cleanups.
	 **/
	void writeSettings();


    protected slots:

	/**
	 * Execute a cleanup. This uses sender() to find out which cleanup it
	 * was.
	 **/
	void execute();


    protected:

	/**
	 * Ask user for confirmation to execute a cleanup action for
	 * 'items'. Returns 'true' if user accepts, 'false' otherwise.
	 **/
	bool confirmation( Cleanup * cleanup, const FileInfoSet & items );


	//
	// Data members
	//

	SelectionModel * _selectionModel;
	CleanupList	 _cleanupList;
    };
}	// namespace QDirStat


#endif // ifndef CleanupCollection_h

