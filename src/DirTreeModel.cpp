/*
 *   File name: DirTreeModel.cpp
 *   Summary:	Qt data model for directory tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DirTreeModel.h"
#include "DirTree.h"
#include "FileInfoIterator.h"
#include "DataColumns.h"
#include "SelectionModel.h"
#include "Settings.h"
#include "Logger.h"
#include "Exception.h"
#include "DebugHelpers.h"

using namespace QDirStat;


DirTreeModel::DirTreeModel( QObject * parent ):
    QAbstractItemModel( parent ),
    _tree(0),
    _selectionModel(0),
    _readJobsCol( PercentBarCol ),
    _updateTimerMillisec( 333 ),
    _slowUpdateMillisec( 3000 ),
    _slowUpdate( false ),
    _sortCol( NameCol ),
    _sortOrder( Qt::AscendingOrder ),
    _removingRows( false )
{
    createTree();
    readSettings();
    loadIcons();
    _updateTimer.setInterval( _updateTimerMillisec );

    connect( &_updateTimer, SIGNAL( timeout()		 ),
	     this,	    SLOT  ( sendPendingUpdates() ) );
}


DirTreeModel::~DirTreeModel()
{
    writeSettings();

    if ( _tree )
	delete _tree;
}


void DirTreeModel::readSettings()
{
    Settings settings;
    settings.beginGroup( "DirectoryTree" );

    _tree->setCrossFileSystems( settings.value( "CrossFileSystems", false ).toBool() );
    _treeIconDir	 = settings.value( "TreeIconDir" , ":/icons/tree-medium/" ).toString();
    _updateTimerMillisec = settings.value( "UpdateTimerMillisec", 333 ).toInt();
    _slowUpdateMillisec	 = settings.value( "SlowUpdateMillisec", 3000 ).toInt();

    settings.endGroup();
}


void DirTreeModel::writeSettings()
{
    Settings settings;
    settings.beginGroup( "DirectoryTree" );

    settings.setValue( "SlowUpdateMillisec", _slowUpdateMillisec  );

    settings.setDefaultValue( "CrossFileSystems",    _tree ? _tree->crossFileSystems() : false );
    settings.setDefaultValue( "TreeIconDir" ,	     _treeIconDir	  );
    settings.setDefaultValue( "UpdateTimerMillisec", _updateTimerMillisec );

    settings.endGroup();
}


void DirTreeModel::setSlowUpdate( bool slow )
{
    _slowUpdate = slow;
    _updateTimer.setInterval( _slowUpdate ? _slowUpdateMillisec : _updateTimerMillisec );

    if ( slow )
	logInfo() << "Display update every " << _updateTimer.interval() << " millisec" << endl;
}


void DirTreeModel::createTree()
{
    _tree = new DirTree();
    CHECK_NEW( _tree );

    connect( _tree, SIGNAL( startingReading() ),
	     this,  SLOT  ( busyDisplay() ) );

    connect( _tree, SIGNAL( finished()	      ),
	     this,  SLOT  ( readingFinished() ) );

    connect( _tree, SIGNAL( aborted()	      ),
	     this,  SLOT  ( readingFinished() ) );

    connect( _tree, SIGNAL( readJobFinished( DirInfo * ) ),
	     this,  SLOT  ( readJobFinished( DirInfo * ) ) );

    connect( _tree, SIGNAL( deletingChild( FileInfo * ) ),
	     this,  SLOT  ( deletingChild( FileInfo * ) ) );

    connect( _tree, SIGNAL( clearingSubtree( DirInfo * ) ),
	     this,  SLOT  ( clearingSubtree( DirInfo * ) ) );

    connect( _tree, SIGNAL( subtreeCleared( DirInfo * ) ),
	     this,  SLOT  ( subtreeCleared( DirInfo * ) ) );

    connect( _tree, SIGNAL( childDeleted() ),
	     this,  SLOT  ( childDeleted() ) );
}


void DirTreeModel::clear()
{
    if ( _tree )
    {
	beginResetModel();

	// logDebug() << "After beginResetModel()" << endl;
	// dumpPersistentIndexList();

	_tree->clear();
	endResetModel();

	// logDebug() << "After endResetModel()" << endl;
	// dumpPersistentIndexList();
    }
}


void DirTreeModel::openUrl( const QString & url )
{
    CHECK_PTR( _tree );

    if ( _tree->root() &&  _tree->root()->hasChildren() )
	clear();

    _updateTimer.start();
    _tree->startReading( url );
}


void DirTreeModel::readPkg( const PkgFilter & pkgFilter )
{
    // logDebug() << "Reading " << pkgFilter << endl;
    CHECK_PTR( _tree );

    if ( _tree->root() &&  _tree->root()->hasChildren() )
	clear();

    _updateTimer.start();
    _tree->readPkg( pkgFilter );
}


void DirTreeModel::loadIcons()
{
    if ( _treeIconDir.isEmpty() )
    {
	logWarning() << "No tree icons" << endl;
	return;
    }

    if ( ! _treeIconDir.endsWith( "/" ) )
	_treeIconDir += "/";

    _dirIcon	       = QPixmap( _treeIconDir + "dir.png"	      );
    _dotEntryIcon      = QPixmap( _treeIconDir + "dot-entry.png"      );
    _fileIcon	       = QPixmap( _treeIconDir + "file.png"	      );
    _symlinkIcon       = QPixmap( _treeIconDir + "symlink.png"	      );
    _unreadableDirIcon = QPixmap( _treeIconDir + "unreadable-dir.png" );
    _mountPointIcon    = QPixmap( _treeIconDir + "mount-point.png"    );
    _stopIcon	       = QPixmap( _treeIconDir + "stop.png"	      );
    _excludedIcon      = QPixmap( _treeIconDir + "excluded.png"	      );
    _blockDeviceIcon   = QPixmap( _treeIconDir + "block-device.png"   );
    _charDeviceIcon    = QPixmap( _treeIconDir + "char-device.png"    );
    _specialIcon       = QPixmap( _treeIconDir + "special.png"	      );
    _pkgIcon	       = QPixmap( _treeIconDir + "folder-pkg.png"     );
#if 0
    _pkgIcon	       = QPixmap( _treeIconDir + "package.png"	      );
#endif
}


void DirTreeModel::setColumns( const DataColumnList & columns )
{
    beginResetModel();
    DataColumns::instance()->setColumns( columns );
    endResetModel();
}


FileInfo * DirTreeModel::findChild( DirInfo * parent, int childNo ) const
{
    CHECK_PTR( parent );

    const FileInfoList & childrenList =
	parent->sortedChildren( _sortCol, _sortOrder );

    if ( childNo < 0 || childNo >= childrenList.size() )
    {
	logError() << "Child #" << childNo << " is out of range: 0.."
		   << childrenList.size()-1 << " children for "
		   << parent << endl;
	Debug::dumpChildrenList( parent, childrenList );

	return 0;
    }

    // Debug::dumpChildrenList( parent, childrenList );

    return childrenList.at( childNo );
}


int DirTreeModel::rowNumber( FileInfo * child ) const
{
    if ( ! child->parent() )
	return 0;

    const FileInfoList & childrenList =
	child->parent()->sortedChildren( _sortCol, _sortOrder );

    int row = childrenList.indexOf( child );

    if ( row < 0 )
    {
	// Not found
	logError() << "Child " << child
		   << " (" << (void *) child << ")"
		   << " not found in \""
		   << child->parent() << "\"" << endl;

	Debug::dumpDirectChildren( child->parent() );
    }

    return row;
}



//
// Reimplented from QAbstractItemModel
//

int DirTreeModel::rowCount( const QModelIndex & parentIndex ) const
{
    if ( ! _tree )
	return 0;

    int count = 0;
    FileInfo * item = 0;

    if ( parentIndex.isValid() )
    {
	item = static_cast<FileInfo *>( parentIndex.internalPointer() );
	CHECK_MAGIC( item );
    }
    else
	item = _tree->root();

    if ( ! item->isDirInfo() )
	return 0;

    if ( item->toDirInfo()->isLocked() )
    {
	// logDebug() << item << " is locked - returning 0" << endl;
	return 0;
    }

    switch ( item->readState() )
    {
	case DirQueued:
	    count = 0;	// Nothing yet
	    break;

	case DirReading:

	    // Don't mess with directories that are currently being read: If we
	    // tell our view about them, the view might begin fetching model
	    // indexes for them, and when the tree later sends the
	    // readJobFinished() signal, the beginInsertRows() call in our
	    // readJobFinished() slot will confuse the view; it would assume
	    // that the number of children reported in that beginInsertRows()
	    // call needs to be added to the number reported here. We'd have to
	    // keep track how many children we already reported, and how many
	    // new ones to report later.
	    //
	    // Better keep it simple: Don't report any children until they
	    // are complete.

	    count = 0;
	    break;

	case DirError:

	    // This is a hybrid case: Depending on the dir reader, the dir may
	    // or may not be finished at this time. For a local dir, it most
	    // likely is; for a cache reader, there might be more to come.

	    if ( _tree->isBusy() )
		count = 0;
	    else
		count = item->directChildrenCount();
	    break;

	case DirFinished:
	case DirOnRequestOnly:
	case DirCached:
	case DirAborted:
	    count = item->directChildrenCount();
	    break;

	// intentionally omitting 'default' case so the compiler can report
	// missing enum values
    }

    // logDebug() << dirName << ": " << count << endl;
    return count;
}


int DirTreeModel::columnCount( const QModelIndex & parent ) const
{
    Q_UNUSED( parent );

    return DataColumns::instance()->colCount();
}


QVariant DirTreeModel::data( const QModelIndex & index, int role ) const
{
    if ( ! index.isValid() )
	return QVariant();

    DataColumn col = DataColumns::fromViewCol( index.column() );

    switch ( role )
    {
	case Qt::DisplayRole:
	    {
		FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
		CHECK_PTR( item );
		CHECK_MAGIC( item );

		QVariant result = columnText( item, col );

		if ( item && item->isDirInfo() )
		{
		    // logDebug() << "Touching " << col << "\tof " << item << endl;
		    item->toDirInfo()->touch();
		}

		return result;
	    }

	case Qt::DecorationRole:
	    {
		FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
		CHECK_MAGIC( item );

		QVariant result = columnIcon( item, col );
		return result;
	    }

	case Qt::TextAlignmentRole:
	    {
		int alignment = Qt::AlignVCenter;

		switch ( col )
		{
		    case PercentBarCol:
		    case PercentNumCol:
		    case SizeCol:
		    case TotalItemsCol:
		    case TotalFilesCol:
		    case TotalSubDirsCol:
		    case OctalPermissionsCol:
			alignment |= Qt::AlignRight;
			break;

		    case NameCol:
		    case LatestMTimeCol:
		    case UserCol:
		    case GroupCol:
		    default:
			alignment |= Qt::AlignLeft;
			break;

		    case PermissionsCol:
			alignment |= Qt::AlignHCenter;
			break;
		}

		return alignment;
	    }

	case RawDataRole: // Send raw data to our item delegate (the PercentBarDelegate)
	    {
		FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
		CHECK_PTR( item );
		CHECK_MAGIC( item );

		switch ( col )
		{
		    case NameCol:	      return item->name();
		    case PercentBarCol:
			{
			    if ( ( item->parent() && item->parent()->isBusy() ) ||
				 item == _tree->firstToplevel() )
			    {
				return -1.0;
			    }
			    else
			    {
				return item->subtreePercent();
			    }
			}
		    case PercentNumCol:	      return item->subtreePercent();
		    case SizeCol:	      return item->totalSize();
		    case TotalItemsCol:	      return item->totalItems();
		    case TotalFilesCol:	      return item->totalFiles();
		    case TotalSubDirsCol:     return item->totalSubDirs();
		    case LatestMTimeCol:      return (qulonglong) item->latestMtime();
		    case UserCol:	      return item->uid();
		    case GroupCol:	      return item->gid();
		    case PermissionsCol:      return item->mode();
		    case OctalPermissionsCol: return item->mode();
		    default:		      return QVariant();
		}
	    }

	default:
	    return QVariant();
    }

    /*NOTREACHED*/
    return QVariant();
}


