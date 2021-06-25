/*
 *   File name: FileMTimeStats.cpp
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <math.h>       // ceil()
#include <algorithm>

#include "FileMTimeStats.h"
#include "FileInfoIterator.h"
#include "DirTree.h"
#include "Exception.h"

#define VERBOSE_SORT_THRESHOLD  50000

using namespace QDirStat;


FileMTimeStats::FileMTimeStats( FileInfo * subtree ):
    PercentileStats()
{
    if ( subtree )
    {
        collect( subtree );
        sort();
    }
}


void FileMTimeStats::collect( FileInfo * subtree )
{
    Q_CHECK_PTR( subtree );

    if ( _data.isEmpty() )
        _data.reserve( subtree->totalFiles() );

    if ( subtree->isFile() )
        _data << subtree->mtime();

    FileInfoIterator it( subtree );

    while ( *it )
    {
	FileInfo * item = *it;

	if ( item->hasChildren() )
	{
	    collect( item );
	}
	else if ( item->isFile() )
	{
            _data << item->mtime();
	}
	// Disregard symlinks, block devices and other special files

	++it;
    }
}
