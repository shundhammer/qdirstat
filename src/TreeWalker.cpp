/*
 *   File name: TreeWalker.cpp
 *   Summary:	QDirStat helper class to walk a FileInfo tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "TreeWalker.h"
#include "FileSizeStats.h"
#include "Logger.h"
#include "Exception.h"

#define RESULTS_COUNT  100


using namespace QDirStat;


void LargestFilesTreeWalker::prepare( FileInfo * subtree )
{
    _threshold = 0;

    FileSizeStats stats;
    stats.collect( subtree );
    stats.sort();

    if ( stats.dataSize() <= 100 )
        _threshold = stats.quartile( 3 );
    else if ( stats.dataSize() <= 1000 )
        _threshold = stats.percentile( 99 );
    else
    {
        int index = stats.dataSize() - RESULTS_COUNT;
        _threshold = stats.data().at( index );
    }
}
