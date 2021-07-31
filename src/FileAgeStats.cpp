/*
 *   File name: FileAgeStats.cpp
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include <algorithm>    // std::sort()
#include <QDate>

#include "FileAgeStats.h"
#include "FileInfoIterator.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


short FileAgeStats::_thisYear  = 0;
short FileAgeStats::_thisMonth = 0;
short FileAgeStats::_lastYear  = 0;


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

    clearMonthStats( thisYear() );
    clearMonthStats( lastYear() );

    _totalFilesCount = 0;
    _totalFilesSize  = 0;
}


void FileAgeStats::clearMonthStats( short year )
{
    for ( int month = 1; month <= 12; month++ )
    {
        YearStats * stats = monthStats( year, month );

        if ( stats )
            *stats = YearStats( year, month );
    }
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
            short year  = item->mtimeYear();
            short month = item->mtimeMonth();

            YearStats &yearStats = _yearStats[ year ];

            yearStats.year = year;
            yearStats.filesCount++;
            yearStats.size += item->size();

            YearStats * monthStats = this->monthStats( year, month );

            if ( monthStats )
            {
                monthStats->filesCount++;
                monthStats->size += item->size();
            }
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

    calcMonthPercentages( _thisYear );
    calcMonthPercentages( _lastYear );
}


void FileAgeStats::calcMonthPercentages( short year )
{
    YearStats * yearStats = this->yearStats( year );

    if ( ! yearStats )
        return;

    for ( int month = 1; month <= 12; month++ )
    {
        YearStats * stats = monthStats( year, month );

        if ( stats )
        {
            if ( yearStats->filesCount > 0 )
                stats->filesPercent = ( 100.0 * stats->filesCount ) / yearStats->filesCount;

            if ( yearStats->size > 0 )
                stats->sizePercent = ( 100.0 * stats->size ) / yearStats->size;
        }
    }
}


void FileAgeStats::collectYears()
{
    _yearsList = _yearStats.keys();
    std::sort( _yearsList.begin(), _yearsList.end() );
}


YearStats * FileAgeStats::yearStats( short year )
{
    if ( _yearStats.contains( year ) )
        return &( _yearStats[ year ] );
    else
        return 0;
}


YearStats * FileAgeStats::monthStats( short year, short month )
{
    YearStats * stats = 0;

    if ( month >= 1 && month <= 12 )
    {
        if ( year == thisYear() && month <= thisMonth() )
            stats = &( _thisYearMonthStats[ month - 1 ] );
        else if ( year == lastYear() )
            stats = &( _lastYearMonthStats[ month - 1 ] );
    }

    return stats;
}


bool FileAgeStats::monthStatsAvailableFor( short year ) const
{
    return year == _thisYear || year == _lastYear;
}


short FileAgeStats::thisYear()
{
    if ( _thisYear == 0 )
        _thisYear = (short) QDate::currentDate().year();

    return _thisYear;
}


short FileAgeStats::thisMonth()
{
    if ( _thisMonth == 0 )
        _thisMonth = (short) QDate::currentDate().month();

    return _thisMonth;
}


short FileAgeStats::lastYear()
{
    if ( _lastYear == 0 )
        _lastYear = thisYear() - 1;

    return _lastYear;
}
