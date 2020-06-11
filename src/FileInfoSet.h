/*
 *   File name: FileInfoSet.h
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileInfoSet_h
#define FileInfoSet_h

#include <QSet>
#include "FileInfo.h"


namespace QDirStat
{
    /**
     * Container for FileInfo pointers. This is a wrapper around QSet with a
     * few add-on functions.
     **/
    class FileInfoSet: public QSet<FileInfo *>
    {
    public:
	FileInfoSet():
	    QSet<FileInfo *>()
	    {}

	/**
	 * Return the first item in this set or 0 if the set is empty.
	 *
	 * This makes most sense if there is only one item at all;
	 * otherwise it would be completely random which item would be
	 * returned as the first.
	 **/
	FileInfo * first() const;

	/**
	 * Return 'true' if the set contains any directory item.
	 **/
	bool containsDir() const;

	/**
	 * Return 'true' if the set contains any file item.
	 **/
	bool containsFile() const;

	/**
	 * Return 'true' if the set contains any special file,
	 * i.e., a char or block device, a FIFO, or a socket.
	 **/
	bool containsSpecial() const;

	/**
	 * Return 'true' if the set contains any PkgInfo item.
	 **/
	bool containsPkg() const;

	/**
	 * Return 'true' if the set contains any pseudo directory, i.e. any dot
	 * entry ("<Files>") or attic ("<Ignored>).
	 **/
	bool containsPseudoDir() const;

	/**
	 * Return 'true' if the set contains any dot entry ("<Files">).
	 **/
	bool containsDotEntry() const;

	/**
	 * Return 'true' if the set contains any attic ("<Ignored>").
	 **/
	bool containsAttic() const;

	/**
	 * Return the sum of all total sizes in the set.
	 *
	 * It is desirable to call this on a normalized() set to avoid
	 * duplicate accounting of sums.
	 **/
	FileSize totalSize() const;

	/**
	 * Return the sum of all total allocated sizes in the set.
	 *
	 * It is desirable to call this on a normalized() set to avoid
	 * duplicate accounting of sums.
	 **/
	FileSize totalAllocatedSize() const;

	/**
	 * Return 'true' if this set contains any ancestor (parent, parent's
	 * parent etc.) of 'item'. This does not check if 'item' itself is in
	 * the set.
	 **/
	bool containsAncestorOf( FileInfo * item ) const;

	/**
	 * Return 'true' if any item in this set is busy.
	 **/
	bool containsBusyItem() const;

	/**
	 * Return 'true' if this set is non-empty and the dir tree is busy.
	 **/
	bool treeIsBusy() const;

	/**
	 * Return a set with all the invalid items removed, i.e. without items
	 * where checkMagicNumber() returns 'false'.
	 *
	 * If there is reason to believe that any items of the set might have
	 * become invalid, call this first before any other operations.
	 *
	 * Notice that this does not modify the existing set, but you can
	 * of course assign the result of this to the set.
	 **/
	FileInfoSet invalidRemoved() const;

	/**
	 * Return a 'normalized' set, i.e. with all items removed that have
	 * ancestors in the set.
	 **/
	FileInfoSet normalized() const;

    };	// class FileInfoSet

}	// namespace QDirStat


#endif	// FileInfoSet_h