QVariant DirTreeModel::headerData( int		   section,
				   Qt::Orientation orientation,
				   int		   role ) const
{
    if ( orientation != Qt::Horizontal )
	return QVariant();

    switch ( role )
    {
	case Qt::DisplayRole:
	    switch ( DataColumns::fromViewCol( section ) )
	    {
		case NameCol:		  return tr( "Name"		  );
		case PercentBarCol:	  return tr( "Subtree Percentage" );
		case PercentNumCol:	  return tr( "%"		  );
		case SizeCol:		  return tr( "Size"		  );
		case TotalItemsCol:	  return tr( "Items"		  );
		case TotalFilesCol:	  return tr( "Files"		  );
		case TotalSubDirsCol:	  return tr( "Subdirs"		  );
		case LatestMTimeCol:	  return tr( "Last Modified"	  );
		case UserCol:		  return tr( "User"		  );
		case GroupCol:		  return tr( "Group"		  );
		case PermissionsCol:	  return tr( "Permissions"	  );
		case OctalPermissionsCol: return tr( "Perm."	    );
		default:		  return QVariant();
	    }

	case Qt::TextAlignmentRole:
	    switch ( DataColumns::fromViewCol( section ) )
	    {
		case PercentBarCol:
		case PercentNumCol:
		case SizeCol:
		case TotalItemsCol:
		case TotalFilesCol:
		case TotalSubDirsCol:
		case LatestMTimeCol:
		case PermissionsCol:
		case OctalPermissionsCol: return Qt::AlignHCenter;
		default:		  return Qt::AlignLeft;
	    }

	default:
	    return QVariant();
    }
}


