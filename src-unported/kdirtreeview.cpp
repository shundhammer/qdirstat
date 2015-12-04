/*
 *   File name:	kdirtreeview.cpp
 *   Summary:	High level classes for KDirStat
 *   License:	LGPL - See file COPYING.LIB for details.
 *   Author:	Stefan Hundhammer <sh@suse.de>
 *
 *   Updated:	2008-12-18
 */


#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <time.h>
#include <stdlib.h>

#include <qtimer.h>
#include <qcolor.h>
#include <qheader.h>
#include <qpopupmenu.h>

#include <kapp.h>
#include <klocale.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kicontheme.h>
#include <kiconloader.h>
#include <kexcluderules.h>

#include "kdirtreeview.h"
#include "kdirreadjob.h"
#include "kdirtreeiterators.h"
#include "kpacman.h"

#define SEPARATE_READ_JOBS_COL	0
#define VERBOSE_PROGRESS_INFO	0

using namespace KDirStat;


KDirTreeView::KDirTreeView( QWidget * parent )
    : KDirTreeViewParentClass( parent )
{
    _tree		= 0;
    _updateTimer	= 0;
    _selection		= 0;
    _openLevel		= 1;
    _doLazyClone	= true;
    _doPacManAnimation	= false;
    _updateInterval	= 333;	// millisec
    _sortCol		= -1;

    for ( int i=0; i < DEBUG_COUNTERS; i++ )
	_debugCount[i]	= 0;

    setDebugFunc( 1, "KDirTreeViewItem::init()" );
    setDebugFunc( 2, "KDirTreeViewItem::updateSummary()" );
    setDebugFunc( 3, "KDirTreeViewItem::deferredClone()" );
    setDebugFunc( 4, "KDirTreeViewItem::compare()" );
    setDebugFunc( 5, "KDirTreeViewItem::paintCell()" );

#if SEPARATE_READ_JOBS_COL
    _readJobsCol	= -1;
#endif
    setRootIsDecorated( false );

    int numCol = 0;
    addColumn( i18n( "Name"			) ); _nameCol		= numCol;
    _iconCol = numCol++;
    addColumn( i18n( "Subtree Percentage" 	) ); _percentBarCol	= numCol++;
    addColumn( i18n( "Percentage"		) ); _percentNumCol	= numCol++;
    addColumn( i18n( "Subtree Total"		) ); _totalSizeCol	= numCol++;
    _workingStatusCol = _totalSizeCol;
    addColumn( i18n( "Own Size"			) ); _ownSizeCol	= numCol++;
    addColumn( i18n( "Items"			) ); _totalItemsCol	= numCol++;
    addColumn( i18n( "Files"			) ); _totalFilesCol	= numCol++;
    addColumn( i18n( "Subdirs"			) ); _totalSubDirsCol	= numCol++;
    addColumn( i18n( "Last Change"		) ); _latestMtimeCol	= numCol++;

#if ! SEPARATE_READ_JOBS_COL
    _readJobsCol = _percentBarCol;
#endif

    setColumnAlignment ( _totalSizeCol,		AlignRight );
    setColumnAlignment ( _percentNumCol,	AlignRight );
    setColumnAlignment ( _ownSizeCol,		AlignRight );
    setColumnAlignment ( _totalItemsCol,	AlignRight );
    setColumnAlignment ( _totalFilesCol,	AlignRight );
    setColumnAlignment ( _totalSubDirsCol,	AlignRight );
    setColumnAlignment ( _readJobsCol,		AlignRight );


    setSorting( _totalSizeCol );


#define loadIcon(ICON)	KGlobal::iconLoader()->loadIcon( (ICON), KIcon::Small )

    _openDirIcon	= loadIcon( "folder_open" 	);
    _closedDirIcon	= loadIcon( "folder"		);
    _openDotEntryIcon	= loadIcon( "folder_orange_open");
    _closedDotEntryIcon	= loadIcon( "folder_orange"	);
    _unreadableDirIcon	= loadIcon( "folder_locked" 	);
    _mountPointIcon	= loadIcon( "hdd_mount"		);
    _fileIcon		= loadIcon( "mime_empty"	);
    _symLinkIcon	= loadIcon( "symlink"		);	// The KDE standard link icon is ugly!
    _blockDevIcon	= loadIcon( "blockdevice"	);
    _charDevIcon	= loadIcon( "chardevice"	);
    _fifoIcon		= loadIcon( "socket"		);
    _stopIcon		= loadIcon( "stop"		);
    _readyIcon		= QPixmap();

#undef loadIcon

    setDefaultFillColors();
    readConfig();
    ensureContrast();


    connect( kapp,	SIGNAL( kdisplayPaletteChanged()	),
	     this,	SLOT  ( paletteChanged()		) );

    connect( this,	SIGNAL( selectionChanged	( QListViewItem * ) ),
	     this,	SLOT  ( selectItem		( QListViewItem * ) ) );

    connect( this,	SIGNAL( rightButtonPressed	( QListViewItem *, const QPoint &, int ) ),
	     this,	SLOT  ( popupContextMenu	( QListViewItem *, const QPoint &, int ) ) );

    connect( header(),	SIGNAL( sizeChange   ( int, int, int ) ),
	     this,	SLOT  ( columnResized( int, int, int ) ) );

   _contextInfo	  = new QPopupMenu;
   _idContextInfo = _contextInfo->insertItem ( "dummy" );

   createTree();
}


KDirTreeView::~KDirTreeView()
{
    if ( _tree )
	delete _tree;

    /*
     * Don't delete _updateTimer here, it's already automatically deleted by Qt!
     * (Since it's derived from QObject and has a QObject parent).
     */
}


void
KDirTreeView::setDebugFunc( int i, const QString & functionName )
{
    if ( i > 0 && i < DEBUG_COUNTERS )
	_debugFunc[i] = functionName;
}


void
KDirTreeView::incDebugCount( int i )
{
    if ( i > 0 && i < DEBUG_COUNTERS )
	_debugCount[i]++;
}


void
KDirTreeView::busyDisplay()
{
#if SEPARATE_READ_JOBS_COL
    if ( _readJobsCol < 0 )
    {
	_readJobsCol = header()->count();
	addColumn( i18n( "Read Jobs" ) );
	setColumnAlignment( _readJobsCol, AlignRight );
    }
#else
    _readJobsCol = _percentBarCol;
#endif
}


