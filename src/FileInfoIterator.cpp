/*
 *   File name: FileInfoIterator.cpp
 *   Summary:	Support classes for QDirStat - DirTree iterator classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <algorithm>

#include "FileInfoIterator.h"
#include "FileInfoSorter.h"
#include "DotEntry.h"
#include "Exception.h"

using namespace QDirStat;



FileInfoIterator::FileInfoIterator( FileInfo * parent )
{
    init( parent,
	  true ); // callNext
}


FileInfoIterator::FileInfoIterator( FileInfo * parent,
				    bool       callNext )
{
    init( parent, callNext );
}


void FileInfoIterator::init( FileInfo * parent,
			     bool	callNext )
{
    _parent  = parent;
    _current = 0;

    _directChildrenProcessed = false;
    _dotEntryProcessed	     = false;

    if ( callNext )
	next();
}


void FileInfoIterator::next()
{
    if ( ! _directChildrenProcessed )
    {
	// Process direct children

	_current = _current ? _current->next() : _parent->firstChild();

	if ( ! _current )
	{
	    _directChildrenProcessed = true;
	    next();
	}
    }
    else // _directChildrenProcessed
    {
	if ( ! _dotEntryProcessed )
	{
	    // Process dot entry

	    _current = _parent->dotEntry();
	    _dotEntryProcessed = true;
	}
	else	// Dot entry already processed
	{
	    _current = 0;
	}
    }
}


int FileInfoIterator::count()
{
    int cnt = 0;

    // Count direct children

    FileInfo * child = _parent->firstChild();

    while ( child )
    {
	cnt++;
	child = child->next();
    }


    // Handle the dot entry

    if ( _parent->dotEntry() )
	cnt++;

    return cnt;
}


FileInfoSortedBySizeIterator::FileInfoSortedBySizeIterator( FileInfo	  * parent,
							    FileSize	    minSize,
							    Qt::SortOrder   sortOrder )
{
    _currentIndex = 0;
    FileInfoIterator it( parent );

    while ( *it )
    {
	if ( (*it)->totalSize() >= minSize )
	    _sortedChildren << *it;

	++it;
    }

    std::stable_sort( _sortedChildren.begin(),
		      _sortedChildren.end(),
		      FileInfoSorter( SizeCol, sortOrder ) );

}


FileInfo * FileInfoSortedBySizeIterator::current()
{
    if ( _currentIndex >= 0 && _currentIndex < _sortedChildren.size() )
	return _sortedChildren.at( _currentIndex );
    else
	return 0;
}


void FileInfoSortedBySizeIterator::next()
{
    // Intentionally letting _currentIndex move one position after the last so
    // current() will return 0 to indicate we are finished.

    if ( _currentIndex < _sortedChildren.size() )
	_currentIndex++;
}

