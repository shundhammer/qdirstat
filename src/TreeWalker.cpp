/*
 *   File name: TreeWalker.cpp
 *   Summary:	QDirStat helper class to walk a FileInfo tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "TreeWalker.h"
#include "FileSizeStats.h"
#include "FileMTimeStats.h"
#include "SysUtil.h"
#include "Logger.h"
#include "Exception.h"

#define RESULTS_COUNT  50


using namespace QDirStat;


void LargestFilesTreeWalker::prepare( FileInfo * subtree )
{
    _threshold = 0;

    FileSizeStats stats;
    stats.collect( subtree );
    stats.sort();

    if ( stats.dataSize() <= 100 )
        _threshold = stats.percentile( 80 );
    else if ( stats.dataSize() <= 1000 )
        _threshold = stats.percentile( 95 );
    else
    {
        int index = stats.dataSize() - RESULTS_COUNT;
        _threshold = stats.data().at( index );
    }
}


void NewFilesTreeWalker::prepare( FileInfo * subtree )
{
    _threshold = 0;

    FileMTimeStats stats;
    stats.collect( subtree );
    stats.sort();

    if ( stats.dataSize() <= 100 )
        _threshold = stats.percentile( 80 );
    else if ( stats.dataSize() <= 1000 )
        _threshold = stats.percentile( 95 );
    else
    {
        int index = stats.dataSize() - RESULTS_COUNT;
        _threshold = stats.data().at( index );
    }
}


void OldFilesTreeWalker::prepare( FileInfo * subtree )
{
    _threshold = 0;

    FileMTimeStats stats;
    stats.collect( subtree );
    stats.sort();

    if ( stats.dataSize() <= 100 )
        _threshold = stats.percentile( 20 );
    else if ( stats.dataSize() <= 1000 )
        _threshold = stats.percentile( 5 );
    else
    {
        int index = RESULTS_COUNT;
        _threshold = stats.data().at( index );
    }
}


bool BrokenSymLinksTreeWalker::check( FileInfo * item )
{
    return item &&
        item->isSymLink() &&
        SysUtil::isBrokenSymLink( item->url() );
}