void
KDirTreeView::idleDisplay()
{
#if SEPARATE_READ_JOBS_COL
    if ( _readJobsCol >= 0 )
    {
	removeColumn( _readJobsCol );
    }
#else
    if ( _sortCol == _readJobsCol && _sortCol >= 0 )
    {
	// A pathological case: The user requested sorting by read jobs, and
	// now that everything is read, the items are still in that sort order.
	// Not only is that sort order now useless (since all read jobs are
	// done), it is contrary to the (now changed) semantics of this
	// column. Calling QListView::sort() might do the trick, but we can
	// never know just how clever that QListView widget tries to be and
	// maybe avoid another sorting by the same column - so let's use the
	// easy way out and sort by another column that has the same sorting
	// semantics like the percentage bar column (that had doubled as the
	// read job column while reading) now has.

	setSorting( _percentNumCol );
    }
#endif

    setColumnAlignment ( _percentBarCol, AlignLeft );
    _readJobsCol = -1;
}


void
KDirTreeView::openURL( KURL url )
{
    clear();
    _tree->clear();

    // Implicitly calling prepareReading() via the tree's startingReading() signal
    _tree->startReading( url );

    logActivity( 30 );
}


void
KDirTreeView::createTree()
{
    // Clean up any old leftovers

    clear();
    _currentDir = "";

    if ( _tree )
	delete _tree;


    // Create new (empty) dir tree

    _tree = new KDirTree();


    // Connect signals

    connect( _tree, SIGNAL( progressInfo    ( const QString & ) ),
	     this,  SLOT  ( sendProgressInfo( const QString & ) ) );

    connect( _tree, SIGNAL( childAdded( KFileInfo * ) ),
	     this,  SLOT  ( addChild  ( KFileInfo * ) ) );

    connect( _tree, SIGNAL( deletingChild( KFileInfo * ) ),
	     this,  SLOT  ( deleteChild  ( KFileInfo * ) ) );

    connect( _tree, SIGNAL( startingReading() ),
	     this,  SLOT  ( prepareReading()  ) );

    connect( _tree, SIGNAL( finished()     ),
	     this,  SLOT  ( slotFinished() ) );

    connect( _tree, SIGNAL( aborted()     ),
	     this,  SLOT  ( slotAborted() ) );

    connect( _tree, SIGNAL( finalizeLocal( KDirInfo * ) ),
	     this,  SLOT  ( finalizeLocal( KDirInfo * ) ) );

    connect( this,  SIGNAL( selectionChanged( KFileInfo * ) ),
	     _tree, SLOT  ( selectItem      ( KFileInfo * ) ) );

    connect( _tree, SIGNAL( selectionChanged( KFileInfo * ) ),
	     this,  SLOT  ( selectItem      ( KFileInfo * ) ) );
}


void
KDirTreeView::prepareReading()
{
    // Prepare cyclic update

    if ( _updateTimer )
	delete _updateTimer;

    _updateTimer = new QTimer( this );

    if ( _updateTimer )
    {
	_updateTimer->changeInterval( _updateInterval );
	connect( _updateTimer,	SIGNAL( timeout() ),
		 this,   	SLOT  ( updateSummary() ) );

	connect( _updateTimer, SIGNAL( timeout() ),
		 this,   	SLOT  ( sendProgressInfo() ) );
    }


    // Change display to busy state

    setSorting( _totalSizeCol );
    busyDisplay();
    emit startingReading();

    _stopWatch.start();
}


void
KDirTreeView::refreshAll()
{
    if ( _tree && _tree->root() )
    {
	clear();
	// Implicitly calling prepareReading() via the tree's startingReading() signal
	_tree->refresh( 0 );
    }
}


void
KDirTreeView::refreshSelected()
{
    if ( _tree && _tree->root() && _selection )
    {
	// Implicitly calling prepareReading() via the tree's startingReading() signal
	_tree->refresh( _selection->orig() );
    }

    logActivity( 10 );
}


void
KDirTreeView::abortReading()
{
    if ( _tree )
	_tree->abortReading();
}


void
KDirTreeView::clear()
{
    clearSelection();
    KDirTreeViewParentClass::clear();

    for ( int i=0; i < DEBUG_COUNTERS; i++ )
	_debugCount[i]	= 0;
}


bool
KDirTreeView::writeCache( const QString & cacheFileName )
{
    if ( _tree )
	return _tree->writeCache( cacheFileName );

    return false;
}


void
KDirTreeView::readCache( const QString & cacheFileName )
{
    clear();
    _tree->clear();
    _tree->readCache( cacheFileName );
}


void
KDirTreeView::addChild( KFileInfo *newChild )
{
    if ( newChild->parent() )
    {
	KDirTreeViewItem *cloneParent = locate( newChild->parent(),
						_doLazyClone,		// lazy
						true );			// doClone

	if ( cloneParent )
	{
	    if ( isOpen( cloneParent ) || ! _doLazyClone )
	    {
		// kdDebug() << "Immediately cloning " << newChild << endl;
		new KDirTreeViewItem( this, cloneParent, newChild );
	    }
	}
	else	// Error
	{
	    if ( ! _doLazyClone )
	    {
		kdError() << k_funcinfo << "Can't find parent view item for "
			  << newChild << endl;
	    }
	}
    }
    else	// No parent - top level item
    {
	// kdDebug() << "Immediately top level cloning " << newChild << endl;
	new KDirTreeViewItem( this, newChild );
    }
}


void
KDirTreeView::deleteChild( KFileInfo *child )
{
    KDirTreeViewItem *clone = locate( child,
				      false,	// lazy
				      false );	// doClone
    KDirTreeViewItem *nextSelection = 0;

    if ( clone )
    {
	if ( clone == _selection )
	{
	    /**
	     * The selected item is about to be deleted. Select some other item
	     * so there is still something selected: Preferably the next item
	     * or the parent if there is no next. This cannot be done from
	     * outside because the order of items is not known to the outside;
	     * it might appear very random if the next item in the KFileInfo
	     * list would be selected. The order of that list is definitely
	     * different than the order of this view - which is what the user
	     * sees. So let's give the user a reasonable next selection so he
	     * can continue working without having to explicitly select another
	     * item.
	     *
	     * This is very useful if the user just activated a cleanup action
	     * that deleted an item: It makes sense to implicitly select the
	     * next item so he can clean up many items in a row.
	     **/

	    nextSelection = clone->next() ? clone->next() : clone->parent();
	    // kdDebug() << k_funcinfo << " Next selection: " << nextSelection << endl;
	}

	KDirTreeViewItem *parent = clone->parent();
	delete clone;

	while ( parent )
	{
	    parent->updateSummary();
	    parent = parent->parent();
	}

	if ( nextSelection )
	    selectItem( nextSelection );
    }
}


void
KDirTreeView::updateSummary()
{
    KDirTreeViewItem *child = firstChild();

    while ( child )
    {
	child->updateSummary();
	child = child->next();
    }
}


