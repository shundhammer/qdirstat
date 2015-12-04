/*
 *   File name:	kcleanupcollection.cpp
 *   Summary:	Support classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2007-02-11
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <klocale.h>
#include "kcleanup.h"
#include "kstdcleanup.h"
#include "kcleanupcollection.h"


using namespace KDirStat;


KCleanupCollection::KCleanupCollection( KActionCollection * actionCollection )
    : QObject()
    , _actionCollection( actionCollection )
{
    /**
     * All cleanups beloningt to this collection are stored in two separate Qt
     * collections, a QList and a QDict. Make _one_ of them manage the cleanup
     * objects, i.e. have them clear the KCleanup objects upon deleting. The
     * QList is the master collection, the QDict the slave.
     **/
    
    _cleanupList.setAutoDelete( true  );
    _cleanupDict.setAutoDelete( false );
    
    _nextUserCleanupNo	= 0;
}


KCleanupCollection::KCleanupCollection( const KCleanupCollection &src )
    : QObject()
{
    deepCopy( src );
    
    // Keep consistent with the KCleanup copy constructor: It explicitly uses a
    // zero KActionCollecton to make sure no duplicates of cleanups get into
    // the action collection.
    _actionCollection	 = 0;
}


KCleanupCollection::~KCleanupCollection()
{
    // No need to delete the cleanups: _cleanupList takes care of that
    // (autoDelete!).  
}


KCleanupCollection &
KCleanupCollection::operator= ( const KCleanupCollection &src )
{
    if ( size() != src.size() )
    {
	/**
	 * If the sizes are different, we really need to make a deep copy -
	 * i.e. discard all the existing cleanups in this collection and create
	 * new ones with the KCleanup copy constructor.
	 **/
	
	// kdDebug() << k_funcinfo << "Sizes different - deep copy" << endl;
	
	deepCopy( src );
    }
    else
    {
	/**
	 * If the sizes are the same, we'd rather just use the KCleanup
	 * assignment operator to individually assign each cleanup in the
	 * source collection to the corresponding one in this collection.
	 *
	 * The background of this seemingly awkward solution are (again) the
	 * limitations of the KCleanup copy constructor: It doesn't make a
	 * truly identical copy of the entire KCleanup object. Rather, it
	 * copies only the KCleanup members and leaves most of the KAction
	 * members (the parent class) untouched.
	 *
	 * The behaviour implemented here comes handy in the most common
	 * situation where this assignment operator is used:
	 *
	 *	KCleanupCollection tmpCollection( origCollection );
	 *	...
	 *	... // let use change settings in settings dialog
	 *	...
	 *	origCollection = tmpCollection;
	 *
	 * 'tmpCollection' here is an incomplete copy of 'origCollection' -
	 * which represents what the user really can see in the menus, i.e. all
	 * the KAction stuff in there really needs to work.
	 *
	 * During changing preferences in the 'settings' dialog, the user only
	 * changes 'tmpCollection' - if he chooses to abandon his changes
	 * (e.g., he clicks on the 'cancel' button), no harm is done -
	 * 'tmpCollection' is simply not copied back to
	 * 'origCollection'. Anyway, since 'tmpCollection' is merely a
	 * container for the true KCleanup members, the KAction members don't
	 * matter here: There is no representation of 'tmpCollection' in any
	 * menu or tool bar.
	 *
	 * As soon as the user clicks on 'apply' or 'ok' in the 'settings'
	 * dialog, however, 'tmpCollection' is copied back to 'origCollection'
	 * - that is, its KCleanup members. Most of the KAction members (other
	 * than 'text()' which is explicitly copied back) remain untouched,
	 * thus maintaining consistency with the user interface is guaranteed.
	 **/
	
	// kdDebug() << k_funcinfo << "Same sizes - individual assignment" << endl;
	
	KCleanupList srcList = src.cleanupList();
	KCleanupListIterator srcIt( srcList );
	KCleanupListIterator destIt( _cleanupList );

	while ( *srcIt && *destIt )
	{
	    // kdDebug() << "Assigning " << *srcIt << endl;
	    **destIt = **srcIt;
	    ++srcIt;
	    ++destIt;
	}
    }

    // Intentionally leaving '_actionCollection' untouched!
    
    return *this;
}


