/*
 *   File name: FileSizeStats.cpp
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <math.h>       // ceil()
#include <algorithm>

#include "FileSizeStats.h"
#include "FileInfoIterator.h"
#include "DirTree.h"
#include "Exception.h"

#define VERBOSE_SORT_THRESHOLD  50000

using namespace QDirStat;


FileSizeStats::FileSizeStats():
    _sorted( false )
{

}

void FileSizeStats::clear()
{
    // Just _data.clear() does not free any memory; we need to assign an empty
    // list to _data.

    _data = FileSizeList();
}


void FileSizeStats::collect( DirTree * tree )
{
    Q_CHECK_PTR( tree );

    collect( tree->root() );
}


void FileSizeStats::collect( FileInfo * subtree )
{
    Q_CHECK_PTR( subtree );

    if ( _data.isEmpty() )
        _data.reserve( subtree->totalFiles() );

    if ( subtree->isFile() )
        _data << subtree->size();

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
            _data << item->size();
	}
	// Disregard symlinks, block devices and other special files

	++it;
    }
}


void FileSizeStats::collect( DirTree * tree, const QString & suffix )
{
    Q_CHECK_PTR( tree );

    if ( ! suffix.startsWith( "." ) )
    {
        logError() << "Suffix should start with \".\" - got \""
                   << suffix << "\"" << endl;
    }

    collect( tree->root(), suffix );
}


void FileSizeStats::collect( FileInfo * subtree, const QString & suffix )
{
    Q_CHECK_PTR( subtree );

    if ( _data.isEmpty() )
        _data.reserve( subtree->totalFiles() );

    if ( subtree->isFile() && subtree->name().endsWith( suffix ) )
        _data << subtree->size();

    FileInfoIterator it( subtree );

    while ( *it )
    {
	FileInfo * item = *it;

	if ( item->hasChildren() )
	{
	    collect( item, suffix );
	}
	else if ( item->isFile() )
	{
            if ( item->name().endsWith( suffix ) )
                _data << item->size();
	}
	// Disregard symlinks, block devices and other special files

	++it;
    }
}


void FileSizeStats::sort()
{
    if ( _data.size() > VERBOSE_SORT_THRESHOLD )
        logDebug() << "Sorting " << _data.size() << " elements" << endl;

    std::sort( _data.begin(), _data.end() );
    _sorted = true;

    if ( _data.size() > VERBOSE_SORT_THRESHOLD )
        logDebug() << "Sorting done." << endl;
}


FileSize FileSizeStats::median()
{
    if ( _data.isEmpty() )
        return 0;

    if ( ! _sorted )
        sort();

    int centerPos = _data.size() / 2;

    // Since we are doing integer division, the center is already rounded down
    // if there is an odd number of data items, so don't use the usual -1 to
    // compensate for the index of the first data element being 0, not 1: if
    // _data.size() is 5, we get _data[2] which is the center of
    // [0, 1, 2, 3, 4].

    FileSize result = _data.at( centerPos );

    if ( _data.size() % 2 == 0 ) // Even number of data
    {
        // Use the average of the two center items. We already accounted for
        // the upper one with the above integer division, so now we need to
        // account for the lower one: If _data.size() is 6, we already got
        // _data[3], and now we need to average this with _data[2] of
        // [0, 1, 2, 3, 4, 5].
        //
        // Since our value type FileSize is also an integer type, it is
        // arbitrary if we round this average up or down, so let's keep the
        // default "round down" result.

        result = ( result + _data.at( centerPos - 1 ) ) / 2;
    }

    return result;
}


FileSize FileSizeStats::average()
{
    if ( _data.isEmpty() )
        return 0;

    int count    = _data.size();
    FileSize sum = 0;

    for ( int i=0; i < count; ++i )
        sum += _data.at(i);

    return sum / count;
}


FileSize FileSizeStats::min()
{
    if ( _data.isEmpty() )
        return 0;

    if ( ! _sorted )
        sort();

    return _data.first();
}



FileSize FileSizeStats::max()
{
    if ( _data.isEmpty() )
        return 0;

    if ( ! _sorted )
        sort();

    return _data.last();
}


FileSize FileSizeStats::quantile( int order, int number )
{
    if ( _data.isEmpty() )
        return 0;

    if ( number > order )
    {
        QString msg = QString( "Cannot determine quantile #%1" ).arg( number );
        msg += QString( " for %1-quantile" ).arg( order );
        THROW( Exception( msg ) );
    }

    if ( order < 2 )
    {
        QString msg = QString( "Invalid quantile order %1" ).arg( order );
        THROW( Exception( msg ) );
    }

    if ( ! _sorted )
        sort();

    if ( number == 0 )
        return _data.first();

    if ( number == order )
        return _data.last();

    int pos = ( _data.size() * number ) / order;

    // Same as in median(): The integer division already cut off any non-zero
    // decimal place, so don't subtract 1 to compensate for starting _data with
    // index 0.

    FileSize result = _data.at( pos );

    if ( ( _data.size() * number ) % order == 0 )
    {
        // Same as in median: We hit between two elements, so use the average
        // between them.

        result = ( result + _data.at( pos - 1 ) ) / 2;
    }

    return result;
}


QRealList FileSizeStats::fillBuckets( int bucketCount,
                                      int startPercentile,
                                      int endPercentile )
{
    CHECK_INDEX( startPercentile, 0, 100 );
    CHECK_INDEX( endPercentile,   0, 100 );

    if ( startPercentile >= endPercentile )
        THROW( Exception( "startPercentile must be less than endPercentile" ) );

    if ( bucketCount < 1 )
        THROW( Exception( QString( "Invalid bucket count %1" ).arg( bucketCount ) ) );

    QRealList buckets;
    buckets.reserve( bucketCount );

    for ( int i=0; i < bucketCount; ++i )
        buckets << 0;

    if ( _data.isEmpty() )
        return buckets;


    // The first call to percentile() or quantile() will cause the data to be
    // sorted, so there is no need to sort them again here.

    qreal startVal = percentile( startPercentile );
    qreal endVal   = percentile( endPercentile );
    qreal bucketWidth = ( endVal - startVal ) / bucketCount;

#if 1
    logDebug() << "startPercentile: " << startPercentile
               << " endPercentile: " << endPercentile
               << " startVal: " << formatSize( startVal )
               << " endVal: " << formatSize( endVal )
               << " bucketWidth: " << formatSize( bucketWidth )
               << endl;
#endif

    for ( int i=0; i < _data.size(); ++i )
    {
        qreal val = _data.at( i );

        if ( val < startVal )
            continue;

        if ( val > endVal )
            break;

        // TO DO: Optimize this by taking into account that the data are sorted
        // already. We don't really need that many divisions; just when leaving
        // the current bucket would be sufficient.

        int index = qMin( ( val - startVal ) / bucketWidth, bucketCount - 1.0 );
        ++buckets[ index ];
    }

    return buckets;
}


QRealList FileSizeStats::percentileList()
{
    QRealList percentiles;
    percentiles.reserve( 100 );

    for ( int i=0; i <= 100; ++i )
        percentiles << percentile( i );

    return percentiles;
}


QRealList FileSizeStats::percentileSums()
{
    QRealList sums;
    sums.reserve( 100 );

    for ( int i=0; i <= 100; ++i )
        sums << 0.0;

    if ( ! _sorted )
        sort();

    qreal percentileSize = _data.size() / 100.0;

    for ( int i=0; i < _data.size(); ++i )
    {
        int percentile = qMax( 1, (int) ceil( i / percentileSize ) );

        sums[ percentile ] += _data.at(i);
    }

#if 0
    for ( int i=0; i < sums.size(); ++i )
        logDebug() << "sum[ " << i << " ] : " << formatSize( sums[i] ) << endl;
#endif

    return sums;
}
