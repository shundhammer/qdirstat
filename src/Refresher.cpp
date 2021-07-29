/*
 *   File name: Refresher.h
 *   Summary:	Helper class to refresh a number of subtrees
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Refresher.h"
#include "DirTree.h"
#include "DirInfo.h"
#include "FileInfoSet.h"
#include "Logger.h"

using namespace QDirStat;


Refresher::Refresher( const FileInfoSet items, QObject * parent ):
    QObject( parent ),
    _items( items ),
    _tree( 0 )
{
    // logDebug() << "Creating refresher for " <<  _items.size() << " items" << endl;

    // Storing the tree right now in a separate variable because by the time we
    // need it (in refresh()) any (or even all) of the items might have become
    // invalid already, so any attempt to dereference them to obtain the tree
    // from there might result in a segfault.

    if ( ! _items.isEmpty() )
	_tree = _items.first()->tree();
}


void Refresher::refresh()
{
    if ( ! _items.isEmpty() && _tree )
    {
	logDebug() << "Refreshing " << _items.size() << " items" << endl;

	_tree->refresh( _items );
    }
    else
    {
	logWarning() << "No items to refresh" << endl;
    }

    this->deleteLater();
}


FileInfoSet Refresher::parents( const FileInfoSet children )
{
    FileInfoSet parents;

    foreach ( FileInfo * child, children )
    {
	if ( child && child->parent() )
        {
            FileInfo * parent = child->parent();

            if ( parent->isPseudoDir() )
                parent = parent->parent();

            if ( parent )
                parents << parent;
        }
    }

    return parents.normalized();
}

