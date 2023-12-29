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
    FileInfo * item = locate();

    if ( ! item && _useRootFallback && _tree )
	item = _tree->firstToplevel();

    return item;
}


DirInfo * Subtree::dir()
{
    FileInfo * item = subtree();

    if ( ! item )
        return 0;

    DirInfo * dir = item->toDirInfo();

    if ( ! dir && item->parent() )
        dir = item->parent();

    if ( dir && _tree && dir == _tree->root() )
        dir = 0;

    return dir;
}


QString Subtree::url() const
{
    if ( _url == "<root>" && _tree )
	return _tree->url();
    else
	return _url;
}


void Subtree::setUrl( const QString & newUrl )
{
    logDebug() << "URL: " << newUrl << endl;
    _url = newUrl;

    if ( ! _tree )
        logWarning() << "NULL tree!" << endl;
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
