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
	CleanupCollection( SelectionModel * selectionModel );

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
	 * Retrieve a cleanup by its ID (internal name).
	 * Returns 0 if there is no such cleanup.
	 **/
	Cleanup * findById( const QString & id ) const;

	/**
	 * Return the index of the cleanup with ID 'id' or -1 if there is no
	 * such cleanup.
	 **/
	int indexOf( const QString & id ) const;

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
	 * Remove all cleanups from this collection.
	 **/
	void clear();

	/**
	 * Return the internal cleanup list.
	 **/
	const CleanupList & cleanupList() const { return _cleanupList; }

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

	//
	// Data members
	//

	SelectionModel * _selectionModel;
	CleanupList	 _cleanupList;
    };
}	// namespace QDirStat


#endif // ifndef CleanupCollection_h