Qt::ItemFlags DirTreeModel::flags( const QModelIndex & index ) const
{
    if ( ! index.isValid() )
	return Qt::NoItemFlags;


    FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
    CHECK_PTR( item );
    CHECK_MAGIC( item );

    Qt::ItemFlags baseFlags = Qt::ItemIsEnabled;

#if (QT_VERSION >= QT_VERSION_CHECK( 5, 1, 0 ))
    if ( ! item->isDirInfo() )
	baseFlags |= Qt::ItemNeverHasChildren;
#endif

    // logDebug() << "Flags for " << index << endl;
    DataColumn col = DataColumns::fromViewCol( index.column() );

    switch ( col )
    {
	case PercentBarCol:
	    return baseFlags;

	default:
	    return baseFlags | Qt::ItemIsSelectable;
    }
}


QModelIndex DirTreeModel::index( int row, int column, const QModelIndex & parentIndex ) const
{
    if ( ! _tree  || ! _tree->root() || ! hasIndex( row, column, parentIndex ) )
	return QModelIndex();

    FileInfo *parent;

    if ( parentIndex.isValid() )
    {
	parent = static_cast<FileInfo *>( parentIndex.internalPointer() );
	CHECK_MAGIC( parent );
    }
    else
	parent = _tree->root();

    if ( parent->isDirInfo() )
    {
	FileInfo * child = findChild( parent->toDirInfo(), row );
	CHECK_PTR( child );

	if ( child )
	    return createIndex( row, column, child );
    }

    return QModelIndex();
}