void
KDirTreeView::slotFinished()
{
    emit progressInfo( i18n( "Finished. Elapsed time: %1" )
		       .arg( formatTime( _stopWatch.elapsed(), true ) ) );

    if ( _updateTimer )
    {
	delete _updateTimer;
	_updateTimer = 0;
    }

    idleDisplay();
    updateSummary();
    logActivity( 30 );

    if ( _tree->root() &&
	 _tree->root()->totalSubDirs() == 0 &&	// No subdirs
	 _tree->root()->totalItems() > 0 )	// but file children
    {
	QListViewItem * root = firstChild();

	if ( root )
	    root->setOpen( true );
    }



#if 0
    for ( int i=0; i < DEBUG_COUNTERS; i++ )
    {
	kdDebug() << "Debug counter #" << i << ": " << _debugCount[i]
		  << "\t" << _debugFunc[i]
		  << endl;
    }
    kdDebug() << endl;
#endif

    emit finished();
}


void
KDirTreeView::slotAborted()
{
    emit progressInfo( i18n( "Aborted. Elapsed time: %1" )
		       .arg( formatTime( _stopWatch.elapsed(), true ) ) );

    if ( _updateTimer )
    {
	delete _updateTimer;
	_updateTimer = 0;
    }

    idleDisplay();
    updateSummary();

    emit aborted();
}


void
KDirTreeView::finalizeLocal( KDirInfo *dir )
{
    if ( dir )
    {
	KDirTreeViewItem *clone = locate( dir,
					  false,	// lazy
					  false );	// doClone
	if ( clone )
	    clone->finalizeLocal();
    }
}


void
KDirTreeView::sendProgressInfo( const QString & newCurrentDir )
{
    _currentDir = newCurrentDir;

#if VERBOSE_PROGRESS_INFO
    emit progressInfo( i18n( "Elapsed time: %1   reading directory %2" )
		       .arg( formatTime( _stopWatch.elapsed() ) )
		       .arg( _currentDir ) );
#else
    emit progressInfo( i18n( "Elapsed time: %1" )
		       .arg( formatTime( _stopWatch.elapsed() ) ) );
#endif
}


KDirTreeViewItem *
KDirTreeView::locate( KFileInfo *wanted, bool lazy, bool doClone )
{
    KDirTreeViewItem *child = firstChild();

    while ( child )
    {
	KDirTreeViewItem *wantedChild = child->locate( wanted, lazy, doClone, 0 );

	if ( wantedChild )
	    return wantedChild;
	else
	    child = child->next();
    }

    return 0;
}



int
KDirTreeView::openCount()
{
    int count = 0;
    KDirTreeViewItem *child = firstChild();

    while ( child )
    {
	count += child->openCount();
	child  = child->next();
    }

    return count;
}


void
KDirTreeView::selectItem( QListViewItem *listViewItem )
{
    _selection = dynamic_cast<KDirTreeViewItem *>( listViewItem );

    if ( _selection )
    {
	// kdDebug() << k_funcinfo << " Selecting item " << _selection << endl;
	setSelected( _selection, true );
    }
    else
    {
	// kdDebug() << k_funcinfo << " Clearing selection" << endl;
	clearSelection();
    }


    emit selectionChanged( _selection );
    emit selectionChanged( _selection ? _selection->orig() : (KFileInfo *) 0 );
}


void
KDirTreeView::selectItem( KFileInfo *newSelection )
{
    // Short-circuit for the most common case: The signal has been triggered by
    // this view, and the KDirTree has sent it right back.

    if ( _selection && _selection->orig() == newSelection )
	return;

    if ( ! newSelection )
	clearSelection();
    else
    {
	_selection = locate( newSelection,
			     false,	// lazy
			     true );	// doClone
	if ( _selection )
	{
	    closeAllExcept( _selection );
	    _selection->setOpen( false );
	    ensureItemVisible( _selection );
	    emit selectionChanged( _selection );
	    setSelected( _selection, true );
	}
	else
	    kdError() << "Couldn't clone item " << newSelection << endl;
    }
}


void
KDirTreeView::clearSelection()
{
    // kdDebug() << k_funcinfo << endl;
    _selection = 0;
    QListView::clearSelection();

    emit selectionChanged( (KDirTreeViewItem *) 0 );
    emit selectionChanged( (KFileInfo *) 0 );
}


void
KDirTreeView::closeAllExcept( KDirTreeViewItem *except )
{
    if ( ! except )
    {
	kdError() << k_funcinfo << ": NULL pointer passed" << endl;
	return;
    }

    except->closeAllExceptThis();
}


const QColor &
KDirTreeView::fillColor( int level ) const
{
    if ( level < 0 )
    {
	kdWarning() << k_funcinfo << "Invalid argument: " << level << endl;
	level = 0;
    }

    return _fillColor [ level % _usedFillColors ];
}


const QColor &
KDirTreeView::rawFillColor( int level ) const
{
    if ( level < 0 || level > KDirTreeViewMaxFillColor )
    {
	level = 0;
	kdWarning() << k_funcinfo << "Invalid argument: " << level << endl;
    }

    return _fillColor [ level % KDirTreeViewMaxFillColor ];
}


void
KDirTreeView::setFillColor( int			level,
			    const QColor &	color )
{
    if ( level >= 0 && level < KDirTreeViewMaxFillColor )
	_fillColor[ level ] = color;
}



void
KDirTreeView::setUsedFillColors( int usedFillColors )
{
    if ( usedFillColors < 1 )
    {
	kdWarning() << k_funcinfo << "Invalid argument: "<< usedFillColors << endl;
	usedFillColors = 1;
    }
    else if ( usedFillColors >= KDirTreeViewMaxFillColor )
    {
	kdWarning() << k_funcinfo << "Invalid argument: "<< usedFillColors
		    << " (max: " << KDirTreeViewMaxFillColor-1 << ")" << endl;
	usedFillColors = KDirTreeViewMaxFillColor-1;
    }

    _usedFillColors = usedFillColors;
}


void
KDirTreeView::setDefaultFillColors()
{
    int i;

    for ( i=0; i < KDirTreeViewMaxFillColor; i++ )
    {
	_fillColor[i] = blue;
    }

    i = 0;
    _usedFillColors = 4;

    setFillColor ( i++, QColor ( 0,     0, 255 ) );
    setFillColor ( i++, QColor ( 128,   0, 128 ) );
    setFillColor ( i++, QColor ( 231, 147,  43 ) );
    setFillColor ( i++, QColor (   4, 113,   0 ) );
    setFillColor ( i++, QColor ( 176,   0,   0 ) );
    setFillColor ( i++, QColor ( 204, 187,   0 ) );
    setFillColor ( i++, QColor ( 162,  98,  30 ) );
    setFillColor ( i++, QColor (   0, 148, 146 ) );
    setFillColor ( i++, QColor ( 217,  94,   0 ) );
    setFillColor ( i++, QColor (   0, 194,  65 ) );
    setFillColor ( i++, QColor ( 194, 108, 187 ) );
    setFillColor ( i++, QColor (   0, 179, 255 ) );
}


