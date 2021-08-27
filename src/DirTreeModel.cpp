/*
 *   File name: DirTreeModel.cpp
 *   Summary:	Qt data model for directory tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QPalette>

#include "Qt4Compat.h"

#include "DirTreeModel.h"
#include "DirTree.h"
#include "DirInfo.h"
#include "FileInfoIterator.h"
#include "DataColumns.h"
#include "SelectionModel.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "FormatUtil.h"
#include "Exception.h"
#include "DebugHelpers.h"

// Number of clusters up to which a file will be considered small and will also
// display the allocated size like (4k).
#define SMALL_FILE_CLUSTERS     2

// Used used percent below which a small file will also display the allocated size
// like (4k)
#define SMALL_FILE_SHOW_ALLOC_THRESHOLD         75

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

    _tree->setCrossFilesystems	( settings.value( "CrossFilesystems", false ).toBool() );
    FileInfo::setIgnoreHardLinks( settings.value( "IgnoreHardLinks",  false ).toBool() );
    _treeIconDir	 = settings.value( "TreeIconDir" , ":/icons/tree-medium/" ).toString();
    _updateTimerMillisec = settings.value( "UpdateTimerMillisec", 333 ).toInt();
    _slowUpdateMillisec	 = settings.value( "SlowUpdateMillisec", 3000 ).toInt();

    settings.endGroup();

    if ( usingLightTheme() )
    {
	settings.beginGroup( "TreeTheme-light" );

	_dirReadErrColor     = readColorEntry( settings, "DirReadErrColor",	QColor( Qt::red		 ) );
	_subtreeReadErrColor = readColorEntry( settings, "SubtreeReadErrColor", QColor( 0xa0, 0x00, 0x00 ) );

	settings.endGroup();
    }
    else // dark theme
    {
	settings.beginGroup( "TreeTheme-dark" );

	_dirReadErrColor     = readColorEntry( settings, "DirReadErrColor",	QColor( Qt::red		 ) );
	_subtreeReadErrColor = readColorEntry( settings, "SubtreeReadErrColor", QColor( Qt::yellow	 ) );

	settings.endGroup();
    }
}


void DirTreeModel::writeSettings()
{
    Settings settings;
    settings.beginGroup( "DirectoryTree" );

    settings.setValue( "SlowUpdateMillisec", _slowUpdateMillisec  );

    settings.setDefaultValue( "CrossFilesystems",    _tree ? _tree->crossFilesystems() : false );
    settings.setDefaultValue( "IgnoreHardLinks",     FileInfo::ignoreHardLinks() );
    settings.setDefaultValue( "TreeIconDir",	     _treeIconDir		 );
    settings.setDefaultValue( "UpdateTimerMillisec", _updateTimerMillisec	 );

    settings.endGroup();


    settings.beginGroup( usingLightTheme() ? "TreeTheme-light" : "TreeTheme-dark" );

    writeColorEntry( settings, "DirReadErrColor",     _dirReadErrColor	   );
    writeColorEntry( settings, "SubtreeReadErrColor", _subtreeReadErrColor );

    settings.endGroup();
}


bool DirTreeModel::usingDarkTheme()
{
    QColor background = qAppPalette().color( QPalette::Active, QPalette::Base );

    return background.lightness() < 128; // 0 (black) .. 255 (white)
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

    _dirIcon	       = QIcon( _treeIconDir + "dir.png"	    );
    _dotEntryIcon      = QIcon( _treeIconDir + "dot-entry.png"      );
    _fileIcon	       = QIcon( _treeIconDir + "file.png"	    );
    _symlinkIcon       = QIcon( _treeIconDir + "symlink.png"	    );
    _unreadableDirIcon = QIcon( _treeIconDir + "unreadable-dir.png" );
    _mountPointIcon    = QIcon( _treeIconDir + "mount-point.png"    );
    _stopIcon	       = QIcon( _treeIconDir + "stop.png"	    );
    _excludedIcon      = QIcon( _treeIconDir + "excluded.png"	    );
    _blockDeviceIcon   = QIcon( _treeIconDir + "block-device.png"   );
    _charDeviceIcon    = QIcon( _treeIconDir + "char-device.png"    );
    _specialIcon       = QIcon( _treeIconDir + "special.png"	    );
    _pkgIcon	       = QIcon( _treeIconDir + "folder-pkg.png"     );
    _atticIcon         = _dirIcon;
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
	parent->sortedChildren( _sortCol, _sortOrder,
				true );	    // includeAttic

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
	child->parent()->sortedChildren( _sortCol, _sortOrder,
					 true ); // includeAttic

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


FileInfo * DirTreeModel::itemFromIndex( const QModelIndex & index )
{
    FileInfo * item = 0;

    if ( index.isValid() )
    {
        item = static_cast<FileInfo *>( index.internalPointer() );
        CHECK_MAGIC( item );
    }

    return item;
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
	case DirPermissionDenied:

	    // This is a hybrid case: Depending on the dir reader, the dir may
	    // or may not be finished at this time. For a local dir, it most
	    // likely is; for a cache reader, there might be more to come.

	    if ( _tree->isBusy() )
		count = 0;
	    else
		count = directChildrenCount( item );
	    break;

	case DirFinished:
	case DirOnRequestOnly:
	case DirCached:
	case DirAborted:
	    count = directChildrenCount( item );
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

    DataColumn col  = DataColumns::fromViewCol( index.column() );
    FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
    CHECK_MAGIC( item );

    switch ( role )
    {
	case Qt::DisplayRole:
	    {
		QVariant result = columnText( item, col );

		if ( item && item->isDirInfo() )
		{
		    // logDebug() << "Touching " << col << "\tof " << item << endl;
		    item->toDirInfo()->touch();
		}

		return result;
	    }

	case Qt::ForegroundRole:
	    {
		if ( item->isIgnored() || item->isAttic() )
		    return qAppPalette().brush( QPalette::Disabled, QPalette::Foreground );

		if ( item->isDir() )
		{
		    if ( item->readError() )
			return _dirReadErrColor;

		    if ( item->errSubDirCount() > 0 )
			return _subtreeReadErrColor;
		}

		return QVariant();
	    }

	case Qt::DecorationRole:
	    {
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
		    case OldestFileMTimeCol:
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
		switch ( col )
		{
		    case NameCol:	      return item->name();
		    case PercentBarCol:
			{
			    if ( ( item->parent() && item->parent()->isBusy() ) ||
				 item == _tree->firstToplevel() ||
				 item->isAttic() )
			    {
				return -1.0;
			    }
			    else
			    {
				return item->subtreeAllocatedPercent();
			    }
			}
		    case PercentNumCol:	      return item->subtreeAllocatedPercent();
		    case SizeCol:	      return item->totalSize();
		    case TotalItemsCol:	      return item->totalItems();
		    case TotalFilesCol:	      return item->totalFiles();
		    case TotalSubDirsCol:     return item->totalSubDirs();
		    case LatestMTimeCol:      return (qulonglong) item->latestMtime();
		    case OldestFileMTimeCol:  return (qulonglong) item->oldestFileMtime();
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
		case OldestFileMTimeCol:  return tr( "Oldest File"	  );
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
		case OldestFileMTimeCol:
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

    bool limitedInfo = item->isPseudoDir() || item->readState() == DirCached || item->isPkgInfo();

    if ( item->isAttic() && col == PercentNumCol )
	return QVariant();

    if ( item->isPkgInfo() &&
	 item->readState() == DirAborted &&
	 ! item->firstChild() &&
	 col != NameCol )
    {
	return "?";
    }

    switch ( col )
    {
	case NameCol:		  return item->name();
	case PercentBarCol:	  return item->isExcluded() ? tr( "[Excluded]" ) : QVariant();
	case PercentNumCol:	  return item == _tree->firstToplevel() ? QVariant() : formatPercent( item->subtreeAllocatedPercent() );
	case SizeCol:		  return sizeColText( item );
	case LatestMTimeCol:	  return QString( "  " ) + formatTime( item->latestMtime() );
	case UserCol:		  return limitedInfo ? QVariant() : item->userName();
	case GroupCol:		  return limitedInfo ? QVariant() : item->groupName();
	case PermissionsCol:	  return limitedInfo ? QVariant() : item->symbolicPermissions();
	case OctalPermissionsCol: return limitedInfo ? QVariant() : item->octalPermissions();
    }

    if ( item->isDirInfo() )
    {
	if ( item->readError() )
	{
	    switch ( col )
	    {
		case TotalItemsCol:
		case TotalFilesCol:
		case TotalSubDirsCol:
		    return "?";

		default:
		    break;
	    }
	}

	QString prefix = item->sizePrefix();

	switch ( col )
	{
	    case TotalItemsCol:	  return prefix + QString( "%1" ).arg( item->totalItems() );
	    case TotalFilesCol:	  return prefix + QString( "%1" ).arg( item->totalFiles() );
	    case TotalSubDirsCol:
		if ( item->isDotEntry() )
		    return QVariant();
		else
		    return prefix + QString( "%1" ).arg( item->totalSubDirs() );

	    case OldestFileMTimeCol:  return QString( "	 " ) + formatTime( item->oldestFileMtime() );
	}
    }

    return QVariant();
}


int DirTreeModel::directChildrenCount( FileInfo * subtree ) const
{
    if ( ! subtree )
	return 0;

    int count = subtree->directChildrenCount();

    if ( subtree->attic() )
	++count;

    return count;
}


QString DirTreeModel::sizeText( FileInfo * item, QString (*fmtSz)(FileSize) )
{
    if ( ! item->isFile() )
	return "";

    QString text;

    if ( item->links() > 1 )  // Multiple hard links
    {
	if ( item->isSparseFile() )
	{
	    text = tr( "%1 / %2 Links (allocated: %3)" )
		.arg( fmtSz( item->rawByteSize() ) )
		.arg( item->links() )
		.arg( fmtSz( item->rawAllocatedSize() ) );
	}
	else
	{
	    text = tr( "%1 / %2 Links" )
		.arg( fmtSz( item->rawByteSize() ) )
		.arg( item->links() );
	}
    }
    else // No multiple hard links
    {
	if ( item->isSparseFile() )
	{
	    text = tr( "%1 (allocated: %2)" )
		.arg( fmtSz( item->rawByteSize() ) )
		.arg( fmtSz( item->rawAllocatedSize() ) );
	}
    }

    return text;
}


QString DirTreeModel::smallSizeText( FileInfo * item )
{
    if ( ! item->isFile() && ! item->isSymLink() )
	return "";

    FileSize allocated = item->allocatedSize();
    FileSize size      = item->size();
    QString  text;

    if ( allocated >= 1024 )                    // at least 1k so the (?k) makes sense
    {
        if ( allocated % 1024 == 0   &&         // if it's really even kB
             allocated < 1024 * 1024   )        // and below 1 MB
             // && item->usedPercent() < SMALL_FILE_SHOW_ALLOC_THRESHOLD &&
        {
            if ( size < 1024 )
            {
                text = QString( "%1 B (%2k)" )
                    .arg( size )
                    .arg( allocated / 1024 );
            }
            else
            {
                text = QString( "%1 (%2k)" )
                    .arg( formatSize( size ) )
                    .arg( allocated / 1024 );
            }
        }
    }

    if ( text.isEmpty() )
        return formatSize( size );

    return text;
}


bool DirTreeModel::isSmallFileOrSymLink( FileInfo * item )
{
    if ( item			                  &&
         ( item->isFile() || item->isSymLink()  ) &&
         item->blocks() > 0                       &&
         ! item->isSparseFile()                   &&
         item->tree()		                    )
    {
        FileSize clusterSize = item->tree()->clusterSize();

	if ( clusterSize > 0 )
        {
            if ( item->allocatedSize() <= clusterSize * SMALL_FILE_CLUSTERS )
                return true;

            if ( item->allocatedSize() <= clusterSize * ( SMALL_FILE_CLUSTERS + 1 ) )
            {
                FileSize unused = item->allocatedSize() - item->rawByteSize();

                // 'unused' might be negative for sparse files, but the check
                // will still be successful.

                if ( unused > clusterSize / 2 )
                    return true;
            }
        }
    }

    return false;
}


QVariant DirTreeModel::sizeColText( FileInfo * item ) const
{
    if ( item->isDevice() )
	return QVariant();

    QString leftMargin( 2, ' ' );

    if ( item->isDirInfo() )
	return leftMargin + item->sizePrefix() + formatSize( item->totalAllocatedSize() );

    QString text = sizeText( item );

    if ( text.isEmpty() && isSmallFileOrSymLink( item ) )
	text = smallSizeText( item );

    if ( text.isEmpty() )
	text = leftMargin + formatSize( item->size() );

    return text;
}


QVariant DirTreeModel::columnIcon( FileInfo * item, int col ) const
{
    if ( col != NameCol )
	return QVariant();

    QIcon icon;

    if	    ( item->isDotEntry() )  icon = _dotEntryIcon;
    else if ( item->isAttic() )	    icon = _atticIcon;
    else if ( item->isPkgInfo()	 )  icon = _pkgIcon;
    else if ( item->isExcluded() )  icon = _excludedIcon;
    else if ( item->isDir()	 )
    {
	if	( item->readState() == DirAborted )   icon = _stopIcon;
	else if ( item->readError()		  )   icon = _unreadableDirIcon;
	else if ( item->isMountPoint()		  )   icon = _mountPointIcon;
	else if ( item->isIgnored()		  )   icon = _atticIcon;
	else					      icon = _dirIcon;
    }
    else // ! item->isDir()
    {
	if	( item->readError()		  )   icon = _unreadableDirIcon;
	else if ( item->isFile()		  )   icon = _fileIcon;
	else if ( item->isSymLink()		  )   icon = _symlinkIcon;
	else if ( item->isBlockDevice()		  )   icon = _blockDeviceIcon;
	else if ( item->isCharDevice()		  )   icon = _charDeviceIcon;
	else if ( item->isSpecial()		  )   icon = _specialIcon;
    }

    if ( icon.isNull() )
	return QVariant();

    bool  useDisabled = item->isIgnored() || item->isAttic();
    QSize iconSize( icon.actualSize( QSize( 1024, 1024 ) ) );

    return icon.pixmap( iconSize, useDisabled ?
			QIcon::Disabled : QIcon::Normal );
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
    int count = directChildrenCount( dir );
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
	int count = directChildrenCount( subtree );

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
    QString text = ::formatPercent( percent );

    if ( text.isEmpty() )
        return QVariant();

    return text;
}


void DirTreeModel::refreshSelected()
{
    CHECK_PTR( _selectionModel );
    FileInfo * sel = _selectionModel->selectedItems().first();

    while ( sel &&
	    ( ! sel->isDir() || sel->isPseudoDir() ) &&
	    sel->parent() )
    {
	sel = sel->parent();
    }

    if ( sel && sel->isDirInfo() )
    {
	logDebug() << "Refreshing " << sel << endl;
	busyDisplay();
	FileInfoSet refreshSet;
	refreshSet << sel;
	_selectionModel->prepareRefresh( refreshSet );
	_tree->refresh( sel->toDirInfo() );
    }
    else
    {
	logWarning() << "NOT refreshing " << sel << endl;
    }
}

