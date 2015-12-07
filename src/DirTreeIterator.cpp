/*
 *   File name: DirTreeIterators.cpp
 *   Summary:	Support classes for QDirStat - DirTree iterator classes
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DirTreeIterator.h"
#include "Exception.h"


using namespace QDirStat;



FileInfoIterator::FileInfoIterator( FileInfo *	   parent,
				    DotEntryPolicy dotEntryPolicy )
{
    init( parent,
	  dotEntryPolicy,
	  true );		// callNext
}


FileInfoIterator::FileInfoIterator( FileInfo *	   parent,
				    DotEntryPolicy dotEntryPolicy,
				    bool	   callNext )
{
    init( parent, dotEntryPolicy, callNext );
}


void
FileInfoIterator::init( FileInfo *     parent,
			DotEntryPolicy dotEntryPolicy,
			bool	       callNext )
{
    _parent  = parent;
    _policy  = dotEntryPolicy;
    _current = 0;

    _directChildrenProcessed   = false;
    _dotEntryProcessed	       = false;
    _dotEntryChildrenProcessed = false;

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
	else
	{
	    // logDebug() << " direct child " << _current << endl;
	}
    }
    else	// _directChildrenProcessed
    {
	if ( ! _dotEntryProcessed )
	{
	    // Process dot entry

	    _current = _policy == DotEntryIsSubDir ? _parent->dotEntry() : 0;
	    _dotEntryProcessed = true;

	    if ( ! _current )
	    {
		next();
	    }
	    else
	    {
		// logDebug() << " dot entry " << _current << endl;
	    }
	}
	else	// Dot entry already processed or processing it not desired
	{
	    if ( ! _dotEntryChildrenProcessed )
	    {
		if ( _policy == DotEntryTransparent )
		{
		    // Process dot entry children

		    _current = _current ?
			_current->next() :
			( _parent->dotEntry() ? _parent->dotEntry()->firstChild() : 0 );

		    if ( ! _current )
		    {
			_dotEntryChildrenProcessed = true;
		    }
		    else
		    {
			// logDebug() << " dot entry child " << _current << endl;
		    }
		}
		else	// _policy != DotEntryTransparent
		{
		    _current = 0;
		    _dotEntryChildrenProcessed = true;
		}
	    }
	}
    }
}


int
FileInfoIterator::count()
{
    int cnt = 0;

    // Count direct children

    FileInfo *child = _parent->firstChild();

    while ( child )
    {
	cnt++;
	child = child->next();
    }


    // Handle the dot entry

    switch ( _policy )
    {
	case DotEntryTransparent:	// Count the dot entry's children as well.
	    if ( _parent->dotEntry() )
	    {
		child = _parent->dotEntry()->firstChild();

		while ( child )
		{
		    cnt++;
		    child = child->next();
		}
	    }
	    break;

	case DotEntryIsSubDir:		// The dot entry counts as one item.
	    if ( _parent->dotEntry() )
		cnt++;
	    break;

	case DotEntryIgnore:		// We're done.
	    break;
    }

    return cnt;
}