void
KCleanupCollection::deepCopy( const KCleanupCollection &src )
{
    // Copy simple values
    _nextUserCleanupNo	 = src.nextUserCleanupNo();

    // Just to make sure - clear the internal collections
    _cleanupList.clear();
    _cleanupDict.clear();

    
    // Make a deep copy of all the cleanups in the source collection

    KCleanupList srcList = src.cleanupList();
    KCleanupListIterator it( srcList );

    while ( *it )
    {
	// kdDebug() << k_funcinfo << "Creating new " << *it << endl;
	
	add( new KCleanup( **it ) );
	++it;
    }
}


void
KCleanupCollection::add( KCleanup *newCleanup )
{
    CHECK_PTR( newCleanup );
    
    if ( _cleanupDict[ newCleanup->id() ] )	// Already there?
    {
	// Delete any old instance in the list.
	//
	// The instance in the dict will be deleted automatically by inserting
	// the new one.

	_cleanupList.first();	// Moves _cleanupList.current() to beginning
	
	while ( _cleanupList.current() )
	{
	    if ( _cleanupList.current()->id() == newCleanup->id() )
	    {
		// Found a cleanup with the same ID -
		// remove the current list item, delete it (autoDelete!) and
		// move _cleanupList.current() to the next item.
		
		_cleanupList.remove();
	    }
	    else
		_cleanupList.next();
	}
    }
    
    _cleanupList.append( newCleanup );
    _cleanupDict.insert( newCleanup->id(), newCleanup );

    connect( this,       SIGNAL( selectionChanged( KFileInfo * ) ),
	     newCleanup, SLOT  ( selectionChanged( KFileInfo * ) ) );
    
    connect( this,       SIGNAL( readConfig() ),
	     newCleanup, SLOT  ( readConfig() ) );
    
    connect( this,       SIGNAL( saveConfig() ),
	     newCleanup, SLOT  ( saveConfig() ) );
    
    connect( newCleanup, SIGNAL( executed() ),
	     this, 	 SLOT  ( cleanupExecuted() ) );
}


void
KCleanupCollection::addStdCleanups()
{
    add( KStdCleanup::openInKonqueror	( _actionCollection ) );
    add( KStdCleanup::openInTerminal	( _actionCollection ) );
    add( KStdCleanup::compressSubtree	( _actionCollection ) );
    add( KStdCleanup::makeClean		( _actionCollection ) );
    add( KStdCleanup::deleteTrash	( _actionCollection ) );
    add( KStdCleanup::moveToTrashBin	( _actionCollection ) );
    add( KStdCleanup::hardDelete	( _actionCollection ) );
}


void
KCleanupCollection::addUserCleanups( int number )
{
    for ( int i=0; i < number; i++ )
    {
	QString id;
	id.sprintf( "cleanup_user_defined_%d", _nextUserCleanupNo );
	QString title;

	if ( _nextUserCleanupNo <= 9 )
	    // Provide a keyboard shortcut for cleanup #0..#9
	    title=i18n( "User Defined Cleanup #&%1" ).arg(_nextUserCleanupNo);
	else
	    // No keyboard shortcuts for cleanups #10.. - they would be duplicates
	    title=i18n( "User Defined Cleanup #%1" ).arg(_nextUserCleanupNo);

	_nextUserCleanupNo++;
	
	KCleanup *cleanup = new KCleanup( id, "", title, _actionCollection );
	CHECK_PTR( cleanup );
	cleanup->setEnabled( false );

	if ( i <= 9 )
	{
	    // Provide an application-wide keyboard accelerator for cleanup #0..#9
	    cleanup->setShortcut( Qt::CTRL + Qt::Key_0 + i );
	}
	
	add( cleanup );
    }
}


KCleanup *
KCleanupCollection::cleanup( const QString & id )
{
    return _cleanupDict[ id ];
}


void
KCleanupCollection::clear()
{
    _cleanupList.clear();
    _cleanupDict.clear();
    _nextUserCleanupNo = 0;
}


void
KCleanupCollection::slotReadConfig()
{
    emit readConfig();
}


void
KCleanupCollection::cleanupExecuted()
{
    emit userActivity( 10 );
}


// EOF
