/*
 *   File name: PercentileStats.cpp
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <math.h>       // ceil()
#include <algorithm>

#include "PercentileStats.h"
#include "Exception.h"

#define VERBOSE_SORT_THRESHOLD  50000

using namespace QDirStat;


PercentileStats::PercentileStats():
    _sorted( false )
{

}


PercentileStats::~PercentileStats()
{

}


void PercentileStats::clear()
{
    // Just _data.clear() does not free any memory; we need to assign an empty
    // list to _data.

    _data = QRealList();
}


void PercentileStats::sort()
{
    if ( _data.size() > VERBOSE_SORT_THRESHOLD )
        logDebug() << "Sorting " << _data.size() << " elements" << endl;

    std::sort( _data.begin(), _data.end() );
    _sorted = true;

    if ( _data.size() > VERBOSE_SORT_THRESHOLD )
        logDebug() << "Sorting done." << endl;
}


qreal PercentileStats::median()
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

    qreal result = _data.at( centerPos );

    if ( _data.size() % 2 == 0 ) // Even number of data
    {
        // Use the average of the two center items. We already accounted for
        // the upper one with the above integer division, so now we need to
        // account for the lower one: If _data.size() is 6, we already got
        // _data[3], and now we need to average this with _data[2] of
        // [0, 1, 2, 3, 4, 5].

        result = ( result + _data.at( centerPos - 1 ) ) / 2.0;
    }

    return result;
}


qreal PercentileStats::average()
{
    if ( _data.isEmpty() )
        return 0.0;

    int count = _data.size();
    qreal sum = 0.0;

    for ( int i=0; i < count; ++i )
        sum += _data.at(i);

    return sum / count;
}


qreal PercentileStats::min()
{
    if ( _data.isEmpty() )
        return 0.0;

    if ( ! _sorted )
        sort();

    return _data.first();
}


qreal PercentileStats::max()
{
    if ( _data.isEmpty() )
        return 0.0;

    if ( ! _sorted )
        sort();

    return _data.last();
}


qreal PercentileStats::quantile( int order, int number )
{
    if ( _data.isEmpty() )
        return 0.0;

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

    qreal result = _data.at( pos );

    if ( ( _data.size() * number ) % order == 0 )
    {
        // Same as in median: We hit between two elements, so use the average
        // between them.

        result = ( result + _data.at( pos - 1 ) ) / 2.0;
    }

    return result;
}


QRealList PercentileStats::percentileList()
{
    QRealList percentiles;
    percentiles.reserve( 100 );

    for ( int i=0; i <= 100; ++i )
        percentiles << percentile( i );

    return percentiles;
}


QRealList PercentileStats::percentileSums()
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
