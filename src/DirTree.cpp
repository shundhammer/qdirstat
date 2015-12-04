/*
 *   File name:	DirTree.cpp
 *   Summary:	Support classes for QDirStat
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DirTree.h"

#if 0
#include "DirReadJob.h"
#include "DirTreeCache.h"
#endif

using namespace QDirStat;



DirTree::KDirTree()
    : QObject()
{
    _root		= 0;
    _selection		= 0;
    _isBusy		= false;

    readConfig();

    connect( & _jobQueue, SIGNAL( finished()     ),
	     this,        SLOT  ( slotFinished() ) );
}


DirTree::~KDirTree()
{
    selectItem( 0 );

    if ( _root )
	delete _root;
}


void
DirTree::readConfig()
{
    KConfig * config = kapp->config();
    config->setGroup( "Directory Reading" );

    _crossFileSystems		= config->readBoolEntry( "CrossFileSystems",     false );
    _enableLocalDirReader	= config->readBoolEntry( "EnableLocalDirReader", true  );
}


void
DirTree::setRoot( FileInfo *newRoot )
{
    if ( _root )
    {
	selectItem( 0 );
	emit deletingChild( _root );
	delete _root;
	emit childDeleted();
    }

    _root = newRoot;
}


void
DirTree::clear( bool sendSignals )
{
    _jobQueue.clear();

    if ( _root )
    {
	selectItem( 0 );

	if ( sendSignals )
	    emit deletingChild( _root );

	delete _root;
	_root = 0;

	if ( sendSignals )
	    emit childDeleted();
    }

    _isBusy = false;
}


void
DirTree::startReading( const KURL & url )
{
    // logDebug() << k_funcinfo << " " << url.url() << endl;

#if 0
    logDebug() << "url: "		<< url.url()		<< endl;
    logDebug() << "path: "		<< url.path()		<< endl;
    logDebug() << "filename: "		<< url.filename() 	<< endl;
    logDebug() << "protocol: "		<< url.protocol() 	<< endl;
    logDebug() << "isValid: "		<< url.isValid() 	<< endl;
    logDebug() << "isMalformed: "	<< url.isMalformed() 	<< endl;
    logDebug() << "isLocalFile: "	<< url.isLocalFile() 	<< endl;
#endif

    _isBusy = true;
    emit startingReading();

    setRoot( 0 );
    readConfig();

    _root = LocalDirReadJob::stat( url, this );
    Q_CHECK_PTR( _root );

    if ( _root )
    {
	childAddedNotify( _root );

	if ( _root->isDir() )
	{
	    DirInfo *dir = (KDirInfo *) _root;
            addJob( new LocalDirReadJob( this, dir ) );
	}
	else
	{
	    _isBusy = false;
	    emit finished();
	}
    }
    else	// stat() failed
    {
	// logWarning() << "stat(" << url.url() << ") failed" << endl;
	_isBusy = false;
	emit finished();
	emit finalizeLocal( 0 );
    }
}


void
DirTree::refresh( FileInfo *subtree )
{
    if ( ! _root )
	return;

    if ( ! subtree || ! subtree->parent() )	// Refresh all (from root)
    {
	startReading( fixedUrl( _root->url() ) );
    }
    else	// Refresh subtree
    {
	// Save some values from the old subtree.

	KURL url		= subtree->url();
	DirInfo * parent	= subtree->parent();


	// Select nothing if the current selection is to be deleted

	if ( _selection && _selection->isInSubtree( subtree ) )
	    selectItem( 0 );


	// Clear any old "excluded" status

	subtree->setExcluded( false );

	
	// Get rid of the old subtree.

	emit deletingChild( subtree );

	// logDebug() << "Deleting subtree " << subtree << endl;

	/**
	 * This may sound stupid, but the parent must be told to unlink its
	 * child from the children list. The child cannot simply do this by
	 * itself in its destructor since at this point important parts of the
	 * object may already be destroyed, e.g., the virtual table -
	 * i.e. virtual methods won't work any more.
	 *
	 * I just found that out the hard way by several hours of debugging. ;-}
	 **/
	parent->deletingChild( subtree );
	delete subtree;
	emit childDeleted();

	_isBusy = true;
	emit startingReading();
	
	// Create new subtree root.

	subtree = LocalDirReadJob::stat( url, this, parent );

	// logDebug() << "New subtree: " << subtree << endl;

	if ( subtree )
	{
	    // Insert new subtree root into the tree hierarchy.

	    parent->insertChild( subtree );
	    childAddedNotify( subtree );

	    if ( subtree->isDir() )
	    {
		// Prepare reading this subtree's contents.

		DirInfo *dir = (KDirInfo *) subtree;
                addJob( new LocalDirReadJob( this, dir ) );
	    }
	    else
	    {
		_isBusy = false;
		emit finished();
	    }
	}
    }
}


