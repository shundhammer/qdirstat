/*
 *   File name: Refresher.h
 *   Summary:	Helper class to refresh a number of subtrees
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Refresher.h"
#include "DirTree.h"
#include "Logger.h"

using namespace QDirStat;


Refresher::Refresher( const FileInfoSet items, QObject * parent ):
    QObject( parent ),
    _items( items )
{
    logDebug() << "Creating refresher for " << _items.first() << endl;
}


void Refresher::refresh()
{
    if ( ! _items.isEmpty() )
    {
	logDebug() << "Refreshing " << _items.first() << endl;
	DirTree * tree = _items.first()->tree();
	tree->refresh( _items );
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
	    parents << child->parent();
    }

    return parents.normalized();
}
