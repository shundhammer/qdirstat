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
	 * Return 'true' if the set contains any dot entry.
	 **/
	bool containsDotEntry() const;

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
	 * Return the sum of all total sizes in the set.
	 *
	 * It is desirable to call this on a normalized() set to avoid
	 * duplicate accounting of sums.
	 **/
	FileSize totalSize() const;

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
	 * Return a 'normalized' set, i.e. with all items removed that have
	 * ancestors in the set.
	 **/
	FileInfoSet normalized() const;

    };	// class FileInfoSet

}	// namespace QDirStat


#endif	// FileInfoSet_h