void
KDirTreeView::setTreeBackground( const QColor &color )
{
    _treeBackground = color;
    _percentageBarBackground = _treeBackground.dark( 115 );

    QPalette pal = kapp->palette();
    pal.setBrush( QColorGroup::Base, _treeBackground );
    setPalette( pal );
}


void
KDirTreeView::ensureContrast()
{
    if ( colorGroup().base() == white ||
	 colorGroup().base() == black   )
    {
	setTreeBackground( colorGroup().midlight() );
    }
    else
    {
	setTreeBackground( colorGroup().base() );
    }
}


void
KDirTreeView::paletteChanged()
{
    setTreeBackground( KGlobalSettings::baseColor() );
    ensureContrast();
}


void
KDirTreeView::popupContextMenu( QListViewItem *	listViewItem,
				const QPoint &	pos,
				int 		column )
{
    KDirTreeViewItem *item = (KDirTreeViewItem *) listViewItem;

    if ( ! item )
	return;

    KFileInfo * orig = item->orig();

    if ( ! orig )
    {
	kdError() << "NULL item->orig()" << endl;
	return;
    }

    if ( column == _nameCol 		||
	 column == _percentBarCol	||
	 column == _percentNumCol	  )
    {
	if ( orig->isExcluded() && column == _percentBarCol )
	{
	    // Show with exclude rule caused the exclusion

	    const KExcludeRule * rule = KExcludeRules::excludeRules()->matchingRule( orig->url() );

	    QString text;

	    if ( rule )
	    {
		text = i18n( "Matching exclude rule:   %1" ).arg( rule->regexp().pattern() );
	    }
	    else
	    {
		text = i18n( "<Unknown exclude rule>" );
	    }

	    popupContextInfo( pos, text );
	}
	else
	{
	    // Make the item the context menu is popping up over the current
	    // selection - all user operations refer to the current selection.
	    // Just right-clicking on an item does not make it the current
	    // item!
	    selectItem( item );

	    // Let somebody from outside pop up the context menu, if so desired.
	    emit contextMenu( item, pos );
	}
    }


    // If the column is one with a large size in kB/MB/GB, open a
    // info popup with the exact number.

    if ( column == _ownSizeCol && ! item->orig()->isDotEntry() )
    {
	if ( orig->isSparseFile() || ( orig->links() > 1 && orig->isFile() ) )
	{
	    QString text;

	    if ( orig->isSparseFile() )
	    {
		text = i18n( "Sparse file: %1 (%2 Bytes) -- allocated: %3 (%4 Bytes)" )
		    .arg( formatSize( orig->byteSize() ) )
		    .arg( formatSizeLong( orig->byteSize()  ) )
		    .arg( formatSize( orig->allocatedSize() ) )
		    .arg( formatSizeLong( orig->allocatedSize() ) );
	    }
	    else
	    {
		text = i18n( "%1 (%2 Bytes) with %3 hard links => effective size: %4 (%5 Bytes)" )
		    .arg( formatSize( orig->byteSize() ) )
		    .arg( formatSizeLong( orig->byteSize() ) )
		    .arg( orig->links() )
		    .arg( formatSize( orig->size() ) )
		    .arg( formatSizeLong( orig->size() ) );
	    }

	    popupContextInfo( pos, text );
	}
	else
	{
	    popupContextSizeInfo( pos, orig->size() );
	}
    }

    if ( column == _totalSizeCol &&
	 ( item->orig()->isDir() || item->orig()->isDotEntry() ) )
    {
	popupContextSizeInfo( pos, item->orig()->totalSize() );
    }


    // Show alternate time / date format in time / date related columns.

    if ( column == _latestMtimeCol )
    {
	popupContextInfo( pos, formatTimeDate( item->orig()->latestMtime() ) );
    }

    logActivity( 3 );
}


void
KDirTreeView::popupContextSizeInfo( const QPoint &	pos,
				    KFileSize		size )
{
    QString info;

    if ( size < 1024 )
    {
	info = formatSizeLong( size ) + " " + i18n( "Bytes" );
    }
    else
    {
	info = i18n( "%1 (%2 Bytes)" )
	    .arg( formatSize( size ) )
	    .arg( formatSizeLong( size ) );
    }

    popupContextInfo( pos, info );
}


void
KDirTreeView::popupContextInfo( const QPoint  &	pos,
				const QString & info )
{
    _contextInfo->changeItem( info, _idContextInfo );
    _contextInfo->popup( pos );
}


void
KDirTreeView::readConfig()
{
    KConfig *config = kapp->config();
    KConfigGroupSaver saver( config, "Tree Colors" );
    _usedFillColors = config->readNumEntry( "usedFillColors", -1 );

    if ( _usedFillColors < 0 )
    {
	/*
	 * No 'usedFillColors' in the config file?  Better forget that
	 * file and use default values. Otherwise, all colors would very
	 * likely become blue - the default color.
	 */
	setDefaultFillColors();
    }
    else
    {
	// Read the rest of the 'Tree Colors' section

	QColor defaultColor( blue );

	for ( int i=0; i < KDirTreeViewMaxFillColor; i++ )
	{
	    QString name;
	    name.sprintf( "fillColor_%02d", i );
	    _fillColor [i] = config->readColorEntry( name, &defaultColor );
	}
    }

    if ( isVisible() )
	triggerUpdate();
}


void
KDirTreeView::saveConfig() const
{
    KConfig *config = kapp->config();
    KConfigGroupSaver saver( config, "Tree Colors" );

    config->writeEntry( "usedFillColors", _usedFillColors );

    for ( int i=0; i < KDirTreeViewMaxFillColor; i++ )
    {
	QString name;
	name.sprintf( "fillColor_%02d", i );
	config->writeEntry ( name, _fillColor [i] );
    }
}


void
KDirTreeView::setSorting( int column, bool increasing )
{
    _sortCol = column;
    QListView::setSorting( column, increasing );
}


void
KDirTreeView::logActivity( int points )
{
    emit userActivity( points );
}


void
KDirTreeView::columnResized( int column, int oldSize, int newSize )
{
    NOT_USED( oldSize );
    NOT_USED( newSize );

    if ( column == _percentBarCol )
	triggerUpdate();
}

