/*
 *   File name: Attic.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef Attic_h
#define Attic_h


#include "DirInfo.h"


namespace QDirStat
{
    // Forward declarations
    class DirTree;

    /**
     * Special DirInfo to store ignored files in. This behaves very much like a
     * normal DirInfo; it can have a DotEntry, and it can have subdirectories.
     *
     * But it is not a normal child of its parent DirInfo, i.e. you normally
     * don't iterate over it; use DirInfo::attic() to access it.  Its sizes,
     * children counts etc. are not added to the parent dir's sums.
     *
     * The treemap will not display anything in the attic; that's the whole
     * point of it. Yet, the user can open the attic (the <Ignored> pseudo
     * entry) in the tree view.
     **/
    class Attic: public DirInfo
    {
    public:
	/**
	 * Constructor.
	 **/
	Attic( DirTree * tree,
	       DirInfo * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~Attic();

	/**
	 * Check if this is an attic entry where ignored files and directories
	 * are stored.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual bool isAttic() const Q_DECL_OVERRIDE
	    { return true; }

	/**
	 * Return the attic of this tree node. Since this already is an attic,
	 * this always returns 0.
	 *
	 * Reimplemented from DirInfo.
	 **/
	virtual Attic * attic() const Q_DECL_OVERRIDE { return 0; }

	/**
	 * Get the current state of the directory reading process.
	 * This reimplementation returns the parent directory's value.
	 *
	 * Reimplemented - inherited from DirInfo.
	 **/
	virtual DirReadState readState() const Q_DECL_OVERRIDE;

	/**
	 * Returns true if this entry has any children.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual bool hasChildren() const Q_DECL_OVERRIDE;

	/**
	 * Check the 'ignored' state of this item and set the '_isIgnored' flag
	 * accordingly.
	 *
	 * Reimplemented - inherited from DirInfo.
	 **/
	virtual void checkIgnored() Q_DECL_OVERRIDE;

	/**
	 * Locate a child somewhere in this subtree whose URL (i.e. complete
	 * path) matches the URL passed. Returns 0 if there is no such child.
	 *
	 * Reimplemented - inherited from FileInfo.
	 **/
	virtual FileInfo * locate( QString url,
				   bool	   findPseudoDirs = false ) Q_DECL_OVERRIDE;

    };	// class Attic

}	// namespace QDirStat


#endif // ifndef Attic_h

