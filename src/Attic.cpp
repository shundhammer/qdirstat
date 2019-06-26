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
    _name = atticName();

    delete _dotEntry;
    _dotEntry = 0;
}


Attic::~Attic()
{

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