void
KDirTreeView::sendMailToOwner()
{
    if ( ! _selection )
    {
	kdError() << k_funcinfo << "Nothing selected!" << endl;
	return;
    }

    QString owner = KioDirReadJob::owner( fixedUrl( _selection->orig()->url() ) );
    QString subject = i18n( "Disk Usage" );
    QString body =
	i18n("Please check your disk usage and clean up if you can. Thank you." )
	+ "\n\n"
	+ _selection->asciiDump()
	+ "\n\n"
	+ i18n( "Disk usage report generated by KDirStat" )
	+ "\nhttp://kdirstat.sourceforge.net/";

    // kdDebug() << "owner: "   << owner   << endl;
    // kdDebug() << "subject: " << subject << endl;
    // kdDebug() << "body:\n"   << body    << endl;

    KURL mail;
    mail.setProtocol( "mailto" );
    mail.setPath( owner );
    mail.setQuery( "?subject="	+ KURL::encode_string( subject ) +
		   "&body="	+ KURL::encode_string( body ) );

    // TODO: Check for maximum command line length.
    //
    // The hard part with this is how to get this from all that 'autoconf'
    // stuff into 'config.h' or some other include file without hardcoding
    // anything - this is too system dependent.

    kapp->invokeMailer( mail );
    logActivity( 10 );
}






KDirTreeViewItem::KDirTreeViewItem( KDirTreeView *	view,
				    KFileInfo *		orig )
    : QListViewItem( view )
{
    init( view, 0, orig );
}


KDirTreeViewItem::KDirTreeViewItem( KDirTreeView *	view,
				    KDirTreeViewItem *	parent,
				    KFileInfo *		orig )
    : QListViewItem( parent )
{
    CHECK_PTR( parent );
    init( view, parent, orig );
}


void
KDirTreeViewItem::init( KDirTreeView *		view,
			KDirTreeViewItem *	parent,
			KFileInfo *		orig )
{
    _view 	= view;
    _parent	= parent;
    _orig	= orig;
    _percent	= 0.0;
    _pacMan	= 0;
    _openCount	= 0;

    // _view->incDebugCount(1);
    // kdDebug() << "new KDirTreeViewItem for " << orig << endl;

    if ( _orig->isDotEntry() )
    {
       setText( view->nameCol(), i18n( "<Files>" ) );
       QListViewItem::setOpen ( false );
    }
    else
    {
	setText( view->nameCol(), QString::fromLocal8Bit(_orig->name()) );

	if ( ! _orig->isDevice() )
	{
	    QString text;

	    if ( _orig->isFile() && ( _orig->links() > 1 ) ) // Regular file with multiple links
	    {
		if ( _orig->isSparseFile() )
		{
		    text = i18n( "%1 / %2 Links (allocated: %3)" )
			.arg( formatSize( _orig->byteSize() ) )
			.arg( formatSize( _orig->links() ) )
			.arg( formatSize( _orig->allocatedSize() ) );
		}
		else
		{
		    text = i18n( "%1 / %2 Links" )
			.arg( formatSize( _orig->byteSize() ) )
			.arg( _orig->links() );
		}
	    }
	    else // No multiple links or no regular file
	    {
		if ( _orig->isSparseFile() )
		{
		    text = i18n( "%1 (allocated: %2)" )
			.arg( formatSize( _orig->byteSize() ) )
			.arg( formatSize( _orig->allocatedSize() ) );
		}
		else
		{
		    text = formatSize( _orig->size() );
		}
	    }

	    setText( view->ownSizeCol(), text );

#ifdef NEVER_EXECUTED
	    // Never executed because during init() a read job for this
	    // directory will still be pending. Only the read job will figure
	    // out if an exclude rule applies.

	    if ( _orig->isExcluded() )
	    {
		setText( _view->percentBarCol(), i18n( "[excluded]" ) );
	    }
#endif
	}

	QListViewItem::setOpen ( _orig->treeLevel() < _view->openLevel() );
	/*
	 * Don't use KDirTreeViewItem::setOpen() here since this might call
	 * KDirTreeViewItem::deferredClone() which would confuse bookkeeping
	 * with addChild() signals that might arrive, too - resulting in double
	 * dot entries.
	 */
    }

    if ( _view->doLazyClone() &&
	 ( _orig->isDir() || _orig->isDotEntry() ) )
    {
	/*
	 * Determine whether or not this item can be opened.
	 *
	 * Normally, Qt handles this very well, but when lazy cloning is in
	 * effect, Qt cannot know whether or not there are children - they may
	 * only be in the original tree until the user tries to open this
	 * item. So let's assume there may be children as long as the directory
	 * is still being read.
	 */

	if ( _orig->readState() == KDirQueued  ||
	     _orig->readState() == KDirReading ||
	     _orig->readState() == KDirCached    )
	{
	    setExpandable( true );
	}
	else	// KDirFinished, KDirError, KDirAborted
	{
	    setExpandable( _orig->hasChildren() );
	}
    }

    if ( ! parent || parent->isOpen() )
    {
	setIcon();
    }

    _openCount = isOpen() ? 1 : 0;
}


KDirTreeViewItem::~KDirTreeViewItem()
{
    if ( _pacMan )
	delete _pacMan;

    if ( this == _view->selection() )
	_view->clearSelection();
}


void
KDirTreeViewItem::setIcon()
{
    QPixmap icon;

    if ( _orig->isDotEntry() )
    {
	icon = isOpen() ? _view->openDotEntryIcon() : _view->closedDotEntryIcon();
    }
    else if ( _orig->isDir() )
    {
	if  ( _orig->readState() == KDirAborted )
	{
	    icon = _view->stopIcon();
	}
	else if ( _orig->readState() == KDirError   )
	{
	    icon = _view->unreadableDirIcon();
	    setExpandable( false );
	}
	else
	{
	    if ( _orig->isMountPoint() )
	    {
		icon = _view->mountPointIcon();
	    }
	    else
	    {
		icon = isOpen() ? _view->openDirIcon() : _view->closedDirIcon();
	    }
	}
    }
    else if ( _orig->isFile() 		)	icon = _view->fileIcon();
    else if ( _orig->isSymLink() 	)	icon = _view->symLinkIcon();
    else if ( _orig->isBlockDevice() 	)	icon = _view->blockDevIcon();
    else if ( _orig->isCharDevice() 	)	icon = _view->charDevIcon();
    else if ( _orig->isSpecial() 	)	icon = _view->fifoIcon();

    setPixmap( _view->iconCol(), icon );
}


