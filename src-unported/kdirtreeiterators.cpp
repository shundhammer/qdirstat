/*
 *   File name:	kdirtreeiterators.h
 *   Summary:	Support classes for KDirStat - KDirTree iterator classes
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include "kdirtreeiterators.h"
#include "kdirtree.h"


using namespace KDirStat;


KFileInfoIterator::KFileInfoIterator( KFileInfo *	parent,
				      KDotEntryPolicy	dotEntryPolicy )
{
    init( parent,
	  dotEntryPolicy,
	  true );		// callNext
}


KFileInfoIterator::KFileInfoIterator( KFileInfo *	parent,
				      KDotEntryPolicy	dotEntryPolicy,
				      bool		callNext )
{
    init( parent, dotEntryPolicy, callNext );
}


void
KFileInfoIterator::init( KFileInfo *		parent,
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


KFileInfoIterator::~KFileInfoIterator()
{
    // NOP
}


void KFileInfoIterator::next()
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
	    // kdDebug() << k_funcinfo << " direct child " << _current << endl;
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
		// kdDebug() << k_funcinfo << " dot entry " << _current << endl;
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
			// kdDebug() << k_funcinfo << " dot entry child " << _current << endl;
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
KFileInfoIterator::count()
{
    int cnt = 0;

    // Count direct children

    KFileInfo *child = _parent->firstChild();

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






KFileInfoSortedIterator::KFileInfoSortedIterator( KFileInfo *		parent,
						  KDotEntryPolicy	dotEntryPolicy,
						  KFileInfoSortOrder	sortOrder,
						  bool			ascending )
    : KFileInfoIterator( parent, dotEntryPolicy, false )
{
    _sortOrder			= sortOrder;
    _ascending			= ascending;
    _initComplete		= false;
    _childrenList		= 0;
    _current			= 0;
}


void
KFileInfoSortedIterator::delayedInit()
{
    _childrenList = new KFileInfoList( _sortOrder, _ascending );
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


KFileInfoSortedIterator::~KFileInfoSortedIterator()
{
    if ( _childrenList )
	delete _childrenList;
}


void KFileInfoSortedIterator::makeDefaultOrderChildrenList()
{
    // Fill children list with direct children

    KFileInfo *child = _parent->firstChild();

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

	KFileInfoList dotEntryChildrenList( _sortOrder, _ascending );
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
KFileInfoSortedIterator::makeChildrenList()
{
    KFileInfoIterator it( _parent, _policy );

    while ( *it )
    {
	_childrenList->append( *it );
	++it;
    }

    _childrenList->sort();
}


KFileInfo *
KFileInfoSortedIterator::current()
{
    if ( ! _initComplete )
	delayedInit();

    return _current;
}


void KFileInfoSortedIterator::next()
{
    if ( ! _initComplete )
	delayedInit();

    _current = _childrenList->next();
}


bool
KFileInfoSortedIterator::finished()
{
    if ( ! _initComplete )
	delayedInit();

    return _current == 0;
}






KFileInfoSortedBySizeIterator::KFileInfoSortedBySizeIterator( KFileInfo *		parent,
							      KFileSize			minSize,
							      KDotEntryPolicy		dotEntryPolicy,
							      bool			ascending )
    : KFileInfoSortedIterator( parent, dotEntryPolicy, KSortByTotalSize, ascending )
    , _minSize( minSize )
{
}


void
KFileInfoSortedBySizeIterator::makeChildrenList()
{
    KFileInfoIterator it( _parent, _policy );

    while ( *it )
    {
	if ( (*it)->totalSize() >= _minSize )
	    _childrenList->append( *it );

	++it;
    }

    _childrenList->sort();
}






KFileInfoList::KFileInfoList( KFileInfoSortOrder sortOrder, bool ascending )
    : QPtrList<KFileInfo>()
{
    _sortOrder	= sortOrder;
    _ascending	= ascending;
}


KFileInfoList::~KFileInfoList()
{
    // NOP
}



KFileSize
KFileInfoList::sumTotalSizes()
{
    KFileSize sum = 0;
    KFileInfoListIterator it( *this );
    
    while ( *it )
    {
	sum += (*it)->totalSize();
	++it;
    }

    return sum;
}



int
KFileInfoList::compareItems( QCollection::Item it1, QCollection::Item it2 )
{
    if ( it1 == it2 )
	return 0;

    KFileInfo *file1 = (KFileInfo *) it1;
    KFileInfo *file2 = (KFileInfo *) it2;

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