QModelIndex DirTreeModel::parent( const QModelIndex & index ) const
{
    if ( ! index.isValid() )
	return QModelIndex();

    FileInfo * child = static_cast<FileInfo*>( index.internalPointer() );
    CHECK_PTR( child );
    CHECK_MAGIC( child );

    FileInfo * parent = child->parent();
    CHECK_PTR( parent );

    if ( ! parent || parent == _tree->root() )
	return QModelIndex();

    int row = rowNumber( parent );
    // logDebug() << "Parent of " << child << " is " << parent << " #" << row << endl;

    return createIndex( row, 0, parent );
}


void DirTreeModel::sort( int column, Qt::SortOrder order )
{
    logDebug() << "Sorting by " << static_cast<DataColumn>( column )
	       << ( order == Qt::AscendingOrder ? " ascending" : " descending" )
	       << endl;

    // logDebug() << "Before layoutAboutToBeChanged()" << endl;
    // dumpPersistentIndexList();

    emit layoutAboutToBeChanged();

    _sortCol = DataColumns::fromViewCol( column );
    _sortOrder = order;

    updatePersistentIndexes();
    emit layoutChanged();

    // logDebug() << "After layoutChanged()" << endl;
    // dumpPersistentIndexList();
}


//---------------------------------------------------------------------------


void DirTreeModel::busyDisplay()
{
    emit layoutAboutToBeChanged();

    _sortCol = NameCol;
    // logDebug() << "Sorting by " << _sortCol << " during reading" << endl;

    updatePersistentIndexes();
    emit layoutChanged();
}


void DirTreeModel::idleDisplay()
{
    emit layoutAboutToBeChanged();

    _sortCol = PercentNumCol;
    // logDebug() << "Sorting by " << _sortCol << " after reading is finished" << endl;

    updatePersistentIndexes();
    emit layoutChanged();
}


