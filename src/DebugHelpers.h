/*
 *   File name: DebugHelpers.h
 *   Summary:	Debugging helper functions for QDirstat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef DebugHelpers_h
#define DebugHelpers_h

#include <QModelIndex>
#include <QStringList>
#include <QString>

#include "FileInfo.h"

class QAbstractItemModel;


namespace Debug
{
    using QDirStat::FileInfo;
    using QDirStat::FileInfoList;

    /**
     * Dump the direct children of 'dir' to the log.
     **/
    void dumpDirectChildren( FileInfo * dir );

    /**
     * Dump the direct children of 'dir' sorted by size to the log.
     **/
    void dumpChildrenBySize( FileInfo * dir );

    /**
     * Dump a children list to the log. If specified, use 'dirName' as the
     * directory name for the heading line. This mostly makes sense with an
     * unnamed directory like the DirTreeModel's pseudo root.
     **/
    void dumpChildrenList( FileInfo	      * dir,
			   const FileInfoList & children );

    /**
     * Recursively dump the model tree from 'index' on to the log.
     * Indent each line with 'indent' (typically a string consisting of
     * blanks).
     **/
    void dumpModelTree( const QAbstractItemModel * model,
			const QModelIndex	 & index,
			const QString		 & indent );

    /**
     * Dump the exclude rules to the log.
     **/
    void dumpExcludeRules();

    /**
     * Return a string list of data(0) of the tree ancestors of 'index'.
     * The list will start with the tree's root.
     **/
    QStringList modelTreeAncestors( const QModelIndex & index );

}	// namespace

#endif	// DebugHelpers_h
