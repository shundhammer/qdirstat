/*
 *   File name: Attic.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Attic.h"
#include "DotEntry.h"
#include "Exception.h"
#include "Logger.h"


using namespace QDirStat;


Attic::Attic( DirTree * tree,
	      DirInfo * parent )
    : DirInfo( tree, parent )
{
    _name      = atticName();
    _isIgnored = true;

    if ( parent )
    {
	_device = parent->device();
	_mode	= parent->mode();
	_uid	= parent->uid();
	_gid	= parent->gid();
	_mtime	= 0;
    }
}


Attic::~Attic()
{
    // NOP
}


DirReadState Attic::readState() const
{
    if ( _parent )
	return _parent->readState();
    else // This should never happen
	return _readState;
}


bool Attic::hasChildren() const
{
    return firstChild();
}


void Attic::checkIgnored()
{
    // NOP
}


FileInfo * Attic::locate( QString url, bool findPseudoDirs )
{
    if ( ! _tree || ! _parent )
	return 0;

    // Search all children

    FileInfo * child = firstChild();

    while ( child )
    {
	FileInfo * foundChild = child->locate( url, findPseudoDirs );

	if ( foundChild )
	    return foundChild;
	else
	    child = child->next();
    }

    // An attic can have neither an attic nor a dot entry, so there is no need
    // to search in either of those.

    return 0;
}

