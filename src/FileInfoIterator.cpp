/*
 *   File name: FileInfoIterators.cpp
 *   Summary:	Support classes for QDirStat - DirTree iterator classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileInfoIterator.h"
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


FileInfoIterator::~FileInfoIterator()
{
    // NOP
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

