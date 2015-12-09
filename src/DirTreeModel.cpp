/*
 *   File name: DirTreeModel.h
 *   Summary:	Qt data model for directory tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DirTreeModel.h"
#include "DirTree.h"
#include "FileInfoIterator.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


DirTreeModel::DirTreeModel( QObject * parent ):
    QAbstractItemModel( parent ),
    _tree(0),
    _dotEntryPolicy( DotEntryIsSubDir ),
    _readJobsCol( PercentBarCol )
{
    loadIcons();
    _colMapping << NameCol
		<< PercentBarCol
		<< PercentNumCol
		<< TotalSizeCol
		<< OwnSizeCol
		<< TotalItemsCol
		<< TotalFilesCol
		<< TotalSubDirsCol
		<< LatestMTimeCol;
}


DirTreeModel::~DirTreeModel()
{
    if ( _tree )
	delete _tree;
}


void DirTreeModel::clear()
{
    if ( _tree )
    {
	beginResetModel();
	delete _tree;
	endResetModel();
    }
}


void DirTreeModel::openUrl( const QString &url )
{
    createTree();
    _tree->startReading( url );
}


void DirTreeModel::createTree()
{
    clear();
    _tree = new DirTree();
    CHECK_NEW( _tree );

    connect( _tree, SIGNAL( finished()	      ),
	     this,  SLOT  ( readingFinished() ) );

    connect( _tree, SIGNAL( aborted()	      ),
	     this,  SLOT  ( readingFinished() ) );

    connect( _tree, SIGNAL( finished	   ( DirInfo * ) ),
	     this,  SLOT  ( readingFinished( DirInfo * ) ) );
}


void DirTreeModel::loadIcons()
{
    _openDirIcon	= QPixmap( ":/icons/open-dir.png"	  );
    _closedDirIcon	= QPixmap( ":/icons/closed-dir.png"	  );
    _openDotEntryIcon	= QPixmap( ":/icons/open-dot-entry.png"	  );
    _closedDotEntryIcon = QPixmap( ":/icons/closed-dot-entry.png" );
    _unreadableDirIcon	= QPixmap( ":/icons/unreadable-dir.png"	  );
    _mountPointIcon	= QPixmap( ":/icons/mount-point.png"	  );
    _fileIcon		= QPixmap( ":/icons/file.png"		  );
    _symLinkIcon	= QPixmap( ":/icons/sym-link.png"	  );
    _blockDevIcon	= QPixmap( ":/icons/block-dev.png"	  );
    _charDevIcon	= QPixmap( ":/icons/char-dev.png"	  );
    _fifoIcon		= QPixmap( ":/icons/fifo.png"		  );
    _stopIcon		= QPixmap( ":/icons/stop.png"		  );
}


void DirTreeModel::setColumns( QList<Column> columns )
{
    beginResetModel();
    _colMapping = columns;
    endResetModel();
}


int DirTreeModel::mappedCol( int viewCol ) const
{
    if ( viewCol < 0 || viewCol >= colCount() )
    {
	// logError() << "Invalid view column no.: " << viewCol << endl;
	return 0;
    }

    return _colMapping.at( viewCol );
}


FileInfo * DirTreeModel::findChild( FileInfo * parent, int childNo ) const
{
    FileInfoIterator it( parent, _dotEntryPolicy );
    int index = 0;

    while ( *it && index != childNo )
    {
	++index;
	++it;
    }

    return *it;
}


int DirTreeModel::childIndex( FileInfo * child ) const
{
    if ( ! child->parent() )
	return 0;

    FileInfoIterator it( child->parent(), _dotEntryPolicy );
    int index = 0;

    while ( *it )
    {
	if ( *it == child )
	    return index;

	++index;
	++it;
    }

    if ( child == child->parent()->dotEntry() )
	return index;

    // Not found
    logError() << "Child \"" << child << "\" not found in \""
	       << child->parent() << "\"" << endl;
    return -1;
}


int DirTreeModel::countDirectChildren( FileInfo * parent ) const
{
    FileInfoIterator it( parent, _dotEntryPolicy );

    return it.count();
}



//
// Reimplented from QAbstractItemModel
//

int DirTreeModel::rowCount( const QModelIndex &parentIndex ) const
{
    if ( ! _tree )
	return 0;

    int count = 0;
    FileInfo * parentItem = 0;

    if ( parentIndex.isValid() )
	parentItem = static_cast<FileInfo *>( parentIndex.internalPointer() );
    else
	parentItem = _tree->root();

    QString parentName = parentItem == _tree->root() ? "<root>" : parentItem->name();

    switch ( parentItem->readState() )
    {
	case DirQueued:
            count = 0;  // Nothing yet
	    break;

	case DirReading:

	    // Don't mess with directories that are currently being read:
	    // If we tell our view about them, the view might begin fetching
	    // model indexes for them, and when the tree later sends the
	    // readingFinished() signal, the beginInsertRows() call in our
	    // readingFinished() slot will confuse the view; it would assume
	    // that the number of children reported in that beginInsertRows()
	    // call needs to be added to the number reported here. We'd have to
	    // keep track how many children we already reported, and how many
	    // new ones to report later.
	    //
	    // Better keep it simple: Don't report any children until they
	    // are complete.
            count = 0;
            break;

	case DirFinished:
	case DirOnRequestOnly:
	case DirCached:
	case DirAborted:
	case DirError:
	    count = countDirectChildren( parentItem );
            break;

	// intentionally omitting 'default' case so the compiler can report
	// missing enum values
    }

    logDebug() << parentName << ": " << count << endl;

    return count;
}


int DirTreeModel::columnCount( const QModelIndex &parent ) const
{
    Q_UNUSED( parent );

    return colCount();
}


QVariant DirTreeModel::data( const QModelIndex &index, int role ) const
{
    if ( ! index.isValid() )
	return QVariant();

    int col = mappedCol( index.column() );

    switch ( role )
    {
	case Qt::DisplayRole:
	    {
		FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
		// No need for CHECK_PTR( item ): columnText() does that.
		return columnText( item, col );
	    }

	case Qt::TextAlignmentRole:
	    {
		switch ( col )
		{
		    case PercentNumCol:
		    case TotalSizeCol:
		    case OwnSizeCol:
		    case TotalItemsCol:
		    case TotalFilesCol:
		    case TotalSubDirsCol:
			return Qt::AlignRight;

		    case NameCol:
		    case PercentBarCol:
		    case LatestMTimeCol:
		    default:
			return Qt::AlignLeft;
		}
	    }

	case SortRole: // Custom QDirStat role: Raw types for sorting
	    {
		// Together with a QSortProxyFilterModel that uses this custom
		// role as its sortRole, this takes care of sorting: The
		// QSortProxyFilterModel queries this model for data for the
		// sort column and uses QVariant and its known types and their
		// operator<() (in its lessThan() function). Since all columns
		// we use are either strings or simple numeric values, all we
		// need to do is to return the corresponding string or numeric
		// value for each column. Notice that we don't even have to
		// calculate any percentages here since for the purpose of
		// sorting, the percentage behaves exactly like the totalSize.

		FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
		CHECK_PTR( item );

		if ( col == _readJobsCol && item->isBusy() )
		    return item->pendingReadJobs();

		switch ( col )
		{
		    case NameCol:	  return item->name();
		    case PercentBarCol:	  // FALLTHRU
		    case PercentNumCol:	  // FALLTHRU
		    case TotalSizeCol:	  return item->totalSize();
		    case OwnSizeCol:	  return item->size();
		    case TotalItemsCol:	  return item->totalItems();
		    case TotalFilesCol:	  return item->totalFiles();
		    case TotalSubDirsCol: return item->totalSubDirs();
		    case LatestMTimeCol:  return (qulonglong) item->latestMtime();
		    default:		  return QVariant();
		}
	    }


	default:
	    return QVariant();
    }

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
	    switch ( mappedCol( section ) )
	    {
		case NameCol:		return tr( "Name"		);
		case PercentBarCol:	return tr( "Subtree Percentage" );
		case PercentNumCol:	return tr( "Percentage"		);
		case TotalSizeCol:	return tr( "Subtree Total"	);
		case OwnSizeCol:	return tr( "Own Size"		);
		case TotalItemsCol:	return tr( "Items"		);
		case TotalFilesCol:	return tr( "Files"		);
		case TotalSubDirsCol:	return tr( "Subdirs"		);
		case LatestMTimeCol:	return tr( "Last Modified"	);
		default:		return QVariant();
	    }

	case Qt::TextAlignmentRole:
	    return Qt::AlignLeft;

	default:
	    return QVariant();
    }
}


Qt::ItemFlags DirTreeModel::flags( const QModelIndex &index ) const
{
    if ( ! index.isValid() )
    {
	return Qt::NoItemFlags;
    }

    int col = mappedCol( index.column() );
    switch ( col )
    {
	case NameCol:
	    return Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	case PercentBarCol:
	    return Qt::ItemIsEnabled;

	default:
	    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
}


QModelIndex DirTreeModel::index( int row, int column, const QModelIndex & parentIndex ) const
{
    if ( ! _tree  || ! _tree->root() || ! hasIndex( row, column, parentIndex ) )
	return QModelIndex();

    FileInfo *parent;

    if ( ! parentIndex.isValid() )
	parent = _tree->root();
    else
	parent = static_cast<FileInfo *>( parentIndex.internalPointer() );

    FileInfo * child = findChild( parent, row );

    if ( child )
	return createIndex( row, column, child );
    else
	return QModelIndex();
}


QModelIndex DirTreeModel::parent( const QModelIndex &index ) const
{
    if ( ! index.isValid() )
	return QModelIndex();

    FileInfo * child = static_cast<FileInfo*>( index.internalPointer() );
    CHECK_PTR( child );
    FileInfo * parent = child->parent();

    if ( ! parent || parent == _tree->root() )
	return QModelIndex();

    int row = childIndex( parent );
    return createIndex( row, 0, child );
}


//---------------------------------------------------------------------------


QModelIndex DirTreeModel::modelIndex( FileInfo * item, int column ) const
{
    if ( ( _tree ) && _tree->root() && item )
	return createIndex( childIndex( item ), column, item );
    else
	return QModelIndex();
}



QVariant DirTreeModel::columnText( FileInfo * item, int col ) const
{
    CHECK_PTR( item );

    if ( col == _readJobsCol && item->isBusy() )
    {
	return tr( "[%1 Read Jobs]" ).arg( item->pendingReadJobs() );
    }

    switch ( col )
    {
	case NameCol:
	    logDebug() << "data() fetched name of " << item << endl;
	    return item->name();
	case PercentBarCol:	return item->isExcluded() ? tr( "[Excluded]" ) : QVariant();
	case OwnSizeCol:	return ownSizeColText( item );
	case PercentNumCol:	return formatPercent( item->subtreePercent() );
	case LatestMTimeCol:	return formatTime( item->latestMtime() );
    }

    if ( item->isDir() || item->isDotEntry() )
    {
	QString prefix = item->readState() == DirAborted ? ">" : "";

	switch ( col )
	{
	    case TotalSizeCol:	  return prefix + formatSize( item->totalSize() );
	    case TotalItemsCol:	  return prefix + QString( "%1" ).arg( item->totalItems() );
	    case TotalFilesCol:	  return prefix + QString( "%1" ).arg( item->totalFiles() );
	    case TotalSubDirsCol: return prefix + QString( "%1" ).arg( item->totalSubDirs() );
	}
    }

    return QVariant();
}


QVariant DirTreeModel::ownSizeColText( FileInfo * item ) const
{
    if ( item->isDevice() )
	return QVariant();

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


void DirTreeModel::readingFinished( DirInfo * dir )
{
    logDebug() << ( dir == _tree->root() ? "<root>" : dir->url() ) << endl;

    if ( anyAncestorBusy( dir ) )
    {
	logDebug() << "Ancestor busy - ignoring readingFinished for "
		   << dir << endl;
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
    logDebug() << ( dir == _tree->root() ? "<root>" : dir->url() ) << endl;

    if ( ! dir )
    {
	logError() << "NULL DirInfo *" << endl;
	return;
    }

    QModelIndex parentIndex = modelIndex( dir->parent() );
    int count = countDirectChildren( dir );
    dumpChildren( dir ); // DEBUG

    if ( count > 0 )
    {
	logDebug() << "Notifying view about " << count << " new children of" << dir << endl;
	dumpChildren( dir );

        DirReadState saved = dir->readState();
        dir->setReadState( DirReading ); // Make sure rowCount() returns 0 for this dir

	beginInsertRows( parentIndex, 0, count - 1 );
        dir->setReadState( saved );
	endInsertRows();
    }

    // If any readingFinished signals were ignored because a parent was not
    // finished yet, now is the time to notify the view about those children,
    // too.
    FileInfoIterator it( dir, _dotEntryPolicy );

    while ( *it )
    {
	if ( (*it)->isDir() &&
	     (*it)->readState() != DirReading &&
	     (*it)->readState() != DirQueued	)
	{
	    newChildrenNotify( static_cast<DirInfo *>(*it) );
	}
	++it;
    }
}


void DirTreeModel::readingFinished()
{
#if 0
    beginResetModel();
    endResetModel();
#endif

    // TO DO: Finalize display

    dumpChildren( _tree->root(), "root" );
}


QVariant DirTreeModel::formatPercent( float percent ) const
{
    if ( percent < 0.0 )	// Invalid percentage?
	return QVariant();

    QString text;
    text.sprintf( "%.1f%%", percent );

    return text;
}


void DirTreeModel::dumpChildren( DirInfo * dir, const QString & dirName ) const
{
    if ( ! dir )
	return;

    QString name = dirName.isEmpty() ? dir->name() : dirName;

    if ( name.isEmpty() && dir == _tree->root() )
	name = "<root>";

    FileInfoIterator it( dir, _dotEntryPolicy );

    if ( dir->hasChildren() )
    {
	logDebug() << "\tChildren of " << name << endl;
	int count = 0;

	while ( *it )
	{
	    logDebug() << "\t	 #" << count++ << ": " << *it << endl;
	    ++it;
	}
    }
    else
    {
	logDebug() << "\t\tNo children in " << name << endl;
    }
}


