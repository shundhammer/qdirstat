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

#define MAX_RESULTS              200
#define MAX_FIND_FILES_RESULTS  1000




qreal TreeWalker::upperPercentileThreshold( PercentileStats & stats )
{
    int percentile = 0;

    if      ( stats.dataSize() <= 100 )                 percentile = 80;
    else if ( stats.dataSize() * 0.10 <= MAX_RESULTS )  percentile = 90;
    else if ( stats.dataSize() * 0.05 <= MAX_RESULTS )  percentile = 95;
    else if ( stats.dataSize() * 0.01 <= MAX_RESULTS )  percentile = 99;

    qreal threshold = 0.0;

    if ( percentile > 0 )
    {
        logDebug() << "Threshold: " << percentile << ". percentile" << endl;
        threshold = stats.percentile( percentile );
    }
    else
    {
        logDebug() << "Threshold: " << MAX_RESULTS << " items" << endl;
        int index = stats.dataSize() - MAX_RESULTS;
        threshold = stats.data().at( index );
    }

    return threshold;
}


qreal TreeWalker::lowerPercentileThreshold( PercentileStats & stats )
{
    int percentile = 0;

    if      ( stats.dataSize() <= 100 )                 percentile = 20;
    else if ( stats.dataSize() * 0.10 <= MAX_RESULTS )  percentile = 10;
    else if ( stats.dataSize() * 0.05 <= MAX_RESULTS )  percentile =  5;
    else if ( stats.dataSize() * 0.01 <= MAX_RESULTS )  percentile =  1;

    qreal threshold = 0.0;

    if ( percentile > 0 )
    {
        logDebug() << "Threshold: " << percentile << ". percentile" << endl;
        threshold = stats.percentile( percentile );
    }
    else
    {
        logDebug() << "Threshold: " << MAX_RESULTS << " items" << endl;
        int index = MAX_RESULTS;
        threshold = stats.data().at( index );
    }

    return threshold;
}




void LargestFilesTreeWalker::prepare( FileInfo * subtree )
{
    TreeWalker::prepare( subtree );
    FileSizeStats stats( subtree );
    _threshold = (FileSize) upperPercentileThreshold( stats );
}


void NewFilesTreeWalker::prepare( FileInfo * subtree )
{
    TreeWalker::prepare( subtree );
    FileMTimeStats stats( subtree );
    _threshold = (time_t) upperPercentileThreshold( stats );
}


void OldFilesTreeWalker::prepare( FileInfo * subtree )
{
    TreeWalker::prepare( subtree );
    FileMTimeStats stats( subtree );
    _threshold = (time_t) lowerPercentileThreshold( stats );
}


bool BrokenSymLinksTreeWalker::check( FileInfo * item )
{
    return item &&
        item->isSymLink() &&
        SysUtil::isBrokenSymLink( item->url() );
}




void FindFilesTreeWalker::prepare( FileInfo * subtree )
{
    TreeWalker::prepare( subtree );
    _count = 0;
}


bool FindFilesTreeWalker::check( FileInfo * item )
{
    if ( _count >= MAX_FIND_FILES_RESULTS )
    {
        _overflow = true;

        return false;
    }

    if ( ! item )
        return false;

    bool match = false;

    if ( ( _filter.findDirs()     && item->isDir()     ) ||
         ( _filter.findFiles()    && item->isFile()    ) ||
         ( _filter.findSymLinks() && item->isSymLink() ) ||
         ( _filter.findPkg()      && item->isPkgInfo() )   )
    {
        match = _filter.matches( item->name() );
    }

    if ( match )
        ++_count;

    return match;
}