void
DirTree::abortReading()
{
    if ( _jobQueue.isEmpty() )
	return;

    _jobQueue.abort();

    _isBusy = false;
    emit aborted();
}


void
DirTree::slotFinished()
{
    _isBusy = false;
    emit finished();
}


void
DirTree::childAddedNotify( FileInfo *newChild )
{
    emit childAdded( newChild );

    if ( newChild->dotEntry() )
	emit childAdded( newChild->dotEntry() );
}


void
DirTree::deletingChildNotify( FileInfo *deletedChild )
{
    emit deletingChild( deletedChild );

    // Only now check for selection and root: Give connected objects
    // (i.e. views) a chance to change either while handling the signal.

    if ( _selection && _selection->isInSubtree( deletedChild ) )
	 selectItem( 0 );

    if ( deletedChild == _root )
	_root = 0;
}


void
DirTree::childDeletedNotify()
{
    emit childDeleted();
}


void
DirTree::deleteSubtree( FileInfo *subtree )
{
    // logDebug() << "Deleting subtree " << subtree << endl;
    DirInfo *parent = subtree->parent();

    if ( parent )
    {
	// Give the parent of the child to be deleted a chance to unlink the
	// child from its children list and take care of internal summary
	// fields
	parent->deletingChild( subtree );
    }

    // Send notification to anybody interested (e.g., to attached views)
    deletingChildNotify( subtree );

    if ( parent )
    {
	if ( parent->isDotEntry() && ! parent->hasChildren() )
	    // This was the last child of a dot entry
	{
	    // Get rid of that now empty and useless dot entry

	    if ( parent->parent() )
	    {
		if ( parent->parent()->isFinished() )
		{
		    // logDebug() << "Removing empty dot entry " << parent << endl;

		    deletingChildNotify( parent );
		    parent->parent()->setDotEntry( 0 );

		    delete parent;
		}
	    }
	    else	// no parent - this should never happen (?)
	    {
		logError() << "Internal error: Killing dot entry without parent " << parent << endl;

		// Better leave that dot entry alone - we shouldn't have come
		// here in the first place. Who knows what will happen if this
		// thing is deleted now?!
		//
		// Intentionally NOT calling:
		//     delete parent;
	    }
	}
    }

    delete subtree;

    if ( subtree == _root )
    {
	selectItem( 0 );
	_root = 0;
    }

    emit childDeleted();
}


void
DirTree::addJob( DirReadJob * job )
{
    _jobQueue.enqueue( job );
}


void
DirTree::sendProgressInfo( const QString &infoLine )
{
    emit progressInfo( infoLine );
}


void
DirTree::sendFinalizeLocal( DirInfo *dir )
{
    emit finalizeLocal( dir );
}


void
DirTree::sendStartingReading()
{
    emit startingReading();
}


void
DirTree::sendFinished()
{
    emit finished();
}


void
DirTree::sendAborted()
{
    emit aborted();
}


void
DirTree::selectItem( FileInfo *newSelection )
{
    if ( newSelection == _selection )
	return;

#if 0
    if ( newSelection )
	logDebug() << k_funcinfo << " selecting " << newSelection << endl;
    else
	logDebug() << k_funcinfo << " selecting nothing" << endl;
#endif

    _selection = newSelection;
    emit selectionChanged( _selection );
}


bool
DirTree::writeCache( const QString & cacheFileName )
{
    CacheWriter writer( cacheFileName, this );
    return writer.ok();
}


void
DirTree::readCache( const QString & cacheFileName )
{
    _isBusy = true;
    emit startingReading();
    addJob( new CacheReadJob( this, 0, cacheFileName ) );
}



// EOF