QModelIndex DirTreeModel::modelIndex( FileInfo * item, int column ) const
{
    CHECK_PTR( _tree );
    CHECK_PTR( _tree->root() );

    if (  ! item || ! item->checkMagicNumber() || item == _tree->root() )
	return QModelIndex();
    else
    {
	int row = rowNumber( item );
	// logDebug() << item << " is row #" << row << " of " << item->parent() << endl;
	return row < 0 ? QModelIndex() : createIndex( row, column, item );
    }
}



QVariant DirTreeModel::columnText( FileInfo * item, int col ) const
{
    CHECK_PTR( item );

    if ( col == _readJobsCol && item->isBusy() )
	return tr( "[%1 Read Jobs]" ).arg( item->pendingReadJobs() );

    bool limitedInfo = item->isDotEntry() || item->readState() == DirCached || item->isPkgInfo();

    switch ( col )
    {
	case NameCol:		  return item->name();
	case PercentBarCol:	  return item->isExcluded() ? tr( "[Excluded]" ) : QVariant();
	case PercentNumCol:	  return item == _tree->firstToplevel() ? QVariant() : formatPercent( item->subtreePercent() );
	case SizeCol:		  return sizeColText( item );
	case LatestMTimeCol:	  return QString( "  " ) + formatTime( item->latestMtime() );
	case UserCol:		  return limitedInfo ? QVariant() : item->userName();
	case GroupCol:		  return limitedInfo ? QVariant() : item->groupName();
	case PermissionsCol:	  return limitedInfo ? QVariant() : item->symbolicPermissions();
	case OctalPermissionsCol: return limitedInfo ? QVariant() : item->octalPermissions();
    }

    if ( item->isDirInfo() )
    {
	QString prefix = item->readState() == DirAborted ? ">" : "";

	switch ( col )
	{
	    case TotalItemsCol:	  return prefix + QString( "%1" ).arg( item->totalItems() );
	    case TotalFilesCol:	  return prefix + QString( "%1" ).arg( item->totalFiles() );
	    case TotalSubDirsCol:
		if ( item->isDotEntry() )
		    return QVariant();
		else
		    return prefix + QString( "%1" ).arg( item->totalSubDirs() );
	}
    }

    return QVariant();
}


QVariant DirTreeModel::sizeColText( FileInfo * item ) const
{
    if ( item->isDevice() )
	return QVariant();

    if ( item->isDirInfo() )
    {
	QString prefix = item->readState() == DirAborted ? ">" : "";
	return prefix + formatSize( item->totalSize() );
    }

    QString text;

    if ( item->isFile() && ( item->links() > 1 ) ) // Regular file with multiple links
    {
	if ( item->isSparseFile() )
	{
	    text = tr( "%1 / %2 Links (allocated: %3)" )
		.arg( formatSize( item->byteSize() ) )
		.arg( formatSize( item->links() ) )
		.arg( formatSize( item->allocatedSize() ) );
	}
	else
	{
	    text = tr( "%1 / %2 Links" )
		.arg( formatSize( item->byteSize() ) )
		.arg( item->links() );
	}
    }
    else // No multiple links or no regular file
    {
	if ( item->isSparseFile() )
	{
	    text = tr( "%1 (allocated: %2)" )
		.arg( formatSize( item->byteSize() ) )
		.arg( formatSize( item->allocatedSize() ) );
	}
	else
	{
	    text = formatSize( item->size() );
	}
    }

    return text;
}


QVariant DirTreeModel::columnIcon( FileInfo * item, int col ) const
{
    if ( col != NameCol )
	return QVariant();

    QPixmap icon;

    if	    ( item->isDotEntry() )  icon = _dotEntryIcon;
    else if ( item->isPkgInfo()	 )  icon = _pkgIcon;
    else if ( item->isExcluded() )  icon = _excludedIcon;
    else if ( item->isDir()	 )
    {
	if	( item->readState() == DirAborted )   icon = _stopIcon;
	else if ( item->readState() == DirError	  )   icon = _unreadableDirIcon;
	else if ( item->isMountPoint()		  )   icon = _mountPointIcon;
	else					      icon = _dirIcon;
    }
    else // ! item->isDir()
    {
	if	( item->readState() == DirError	  )   icon = _unreadableDirIcon;
	else if ( item->isFile()		  )   icon = _fileIcon;
	else if ( item->isSymLink()		  )   icon = _symlinkIcon;
	else if ( item->isBlockDevice()		  )   icon = _blockDeviceIcon;
	else if ( item->isCharDevice()		  )   icon = _charDeviceIcon;
	else if ( item->isSpecial()		  )   icon = _specialIcon;
    }

    return icon.isNull() ? QVariant() : icon;
}


