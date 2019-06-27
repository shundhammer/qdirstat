/*
 *   File name: DotEntry.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DotEntry_h
#define DotEntry_h


#include "DirInfo.h"


namespace QDirStat
{
    // Forward declarations
    class DirTree;

    /**
     * This is a very special kind if DirInfo: The <Files> pseudo container
     * that groups the non-directory children of a directory together. The
     * basic idea is keep the direct file children of a directory in one
     * container so their total size can easily be compared to any of the
     * subdirectories.
     **/
    class DotEntry: public DirInfo
    {
    public:
	/**
	 * Constructor.
	 **/
	DotEntry( DirTree * tree,
		  DirInfo * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~DotEntry();

	/**
	 * Get the "Dot Entry" for this node if there is one (or 0 otherwise).
	 * Since this is a dot entry, this always returns 0: A dot entry does
	 * not have a dot entry itself.
	 **/
	virtual DotEntry * dotEntry() const Q_DECL_OVERRIDE
	    { return 0; }

	/**
	 * Check if this is a dot entry.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual bool isDotEntry() const Q_DECL_OVERRIDE
	    { return true; }

	/**
	 * Insert a child into the children list.
	 *
	 * The order of children in this list is absolutely undefined;
	 * don't rely on any implementation-specific order.
	 *
	 * Reimplemented - inherited from DirInfo.
	 **/
	virtual void insertChild( FileInfo *newChild ) Q_DECL_OVERRIDE;

	/**
	 * Recursively finalize all directories from here on -
	 * call finalizeLocal() recursively.
	 *
	 * Reimplemented - inherited from DirInfo.
	 **/
	virtual void finalizeAll() Q_DECL_OVERRIDE;

	/**
	 * Get the current state of the directory reading process.
	 * This reimplementation returns the parent directory's value.
	 *
	 * Reimplemented - inherited from DirInfo.
	 **/
	virtual DirReadState readState() const Q_DECL_OVERRIDE;

	/**
	 * Reset to the same status like just after construction in preparation
	 * of refreshing the tree from this point on.
	 *
	 * Reimplemented - inherited from DirInfo.
	 **/
	virtual void reset() Q_DECL_OVERRIDE;


    protected:

	/**
	 * Clean up unneeded / undesired dot entries.
	 *
	 * Since a dot entry does not have a dot entry itself, this
	 * reimplementation does nothing.
	 *
	 * Reimplemented - inherited from DirInfo.
	 **/
	virtual void cleanupDotEntries() Q_DECL_OVERRIDE;

    };	// class DotEntry

}	// namespace QDirStat


#endif // ifndef DotEntry_h