void
KDirTreeViewItem::updateSummary()
{
    // _view->incDebugCount(2);

    // Update this item

    setIcon();
    setText( _view->latestMtimeCol(),		"  " + localeTimeDate( _orig->latestMtime() ) );

    if ( _orig->isDir() || _orig->isDotEntry() )
    {
	QString prefix = " ";

	if ( _orig->readState() == KDirAborted )
	    prefix = " >";

	setText( _view->totalSizeCol(),		prefix + formatSize(  _orig->totalSize()	) );
	setText( _view->totalItemsCol(),	prefix + formatCount( _orig->totalItems()	) );
	setText( _view->totalFilesCol(),	prefix + formatCount( _orig->totalFiles()	) );

	if ( _view->readJobsCol() >= 0 )
	{
#if SEPARATE_READ_JOBS_COL
	    setText( _view->readJobsCol(),	" " + formatCount( _orig->pendingReadJobs(), true ) );
#else
	    int jobs 		= _orig->pendingReadJobs();
	    QString text	= "";

	    if ( jobs > 0 )
	    {
		text = i18n( "[%1 Read Jobs]" ).arg( formatCount( _orig->pendingReadJobs(), true ) );
	    }

	    setText( _view->readJobsCol(), text );
#endif
	}
    }

    if ( _orig->isDir() )
    {
	setText( _view->totalSubDirsCol(),	" " + formatCount( _orig->totalSubDirs() ) );

	if ( _orig->isExcluded() )
	    setText( _view->percentBarCol(),	i18n( "[excluded]" ) );
    }


    // Calculate and display percentage

    if ( _orig->parent() &&				// only if there is a parent as calculation base
	 _orig->parent()->pendingReadJobs() < 1	&&	// not before subtree is finished reading
	 _orig->parent()->totalSize() > 0 &&		// avoid division by zero
	 ! _orig->isExcluded() )			// not if this is an excluded object (dir)
    {
	 _percent = ( 100.0 * _orig->totalSize() ) / (float) _orig->parent()->totalSize();
	 setText( _view->percentNumCol(), formatPercent ( _percent ) );
    }
    else
    {
	_percent = 0.0;
	setText( _view->percentNumCol(), "" );
    }

    if ( _view->doPacManAnimation() && _orig->isBusy() )
    {
	if ( ! _pacMan )
	    _pacMan = new KPacManAnimation( _view, height()-4, true );

	repaint();
    }


    if ( ! isOpen() )	// Lazy update: Nobody can see the children
	return;		// -> don't update them.


    // Update all children

    KDirTreeViewItem *child = firstChild();

    while ( child )
    {
	child->updateSummary();
	child = child->next();
    }
}


KDirTreeViewItem *
KDirTreeViewItem::locate( KFileInfo *	wanted,
			  bool		lazy,
			  bool		doClone,
			  int 		level )
{
    if ( lazy && ! isOpen() )
    {
	/*
	 * In "lazy" mode, we don't bother searching all the children of this
	 * item if they are not visible (i.e. the branch is open) anyway. In
	 * this case, cloning that branch is deferred until the branch is
	 * actually opened - which in most cases will never happen anyway (most
	 * users don't manually open each and every subtree). If and when it
	 * happens, we'll probably be fast enough bringing the view tree in
	 * sync with the original tree since opening a branch requires manual
	 * interaction which is a whole lot slower than copying a couple of
	 * objects.
	 *
	 * Note that this mode is _independent_ of lazy cloning in general: The
	 * caller explicitly specifies if he wants to locate an item at all
	 * cost, even if that means deferred cloning children whose creation
	 * has been delayed until now.
	 */

	// kdDebug() << "Too lazy to search for " << wanted << " from " << this << endl;
	return 0;
    }

    if ( _orig == wanted )
    {
	return this;
    }

    if ( level < 0 )
	level = _orig->treeLevel();

    if ( wanted->urlPart( level ) == _orig->name() )
    {
	// Search all children

	KDirTreeViewItem *child = firstChild();

	if ( ! child && _orig->hasChildren() && doClone )
	{
	    // kdDebug() << "Deferred cloning " << this << " for children search of " << wanted << endl;
	    deferredClone();
	    child = firstChild();
	}

	while ( child )
	{
	    KDirTreeViewItem *foundChild = child->locate( wanted, lazy, doClone, level+1 );

	    if ( foundChild )
		return foundChild;
	    else
		child = child->next();
	}
    }

    return 0;
}


void
KDirTreeViewItem::deferredClone()
{
    // _view->incDebugCount(3);

    if ( ! _orig->hasChildren() )
    {
	// kdDebug() << k_funcinfo << "Oops, no children - sorry for bothering you!" << endl;
	setExpandable( false );

	return;
    }


    // Clone all normal children

    int level		 = _orig->treeLevel();
    bool startingClean	 = ! firstChild();
    KFileInfo *origChild = _orig->firstChild();

    while ( origChild )
    {
	if ( startingClean ||
	     ! locate( origChild,
		       false,		// lazy
		       true,		// doClone
		       level ) )
	{
	    // kdDebug() << "Deferred cloning " << origChild << endl;
	    new KDirTreeViewItem( _view, this, origChild );
	}

	origChild = origChild->next();
    }


    // Clone the dot entry

    if ( _orig->dotEntry() &&
	 ( startingClean ||
	   ! locate( _orig->dotEntry(),
		     false,	// lazy
		     true,	// doClone
		     level )
	   )
	 )
    {
	// kdDebug() << "Deferred cloning dot entry for " << _orig << endl;
	new KDirTreeViewItem( _view, this, _orig->dotEntry() );
    }
}


void
KDirTreeViewItem::finalizeLocal()
{
    // kdDebug() << k_funcinfo << _orig << endl;
    cleanupDotEntries();

    if ( _orig->totalItems() == 0 )
	// _orig->hasChildren() would give a wrong answer here since it counts
	// the dot entry, too - which might be removed a moment later.
    {
	setExpandable( false );
    }
}


void
KDirTreeViewItem::cleanupDotEntries()
{
    if ( ! _orig->dotEntry() )
	return;

    KDirTreeViewItem *dotEntry = findDotEntry();

    if ( ! dotEntry )
	return;


    // Reparent dot entry children if there are no subdirectories on this level

    if ( ! _orig->firstChild() )
    {
	// kdDebug() << "Removing solo dot entry clone " << _orig << endl;
	KDirTreeViewItem *child = dotEntry->firstChild();

	while ( child )
	{
	    KDirTreeViewItem *nextChild = child->next();


	    // Reparent this child

	    // kdDebug() << "Reparenting clone " << child << endl;
	    dotEntry->removeItem( child );
	    insertItem( child );

	    child = nextChild;
	}

	/*
	 * Immediately delete the (now emptied) dot entry. The algorithm for
	 * the original tree doesn't quite fit here - there, the dot entry is
	 * actually deleted in the step below. But the 'no children' check for
	 * this fails here since the original dot entry still _has_ its
	 * children - they will be deleted only after all clones have been
	 * processed.
	 *
	 * This had been the cause for a core that took me quite some time to
	 * track down.
	 */
	delete dotEntry;
	dotEntry = 0;
    }


    // Delete dot entries without any children

    if ( ! _orig->dotEntry()->firstChild() && dotEntry )
    {
	// kdDebug() << "Removing empty dot entry clone " << _orig << endl;
	delete dotEntry;
    }
}