void DirTreeModel::readJobFinished( DirInfo * dir )
{
    // logDebug() << dir << endl;
    delayedUpdate( dir );

    if ( anyAncestorBusy( dir ) )
    {
	if  ( dir && ! dir->isMountPoint() )
	    logDebug() << "Ancestor busy - ignoring readJobFinished for " << dir << endl;
    }
    else
    {
	newChildrenNotify( dir );
    }
}


bool DirTreeModel::anyAncestorBusy( FileInfo * item ) const
{
    while ( item )
    {
	if ( item->readState() == DirQueued ||
	     item->readState() == DirReading )
	{
	    return true;
	}

	item = item->parent();
    }

    return false;
}


void DirTreeModel::newChildrenNotify( DirInfo * dir )
{
    // logDebug() << dir << endl;

    if ( ! dir )
    {
	logError() << "NULL DirInfo *" << endl;
	return;
    }

    if ( ! dir->isTouched() && dir != _tree->root() && dir != _tree->firstToplevel() )
    {
	// logDebug() << "Remaining silent about untouched dir " << dir << endl;
	return;
    }

    QModelIndex index = modelIndex( dir );
    int count = dir->directChildrenCount();
    // Debug::dumpDirectChildren( dir );

    if ( count > 0 )
    {
	// logDebug() << "Notifying view about " << count << " new children of " << dir << endl;

	dir->lock();
	beginInsertRows( index, 0, count - 1 );
	dir->unlock();
	endInsertRows();
    }

    // If any readJobFinished signals were ignored because a parent was not
    // finished yet, now is the time to notify the view about those children,
    // too.
    FileInfoIterator it( dir );

    while ( *it )
    {
	if ( (*it)->isDirInfo() &&
	     (*it)->readState() != DirReading &&
	     (*it)->readState() != DirQueued	)
	{
	    newChildrenNotify( (*it)->toDirInfo() );
	}
	++it;
    }
}


void DirTreeModel::delayedUpdate( DirInfo * dir )
{
    while ( dir && dir != _tree->root() )
    {
	if ( dir->isTouched() )
	    _pendingUpdates.insert( dir );

	dir = dir->parent();
    }
}


void DirTreeModel::sendPendingUpdates()
{
    // logDebug() << "Sending " << _pendingUpdates.size() << " updates" << endl;

    foreach ( DirInfo * dir, _pendingUpdates )
    {
	dataChangedNotify( dir );
    }

    _pendingUpdates.clear();
}


void DirTreeModel::dataChangedNotify( DirInfo * dir )
{
    if ( ! dir || dir == _tree->root() )
	return;

    if ( dir->isTouched() ) // only if the view ever requested data about this dir
    {
	QModelIndex topLeft	= modelIndex( dir, 0 );
	QModelIndex bottomRight = createIndex( topLeft.row(), DataColumns::instance()->colCount() - 1, dir );

#if (QT_VERSION < QT_VERSION_CHECK( 5, 1, 0))
	emit dataChanged( topLeft, bottomRight );
#else
	QVector<int> roles;
	roles << Qt::DisplayRole;

	emit dataChanged( topLeft, bottomRight, roles );
#endif
	// logDebug() << "Data changed for " << dir << endl;

	// If the view is still interested in this dir, it will fetch data, and
	// then the dir will be touched again. For all we know now, this dir
	// might easily be out of scope for the view, so let's not bother the
	// view again about this dir until it's clear that the view still wants
	// updates about it.

	dir->clearTouched();
    }
}


void DirTreeModel::readingFinished()
{
    _updateTimer.stop();
    idleDisplay();
    sendPendingUpdates();

    // dumpPersistentIndexList();
    // Debug::dumpDirectChildren( _tree->root(), "root" );
}


