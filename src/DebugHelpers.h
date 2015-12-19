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

    /**
     * Dump the direct children of 'dir' to the log. If specified, use
     * 'dirName' as the directory name for the heading line. This mostly
     * makes sense with an unnamed directory like the DirTreeModel's pseudo
     * root.
     **/
    void dumpDirectChildren( FileInfo * dir, const QString & dirName = QString() );

    /**
     * Recursively dump the model tree from 'index' on to the log.
     * Indent each line with 'indent' (typically a string consisting of
     * blanks).
     **/
    void dumpModelTree( const QAbstractItemModel * model,
			const QModelIndex	 & index,
			const QString		 & indent );

    /**
     * Return a string list of data(0) of the tree ancestors of 'index'.
     * The list will start with the tree's root.
     **/
    QStringList modelTreeAncestors( const QModelIndex & index );

}	// namespace

#endif	// DebugHelpers_h
