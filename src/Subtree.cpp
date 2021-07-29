/*
 *   File name: Subtree.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Subtree.h"
#include "DirInfo.h"

using namespace QDirStat;


FileInfo * Subtree::subtree()
{
    FileInfo * dir = locate();

    if ( ! dir && _useRootFallback && _tree )
	dir = _tree->root();

    return dir;
}


QString Subtree::url() const
{
    if ( _url == "<root>" && _tree )
	return _tree->url();
    else
	return _url;
}


void Subtree::set( FileInfo * subtree )
{
    if ( subtree )
    {
	_tree = subtree->tree();
	_url  = subtree->debugUrl();
    }
    else
    {
	_url.clear();
    }
}


FileInfo * Subtree::locate()
{
    if ( ! _tree || _url.isEmpty() )
	return 0;

    return _tree->locate( _url,
			  true ); // findPseudoDirs
}


void Subtree::clone( const Subtree & other )
{
    if ( &other == this )
	return;

    _tree = other.tree();
    _url  = other.url();

    _useRootFallback = other.useRootFallback();
}
