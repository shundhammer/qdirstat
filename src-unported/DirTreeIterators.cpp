/*
 *   File name:	DirTreeIterators.cpp
 *   Summary:	Support classes for QDirStat - DirTree iterator classes
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "DirTreeIterators.h"
#include "DirTree.h"


using namespace QDirStat;


FileInfoIterator::FileInfoIterator( KFileInfo *	parent,
				      KDotEntryPolicy	dotEntryPolicy )
{
    init( parent,
	  dotEntryPolicy,
	  true );		// callNext
}


FileInfoIterator::FileInfoIterator( KFileInfo *	parent,
				      KDotEntryPolicy	dotEntryPolicy,
				      bool		callNext )
{
    init( parent, dotEntryPolicy, callNext );
}


void
FileInfoIterator::init( FileInfo *		parent,
			 KDotEntryPolicy	dotEntryPolicy,
			 bool			callNext )
{
    _parent	= parent;
    _policy	= dotEntryPolicy;
    _current	= 0;

    _directChildrenProcessed	= false;
    _dotEntryProcessed		= false;
    _dotEntryChildrenProcessed	= false;

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
	    // logDebug() << k_funcinfo << " direct child " << _current << endl;
	}
    }
    else	// _directChildrenProcessed
    {
	if ( ! _dotEntryProcessed )
	{
	    // Process dot entry

	    _current = _policy == KDotEntryAsSubDir ? _parent->dotEntry() : 0;
	    _dotEntryProcessed = true;

	    if ( ! _current )
	    {
		next();
	    }
	    else
	    {
		// logDebug() << k_funcinfo << " dot entry " << _current << endl;
	    }
	}
	else	// Dot entry already processed or processing it not desired
	{
	    if ( ! _dotEntryChildrenProcessed )
	    {
		if ( _policy == KDotEntryTransparent )
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
			// logDebug() << k_funcinfo << " dot entry child " << _current << endl;
		    }
		}
		else	// _policy != KDotEntryTransparent
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
	case KDotEntryTransparent:	// Count the dot entry's children as well.
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

	case KDotEntryAsSubDir:		// The dot entry counts as one item.
	    if ( _parent->dotEntry() )
		cnt++;
	    break;

	case KDotEntryIgnore:		// We're done.
	    break;
    }

    return cnt;
}






FileInfoSortedIterator::FileInfoSortedIterator( KFileInfo *		parent,
						  KDotEntryPolicy	dotEntryPolicy,
						  FileInfoSortOrder	sortOrder,
						  bool			ascending )
    : FileInfoIterator( parent, dotEntryPolicy, false )
{
    _sortOrder			= sortOrder;
    _ascending			= ascending;
    _initComplete		= false;
    _childrenList		= 0;
    _current			= 0;
}


void
FileInfoSortedIterator::delayedInit()
{
    _childrenList = new FileInfoList( _sortOrder, _ascending );
    CHECK_PTR( _childrenList );

    if ( _sortOrder == KSortByName )
    {
	makeDefaultOrderChildrenList();
    }
    else
    {
	makeChildrenList();
    }

    _current = _childrenList->first();
    _initComplete = true;
}


FileInfoSortedIterator::~FileInfoSortedIterator()
{
    if ( _childrenList )
	delete _childrenList;
}


void FileInfoSortedIterator::makeDefaultOrderChildrenList()
{
    // Fill children list with direct children

    FileInfo *child = _parent->firstChild();

    while ( child )
    {
	_childrenList->append( child );
	child = child->next();
    }

    _childrenList->sort();

    if ( _policy == KDotEntryAsSubDir && _parent->dotEntry() )
    {
	// Append dot entry to the children list

	_childrenList->append( _parent->dotEntry() );
    }


    // Append the dot entry's children to the children list

    if ( _policy == KDotEntryTransparent && _parent->dotEntry() )
    {
	// Create a temporary list for the dot entry children

	FileInfoList dotEntryChildrenList( _sortOrder, _ascending );
	child = _parent->dotEntry()->firstChild();

	while ( child )
	{
	    dotEntryChildrenList.append( child );
	    child = child->next();
	}

	dotEntryChildrenList.sort();


	// Now append all of this dot entry children list to the children list

	child = dotEntryChildrenList.first();

	while ( child )
	{
	    _childrenList->append( child );
	    child = dotEntryChildrenList.next();
	}
    }
}


void
FileInfoSortedIterator::makeChildrenList()
{
    FileInfoIterator it( _parent, _policy );

    while ( *it )
    {
	_childrenList->append( *it );
	++it;
    }

    _childrenList->sort();
}


FileInfo *
FileInfoSortedIterator::current()
{
    if ( ! _initComplete )
	delayedInit();

    return _current;
}


void FileInfoSortedIterator::next()
{
    if ( ! _initComplete )
	delayedInit();

    _current = _childrenList->next();
}


bool
FileInfoSortedIterator::finished()
{
    if ( ! _initComplete )
	delayedInit();

    return _current == 0;
}






FileInfoSortedBySizeIterator::FileInfoSortedBySizeIterator( KFileInfo *		parent,
							      KFileSize			minSize,
							      KDotEntryPolicy		dotEntryPolicy,
							      bool			ascending )
    : FileInfoSortedIterator( parent, dotEntryPolicy, KSortByTotalSize, ascending )
    , _minSize( minSize )
{
}


void
FileInfoSortedBySizeIterator::makeChildrenList()
{
    FileInfoIterator it( _parent, _policy );

    while ( *it )
    {
	if ( (*it)->totalSize() >= _minSize )
	    _childrenList->append( *it );

	++it;
    }

    _childrenList->sort();
}






FileInfoList::FileInfoList( KFileInfoSortOrder sortOrder, bool ascending )
    : QPtrList<FileInfo>()
{
    _sortOrder	= sortOrder;
    _ascending	= ascending;
}


FileInfoList::~FileInfoList()
{
    // NOP
}



KFileSize
FileInfoList::sumTotalSizes()
{
    KFileSize sum = 0;
    FileInfoListIterator it( *this );
    
    while ( *it )
    {
	sum += (*it)->totalSize();
	++it;
    }

    return sum;
}



int
FileInfoList::compareItems( QCollection::Item it1, QCollection::Item it2 )
{
    if ( it1 == it2 )
	return 0;

    FileInfo *file1 = (KFileInfo *) it1;
    FileInfo *file2 = (KFileInfo *) it2;

    int result = 0;

    switch ( _sortOrder )
    {
	case KUnsorted:
	    return 1;

	case KSortByName:
	    result = QString::compare( file1->name(), file2->name() );
	    break;

	case KSortByTotalSize:
	    result = compare<KFileSize>( file1->totalSize(), file2->totalSize() );
	    break;

	case KSortByLatestMtime:
	    result = compare<time_t>( file1->latestMtime(), file2->latestMtime() );
	    break;
    }

    return _ascending ? result : -result;
}




// EOF
