/*
 *   File name: StatsCollector.cpp
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <algorithm>

#include "StatsCollector.h"
#include "FileInfoIterator.h"
#include "DirTree.h"

#define VERBOSE_SORT_THRESHOLD  10000

using namespace QDirStat;


StatsCollector::StatsCollector():
    _sorted( false )
{

}

void StatsCollector::clear()
{
    // Just _data.clear() does not free any memory; we need to assign an empty
    // list to _data.

    _data = FileSizeList();
}


void StatsCollector::collect( DirTree * tree )
{
    Q_CHECK_PTR( tree );

    collect( tree->root() );
}


void StatsCollector::collect( FileInfo * subtree )
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


void StatsCollector::collect( DirTree * tree, const QString & suffix )
{
    Q_CHECK_PTR( tree );

    if ( ! suffix.startsWith( "." ) )
    {
        logError() << "Suffix should start with \".\" - got \""
                   << suffix << "\"" << endl;
    }

    collect( tree->root(), suffix );
}


void StatsCollector::collect( FileInfo * subtree, const QString & suffix )
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
	    collect( item );
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


void StatsCollector::sort()
{
    if ( _data.size() > VERBOSE_SORT_THRESHOLD )
        logDebug() << "Sorting " << _data.size() << " elements" << endl;

    std::sort( _data.begin(), _data.end() );
    _sorted = true;

    if ( _data.size() > VERBOSE_SORT_THRESHOLD )
        logDebug() << "Sorting done." << endl;
}


FileSize StatsCollector::median()
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


FileSize StatsCollector::min()
{
    if ( _data.isEmpty() )
        return 0;

    if ( ! _sorted )
        sort();

    return _data.first();
}



FileSize StatsCollector::max()
{
    if ( _data.isEmpty() )
        return 0;

    if ( ! _sorted )
        sort();

    return _data.last();
}


FileSize StatsCollector::quantile( int quantileOrder, int quantileNumber )
{
    if ( ! _sorted )
        sort();

    FileSize result = 0;

    // TO DO
    // TO DO
    // TO DO
    Q_UNUSED( quantileOrder  );
    Q_UNUSED( quantileNumber );

    return result;
}