KDirTreeViewItem *
KDirTreeViewItem::findDotEntry() const
{
    KDirTreeViewItem *child = firstChild();

    while ( child )
    {
	if ( child->orig()->isDotEntry() )
	    return child;

	child = child->next();
    }

    return 0;
}


void
KDirTreeViewItem::setOpen( bool open )
{
    if ( open && _view->doLazyClone() )
    {
	// kdDebug() << "Opening " << this << endl;
	deferredClone();
    }

    if ( isOpen() != open )
    {
	openNotify( open );
    }

    QListViewItem::setOpen( open );
    setIcon();

    if ( open )
	updateSummary();

    // kdDebug() << _openCount << " open in " << this << endl;

    // _view->logActivity( 1 );
}


void
KDirTreeViewItem::openNotify( bool open )
{
    if ( open )
	_openCount++;
    else
	_openCount--;

    if ( _parent )
	_parent->openNotify( open );
}


void
KDirTreeViewItem::openSubtree()
{
    if ( parent() )
	parent()->setOpen( true );

    setOpen( true );
}


void
KDirTreeViewItem::closeSubtree()
{
    setOpen( false );

    if ( _openCount > 0 )
    {
	KDirTreeViewItem * child = firstChild();

	while ( child )
	{
	    child->closeSubtree();
	    child = child->next();
	}
    }

    _openCount = 0;	// just to be sure
}


void
KDirTreeViewItem::closeAllExceptThis()
{
    KDirTreeViewItem *sibling = _parent ?
	_parent->firstChild() : _view->firstChild();

    while ( sibling )
    {
	if ( sibling != this )
	    sibling->closeSubtree();	// Recurse down

	sibling = sibling->next();
    }

    setOpen( true );

    if ( _parent )
	_parent->closeAllExceptThis();	// Recurse up
}


QString
KDirTreeViewItem::asciiDump()
{
    QString dump;

    dump.sprintf( "%10s  %s\n",
		  (const char *) formatSize( _orig->totalSize() ),
		  (const char *) _orig->debugUrl() );

    if ( isOpen() )
    {
	KDirTreeViewItem *child = firstChild();

	while ( child )
	{
	    dump += child->asciiDump();
	    child = child->next();
	}
    }

    return dump;
}


/**
 * Comparison function used for sorting the list.
 * Returns:
 * -1 if this <	 other
 *  0 if this == other
 * +1 if this >	 other
 **/
int
KDirTreeViewItem::compare( QListViewItem *	otherListViewItem,
			   int			column,
			   bool			ascending ) const
{
    // _view->incDebugCount(4);
    KDirTreeViewItem * other = dynamic_cast<KDirTreeViewItem *> (otherListViewItem);

    if ( other )
    {
	KFileInfo * otherOrig = other->orig();

#if ! SEPARATE_READ_JOBS_COL
	if ( column == _view->readJobsCol() )		return - compare( _orig->pendingReadJobs(), otherOrig->pendingReadJobs() );
	else
#endif
	if ( column == _view->totalSizeCol()  ||
	     column == _view->percentNumCol() ||
	     column == _view->percentBarCol()   )	return - compare( _orig->totalSize(), 	 otherOrig->totalSize()    );

        else if ( column == _view->ownSizeCol() )	return - compare( _orig->size(), 	 otherOrig->size() 	   );
	else if ( column == _view->totalItemsCol() )	return - compare( _orig->totalItems(), 	 otherOrig->totalItems()   );
	else if ( column == _view->totalFilesCol() )	return - compare( _orig->totalFiles(), 	 otherOrig->totalFiles()   );
	else if ( column == _view->totalSubDirsCol() )	return - compare( _orig->totalSubDirs(), otherOrig->totalSubDirs() );
	else if ( column == _view->latestMtimeCol() )	return - compare( _orig->latestMtime(),	 otherOrig->latestMtime()  );
	else
	{
	    if ( _orig->isDotEntry() )	// make sure dot entries are last in the list
		return 1;

	    if ( otherOrig->isDotEntry() )
		return -1;
	}
    }

    return QListViewItem::compare( otherListViewItem, column, ascending );
}


void
KDirTreeViewItem::paintCell( QPainter *			painter,
			     const QColorGroup &	colorGroup,
			     int			column,
			     int			width,
			     int			alignment )
{
    // _view->incDebugCount(5);

    if ( column == _view->percentBarCol() )
    {
	painter->setBackgroundColor( colorGroup.base() );

	if ( _percent > 0.0 )
	{
	    if ( _pacMan )
	    {
		delete _pacMan;
		_pacMan = 0;
	    }

	    int level = _orig->treeLevel();
	    paintPercentageBar ( _percent,
				 painter,
				 _view->treeStepSize() * ( level-1 ),
				 width,
				 _view->fillColor( level-1 ),
				 _view->percentageBarBackground() );
	}
	else
	{
	    if ( _pacMan && _orig->isBusy() )
	    {
		// kdDebug() << "Animating PacMan for " << _orig << endl;
		// painter->setBackgroundColor( _view->treeBackground() );
		_pacMan->animate( painter, QRect( 0, 0, width, height() ) );
	    }
	    else
	    {
		if ( ( _view->percentBarCol() == _view->readJobsCol()
		       && ! _pacMan )
		     || _orig->isExcluded() )
		{
		    QListViewItem::paintCell( painter,
					      colorGroup,
					      column,
					      width,
					      alignment );
		}
		else
		{
		    painter->eraseRect( 0, 0, width, height() );
		}
	    }
	}
    }
    else
    {
	/*
	 * Call the parent's paintCell() method.  We don't want to do
	 * all the hassle of drawing strings and pixmaps, regarding
	 * alignments etc.
	 */
	QListViewItem::paintCell( painter,
				  colorGroup,
				  column,
				  width,
				  alignment );
    }
}