void DirTreeModel::dumpPersistentIndexList() const
{
    QModelIndexList persistentList = persistentIndexList();

    logDebug() << persistentList.size() << " persistent indexes" << endl;

    for ( int i=0; i < persistentList.size(); ++i )
    {
	QModelIndex index = persistentList.at(i);

	FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
	CHECK_MAGIC( item );

	logDebug() << "#" << i
		   << " Persistent index "
		   << index
		   << endl;
    }
}


void DirTreeModel::updatePersistentIndexes()
{
    QModelIndexList persistentList = persistentIndexList();

    for ( int i=0; i < persistentList.size(); ++i )
    {
	QModelIndex oldIndex = persistentList.at(i);

	if ( oldIndex.isValid() )
	{
	    FileInfo * item = static_cast<FileInfo *>( oldIndex.internalPointer() );
	    QModelIndex newIndex = modelIndex( item, oldIndex.column() );
#if 0
	    logDebug() << "Updating #" << i
		       << " " << item
		       << " col " << oldIndex.column()
		       << " row " << oldIndex.row()
		       << " --> " << newIndex.row()
		       << endl;
#endif
	    changePersistentIndex( oldIndex, newIndex );
	}
    }
}


void DirTreeModel::beginRemoveRows( const QModelIndex & parent, int first, int last )
{
    if ( _removingRows )
    {
	logError() << "Removing rows already in progress" << endl;
	return;
    }


    if ( ! parent.isValid() )
    {
	logError() << "Invalid QModelIndex" << endl;
	return;
    }

    _removingRows = true;
    QAbstractItemModel::beginRemoveRows( parent, first, last );
}


void DirTreeModel::endRemoveRows()
{
    if ( _removingRows )
    {
	QAbstractItemModel::endRemoveRows();
	_removingRows = false;
    }
}


void DirTreeModel::deletingChild( FileInfo * child )
{
    logDebug() << "Deleting child " << child << endl;

    if ( child->parent() &&
	 ( child->parent() == _tree->root() ||
	   child->parent()->isTouched()	 ) )
    {
	QModelIndex parentIndex = modelIndex( child->parent(), 0 );
	int row = rowNumber( child );
	logDebug() << "beginRemoveRows for " << child << " row " << row << endl;
	beginRemoveRows( parentIndex, row, row );
    }

    invalidatePersistent( child, true );
}


void DirTreeModel::childDeleted()
{
    endRemoveRows();
}


void DirTreeModel::clearingSubtree( DirInfo * subtree )
{
    logDebug() << "Deleting all children of " << subtree << endl;

    if ( subtree == _tree->root() || subtree->isTouched() )
    {
	QModelIndex subtreeIndex = modelIndex( subtree, 0 );
	int count = subtree->directChildrenCount();

	if ( count > 0 )
	{
	    logDebug() << "beginRemoveRows for " << subtree << " row 0 to " << count - 1 << endl;
	    beginRemoveRows( subtreeIndex, 0, count - 1 );
	}
    }

    invalidatePersistent( subtree, false );
}


void DirTreeModel::subtreeCleared( DirInfo * subtree )
{
    Q_UNUSED( subtree );

    endRemoveRows();
}


void DirTreeModel::invalidatePersistent( FileInfo * subtree,
					 bool	    includeParent )
{
    foreach ( const QModelIndex & index, persistentIndexList() )
    {
	FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
	CHECK_PTR( item );

	if ( ! item->checkMagicNumber() ||
	     item->isInSubtree( subtree ) )
	{
	    if ( item != subtree || includeParent )
	    {
#if 1
		logDebug() << "Invalidating " << index << endl;
#endif
		changePersistentIndex( index, QModelIndex() );
	    }
	}
    }

}


QVariant DirTreeModel::formatPercent( float percent ) const
{
    if ( percent < 0.0 )	// Invalid percentage?
	return QVariant();

    QString text;
    text.sprintf( "%.1f%%", percent );

    return text;
}


void DirTreeModel::refreshSelected()
{
    CHECK_PTR( _selectionModel );
    FileInfo * sel = _selectionModel->selectedItems().first();

    if ( sel && sel->isDirInfo() )
    {
	logDebug() << "Refreshing " << sel << endl;
	busyDisplay();
	_tree->refresh( sel->toDirInfo() );
    }
    else
    {
	logWarning() << "NOT refreshing " << sel << endl;
    }
}

