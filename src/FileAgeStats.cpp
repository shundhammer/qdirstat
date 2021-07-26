/*
 *   File name: FileAgeStats.cpp
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include <algorithm>    // std::sort()

#include "FileAgeStats.h"
#include "FileInfoIterator.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


FileAgeStats::FileAgeStats( FileInfo * subtree )
{
    clear();

    if ( subtree )
        collect( subtree );
}


FileAgeStats::~FileAgeStats()
{
    // NOP
}


void FileAgeStats::clear()
{
    _yearStats.clear();
    _yearsList.clear();

    _totalFilesCount = 0;
    _totalFilesSize  = 0;
}


void FileAgeStats::collect( FileInfo * subtree )
{
    clear();
    collectRecursive( subtree );
    calcPercentages();
    collectYears();
}


void FileAgeStats::collectRecursive( FileInfo * dir )
{
    if ( ! dir )
	return;

    FileInfoIterator it( dir );

    while ( *it )
    {
	FileInfo * item = *it;

        if ( item && item->isFile() )
        {
            short year = item->mtimeYear();
            YearStats &stats = _yearStats[ year ];

            stats.year = year;
            stats.filesCount++;
            stats.size += item->size();
        }

	if ( item->hasChildren() )
	{
	    collectRecursive( item );
	}

        ++it;
    }
}


void FileAgeStats::calcPercentages()
{
    // Sum up the totals over all years

    _totalFilesCount = 0;
    _totalFilesSize  = 0;

    foreach ( const YearStats & stats, _yearStats )
    {
        _totalFilesCount += stats.filesCount;
        _totalFilesSize  += stats.size;
    }

    for ( YearStatsHash::iterator it = _yearStats.begin();
          it != _yearStats.end();
          ++it )
    {
        YearStats & stats = it.value();

        if ( _totalFilesCount > 0 )
            stats.filesPercent = ( 100.0 * stats.filesCount ) / _totalFilesCount;

        if ( _totalFilesSize > 0 )
            stats.sizePercent = ( 100.0 * stats.size ) / _totalFilesSize;
    }
}


void FileAgeStats::collectYears()
{
    _yearsList = _yearStats.keys();
    std::sort( _yearsList.begin(), _yearsList.end() );
}


YearStats FileAgeStats::yearStats( short year )
{
    if ( _yearStats.contains( year ) )
    {
        return _yearStats[ year ];
    }
    else
    {
        // Return default-constructed stats (i.e. all fields are 0)
        // for the requested year

        return YearStats( year );
    }
}