void
KDirTreeViewItem::paintPercentageBar( float		percent,
				      QPainter *	painter,
				      int		indent,
				      int		width,
				      const QColor &	fillColor,
				      const QColor &	barBackground )
{
    int penWidth = 2;
    int extraMargin = 3;
    int x = _view->itemMargin();
    int y = extraMargin;
    int w = width    - 2 * _view->itemMargin();
    int h = height() - 2 * extraMargin;
    int fillWidth;

    painter->eraseRect( 0, 0, width, height() );
    w -= indent;
    x += indent;

    if ( w > 0 )
    {
	QPen pen( painter->pen() );
	pen.setWidth( 0 );
	painter->setPen( pen );
	painter->setBrush( NoBrush );
	fillWidth = (int) ( ( w - 2 * penWidth ) * percent / 100.0);


	// Fill bar background.

	painter->fillRect( x + penWidth, y + penWidth,
			   w - 2 * penWidth + 1, h - 2 * penWidth + 1,
			   barBackground );
	/*
	 * Notice: The Xlib XDrawRectangle() function always fills one
	 * pixel less than specified. Altough this is very likely just a
	 * plain old bug, it is documented that way. Obviously, Qt just
	 * maps the fillRect() call directly to XDrawRectangle() so they
	 * inherited that bug (although the Qt doc stays silent about
	 * it). So it is really necessary to compensate for that missing
	 * pixel in each dimension.
	 *
	 * If you don't believe it, see for yourself.
	 * Hint: Try the xmag program to zoom into the drawn pixels.
	 **/

	// Fill the desired percentage.

	painter->fillRect( x + penWidth, y + penWidth,
			   fillWidth+1, h - 2 * penWidth+1,
			   fillColor );


	// Draw 3D shadows.

	pen.setColor( contrastingColor ( Qt::black,
					 painter->backgroundColor() ) );
	painter->setPen( pen );
	painter->drawLine( x, y, x+w, y );
	painter->drawLine( x, y, x, y+h );

	pen.setColor( contrastingColor( barBackground.dark(),
					painter->backgroundColor() ) );
	painter->setPen( pen );
	painter->drawLine( x+1, y+1, x+w-1, y+1 );
	painter->drawLine( x+1, y+1, x+1, y+h-1 );

	pen.setColor( contrastingColor( barBackground.light(),
					painter->backgroundColor() ) );
	painter->setPen( pen );
	painter->drawLine( x+1, y+h, x+w, y+h );
	painter->drawLine( x+w, y, x+w, y+h );

	pen.setColor( contrastingColor( Qt::white,
					painter->backgroundColor() ) );
	painter->setPen( pen );
	painter->drawLine( x+2, y+h-1, x+w-1, y+h-1 );
	painter->drawLine( x+w-1, y+1, x+w-1, y+h-1 );
    }
}








QString
KDirStat::formatSizeLong( KFileSize size )
{
    QString sizeText;
    int count = 0;

    while ( size > 0 )
    {
	sizeText = ( ( size % 10 ) + '0' ) + sizeText;
	size /= 10;

	if ( ++count == 3 && size > 0 )
	{
	    sizeText = KGlobal::locale()->thousandsSeparator() + sizeText;
	    count = 0;
	}
    }

    return sizeText;
}


QString
KDirStat::hexKey( KFileSize size )
{
    /**
     * This is optimized for performance, not for aesthetics.
     * And every now and then the old C hacker breaks through in most of us...
     * ;-)
     **/

    static const char hexDigits[] = "0123456789ABCDEF";
    char key[ sizeof( KFileSize ) * 2 + 1 ];	// 2 hex digits per byte required
    char *cptr = key + sizeof( key ) - 1;	// now points to last char of key

    memset( key, '0', sizeof( key ) - 1 );	// fill with zeroes
    *cptr-- = 0;				// terminate string

    while ( size > 0 )
    {
	*cptr-- = hexDigits[ size & 0xF ];	// same as size % 16
	size >>= 4;				// same as size /= 16
    }

    return QString( key );
}


QString
KDirStat::formatTime( long millisec, bool showMilliSeconds )
{
    QString formattedTime;
    int hours;
    int min;
    int sec;

    hours	= millisec / 3600000L;	// 60*60*1000
    millisec	%= 3600000L;

    min		= millisec / 60000L;	// 60*1000
    millisec	%= 60000L;

    sec		= millisec / 1000L;
    millisec	%= 1000L;

    if ( showMilliSeconds )
    {
	formattedTime.sprintf ( "%02d:%02d:%02d.%03ld",
				hours, min, sec, millisec );
    }
    else
    {
	formattedTime.sprintf ( "%02d:%02d:%02d", hours, min, sec );
    }

    return formattedTime;
}


QString
KDirStat::formatCount( int count, bool suppressZero )
{
    if ( suppressZero && count == 0 )
	return "";

    QString countString;
    countString.setNum( count );

    return countString;
}


QString
KDirStat::formatPercent( float percent )
{
    QString percentString;

    percentString.sprintf( "%.1f%%", percent );

    return percentString;
}


QString
KDirStat::formatTimeDate( time_t rawTime )
{
    QString timeDateString;
    struct tm *t = localtime( &rawTime );

    /*
     * Format this as "yyyy-mm-dd hh:mm:ss".
     *
     * This format may not be POSIX'ly correct, but it is the ONLY of all those
     * brain-dead formats today's computer users are confronted with that makes
     * any sense to the average human.
     *
     * Agreed, it takes some getting used to, too, but once you got that far,
     * you won't want to miss it.
     *
     * Who the hell came up with those weird formats like described in the
     * ctime() man page? Don't those people ever actually use that?
     *
     * What sense makes a format like "Wed Jun 30 21:49:08 1993" ?
     * The weekday (of all things!) first, then a partial month name, then the
     * day of month, then the time and then - at the very end - the year.
     * IMHO this is maximum brain-dead. Not only can't you do any kind of
     * decent sorting or automatic processing with that disinformation
     * hodge-podge, your brain runs in circles trying to make sense of it.
     *
     * I could put up with crap like that if the Americans and Brits like it
     * that way, but unfortunately I as a German am confronted with that
     * bullshit, too, on a daily basis - either because some localization stuff
     * didn't work out right (again) or because some jerk decided to emulate
     * this stuff in the German translation, too. I am sick and tired with
     * that, and since this is MY program I am going to use a format that makes
     * sense to ME.
     *
     * No, no exceptions for Americans or Brits. I had to put up with their
     * crap long enough, now it's time for them to put up with mine.
     * Payback time - though luck, folks.
     * ;-)
     *
     * Stefan Hundhammer <sh@suse.de>	2001-05-28
     * (in quite some fit of frustration)
     */
    timeDateString.sprintf( "%4d-%02d-%02d  %02d:%02d:%02d",
			    t->tm_year + 1900,
			    t->tm_mon  + 1,	// another brain-dead common pitfall - 0..11
			    t->tm_mday,
			    t->tm_hour, t->tm_min, t->tm_sec );

    return timeDateString;
}


QString
KDirStat::localeTimeDate( time_t rawTime )
{
    QDateTime timeDate;
    timeDate.setTime_t( rawTime );
    QString timeDateString =
	KGlobal::locale()->formatDate( timeDate.date(), true ) + "  " +	// short format
	KGlobal::locale()->formatTime( timeDate.time(), true );		// include seconds

    return timeDateString;
}


QColor
KDirStat::contrastingColor( const QColor &desiredColor,
			    const QColor &contrastColor )
{
    if ( desiredColor != contrastColor )
    {
	return desiredColor;
    }

    if ( contrastColor != contrastColor.light() )
    {
	// try a little lighter
	return contrastColor.light();
    }
    else
    {
	// try a little darker
	return contrastColor.dark();
    }
}





// EOF
