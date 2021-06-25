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

#define MAX_RESULTS  200


using namespace QDirStat;


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

    FileSizeStats stats( subtree );
    _threshold = (FileSize) upperPercentileThreshold( stats );
}


void NewFilesTreeWalker::prepare( FileInfo * subtree )
{
    FileMTimeStats stats( subtree );
    _threshold = (time_t) upperPercentileThreshold( stats );
}


void OldFilesTreeWalker::prepare( FileInfo * subtree )
{
    FileMTimeStats stats( subtree );
    _threshold = (time_t) lowerPercentileThreshold( stats );
}


bool BrokenSymLinksTreeWalker::check( FileInfo * item )
{
    return item &&
        item->isSymLink() &&
        SysUtil::isBrokenSymLink( item->url() );
}
