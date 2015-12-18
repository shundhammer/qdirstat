/*
 *   File name: DirTreeModel.h
 *   Summary:   Qt data model for directory tree
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:    Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DirTreeModel.h"
#include "DirTree.h"
#include "FileInfoIterator.h"
#include "Logger.h"
#include "Exception.h"
#include "DebugHelpers.h"


using namespace QDirStat;


DirTreeModel::DirTreeModel( QObject * parent ):
    QAbstractItemModel( parent ),
    _tree(0),
    _dotEntryPolicy( DotEntryIsSubDir ),
    _readJobsCol( PercentBarCol )
{
    _treeIconDir = ":/icons/tree-medium/"; // TO DO: Configurable
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
    createTree();
}


DirTreeModel::~DirTreeModel()
{
    if ( _tree )
        delete _tree;
}


void DirTreeModel::createTree()
{
    _tree = new DirTree();
    CHECK_NEW( _tree );

    connect( _tree, SIGNAL( finished()        ),
             this,  SLOT  ( readingFinished() ) );

    connect( _tree, SIGNAL( aborted()         ),
             this,  SLOT  ( readingFinished() ) );

    connect( _tree, SIGNAL( readJobFinished( DirInfo * ) ),
             this,  SLOT  ( readJobFinished( DirInfo * ) ) );
}


void DirTreeModel::clear()
{
    if ( _tree )
    {
        beginResetModel();
        delete _tree;
        createTree();
        endResetModel();
    }
}


void DirTreeModel::openUrl( const QString &url )
{
    if ( _tree && _tree->root() &&  _tree->root()->hasChildren() )
        clear();

    _tree->startReading( url );
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
        
    _dirIcon           = QPixmap( _treeIconDir + "dir.png"            );
    _fileIcon          = QPixmap( _treeIconDir + "file.png"           );
    _dotEntryIcon      = QPixmap( _treeIconDir + "dot-entry.png"      );
    _unreadableDirIcon = QPixmap( _treeIconDir + "unreadable-dir.png" );
    _mountPointIcon    = QPixmap( _treeIconDir + "mount-point.png"    );
    _stopIcon          = QPixmap( _treeIconDir + "stop.png"           );
    _excludedIcon      = QPixmap( _treeIconDir + "excluded.png"       );
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


int DirTreeModel::rowNumber( FileInfo * child ) const
{
    if ( ! child->parent() )
        return 0;

    FileInfoIterator it( child->parent(), _dotEntryPolicy );
    int row = 0;

    while ( *it )
    {
        if ( *it == child )
            return row;

        ++row;
        ++it;
    }

    if ( child == child->parent()->dotEntry() )
        return row;

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
    FileInfo * item = 0;

    if ( parentIndex.isValid() )
        item = static_cast<FileInfo *>( parentIndex.internalPointer() );
    else
        item = _tree->root();

    if ( ! item->isDirInfo() )
        return 0;

    QString dirName = item == _tree->root() ? "<root>" : item->debugUrl();

    if ( item->toDirInfo()->isLocked() )
    {
        // logDebug() << dirName << " is locked - returning 0" << endl;
        return 0;
    }

    switch ( item->readState() )
    {
        case DirQueued:
            count = 0;  // Nothing yet
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
            logError() << "ERROR: DirReading " << item << " - we shouldn't ever get here " << endl;
            count = 0;
            break;

        case DirFinished:
        case DirOnRequestOnly:
        case DirCached:
        case DirAborted:
        case DirError:
            count = countDirectChildren( item );
            break;

        // intentionally omitting 'default' case so the compiler can report
        // missing enum values
    }

    // logDebug() << dirName << ": " << count << endl;
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
                QVariant result = columnText( item, col );
                return result;
            }

        case Qt::DecorationRole:
            {
                FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
                QVariant result = columnIcon( item, col );
                return result;
            }

        case Qt::TextAlignmentRole:
            {
                int alignment = Qt::AlignVCenter;

                switch ( col )
                {
                    case PercentNumCol:
                    case TotalSizeCol:
                    case OwnSizeCol:
                    case TotalItemsCol:
                    case TotalFilesCol:
                    case TotalSubDirsCol:
                        alignment |= Qt::AlignRight;
                        break;

                    case NameCol:
                    case PercentBarCol:
                    case LatestMTimeCol:
                    default:
                        alignment |= Qt::AlignLeft;
                        break;
                }

                return alignment;
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
                    case NameCol:         return item->name();
                    case PercentBarCol:   // FALLTHRU
                    case PercentNumCol:   // FALLTHRU
                    case TotalSizeCol:    return item->totalSize();
                    case OwnSizeCol:      return item->size();
                    case TotalItemsCol:   return item->totalItems();
                    case TotalFilesCol:   return item->totalFiles();
                    case TotalSubDirsCol: return item->totalSubDirs();
                    case LatestMTimeCol:  return (qulonglong) item->latestMtime();
                    default:              return QVariant();
                }
            }

        default:
            return QVariant();
    }

    return QVariant();
}


QVariant DirTreeModel::headerData( int             section,
                                   Qt::Orientation orientation,
                                   int             role ) const
{
    if ( orientation != Qt::Horizontal )
        return QVariant();

    switch ( role )
    {
        case Qt::DisplayRole:
            switch ( mappedCol( section ) )
            {
                case NameCol:           return tr( "Name"               );
                case PercentBarCol:     return tr( "Subtree Percentage" );
                case PercentNumCol:     return tr( "Percentage"         );
                case TotalSizeCol:      return tr( "Subtree Total"      );
                case OwnSizeCol:        return tr( "Own Size"           );
                case TotalItemsCol:     return tr( "Items"              );
                case TotalFilesCol:     return tr( "Files"              );
                case TotalSubDirsCol:   return tr( "Subdirs"            );
                case LatestMTimeCol:    return tr( "Last Modified"      );
                default:                return QVariant();
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
        return Qt::NoItemFlags;


    FileInfo * item = static_cast<FileInfo *>( index.internalPointer() );
    CHECK_PTR( item );
    Qt::ItemFlags baseFlags = Qt::ItemIsEnabled;

    if ( ! item->isDirInfo() )
        baseFlags |= Qt::ItemNeverHasChildren;

    // logDebug() << "Flags for row #" << index.row() << " col #" << index.column() << ": "  << item << endl;

    int col = mappedCol( index.column() );

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
        parent = static_cast<FileInfo *>( parentIndex.internalPointer() );
    else
        parent = _tree->root();

    FileInfo * child = findChild( parent, row );
    CHECK_PTR( child );

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
    CHECK_PTR( parent );

    if ( ! parent || parent == _tree->root() )
        return QModelIndex();

    int row = rowNumber( parent );
    // logDebug() << "Parent of " << child << " is " << parent << " #" << row << endl;

    return createIndex( row, 0, parent );
}


//---------------------------------------------------------------------------


QModelIndex DirTreeModel::modelIndex( FileInfo * item, int column ) const
{
    CHECK_PTR( _tree );
    CHECK_PTR( _tree->root() );

    if (  ! item || item == _tree->root() )
        return QModelIndex();
    else
    {
        int row = rowNumber( item );
        // logDebug() << item << " is row #" << row << " of " << item->parent() << endl;
        return createIndex( row, column, item );
    }
}



QVariant DirTreeModel::columnText( FileInfo * item, int col ) const
{
    CHECK_PTR( item );

    if ( col == _readJobsCol && item->isBusy() )
        return tr( "[%1 Read Jobs]" ).arg( item->pendingReadJobs() );

    switch ( col )
    {
        case NameCol:           return item->name();
        case PercentBarCol:     return item->isExcluded() ? tr( "[Excluded]" ) : QVariant();
        case OwnSizeCol:        return ownSizeColText( item );
        case PercentNumCol:     return formatPercent( item->subtreePercent() );
        case LatestMTimeCol:    return formatTime( item->latestMtime() );
    }

    if ( item->isDirInfo() || item->isDotEntry() )
    {
        QString prefix = item->readState() == DirAborted ? ">" : "";

        switch ( col )
        {
            case TotalSizeCol:    return prefix + formatSize( item->totalSize() );
            case TotalItemsCol:   return prefix + QString( "%1" ).arg( item->totalItems() );
            case TotalFilesCol:   return prefix + QString( "%1" ).arg( item->totalFiles() );
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


QVariant DirTreeModel::columnIcon( FileInfo * item, int col ) const
{
    if ( col != NameCol )
        return QVariant();

    QPixmap icon;

    if      ( item->isDotEntry() )  icon = _dotEntryIcon;
    else if ( item->isExcluded() )  icon = _excludedIcon;
    else if ( item->isDir()      )
    {
        if      ( item->readState() == DirAborted )   icon = _stopIcon;
        else if ( item->readState() == DirError   )   icon = _unreadableDirIcon;
        else if ( item->isMountPoint()            )   icon = _mountPointIcon;
        else                                          icon = _dirIcon;
    }
    else // ! item->isDir()
    {
        if      ( item->isFile()        )  icon = _fileIcon;
        else if ( item->isSymLink()     )  icon = _fileIcon; // TO DO: Find a better icon
        else if ( item->isBlockDevice() )  icon = _fileIcon; // TO DO: Find a better icon
        else if ( item->isCharDevice()  )  icon = _fileIcon; // TO DO: Find a better icon
        else if ( item->isSpecial()     )  icon = _fileIcon; // TO DO: Find a better icon
    }

    return icon.isNull() ? QVariant() : icon;
}


void DirTreeModel::readJobFinished( DirInfo * dir )
{
    logDebug() << ( dir == _tree->root() ? "<root>" : dir->url() ) << endl;

    if ( anyAncestorBusy( dir ) )
    {
        logDebug() << "Ancestor busy - ignoring readJobFinished for "
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
    QString dirName = dir == _tree->root() ? "<root>" : dir->debugUrl();
    logDebug() << dirName << endl;

    if ( ! dir )
    {
        logError() << "NULL DirInfo *" << endl;
        return;
    }

    QModelIndex index = modelIndex( dir );
    int count = countDirectChildren( dir );
    // Debug::dumpDirectChildren( dir );

    if ( count > 0 )
    {
        logDebug() << "Notifying view about " << count << " new children of " << dirName << endl;

        dir->lock();
        beginInsertRows( index, 0, count - 1 );
        dir->unlock();
        endInsertRows();
    }

    // If any readJobFinished signals were ignored because a parent was not
    // finished yet, now is the time to notify the view about those children,
    // too.
    FileInfoIterator it( dir, _dotEntryPolicy );

    while ( *it )
    {
        if ( (*it)->isDirInfo() &&
             (*it)->readState() != DirReading &&
             (*it)->readState() != DirQueued    )
        {
            newChildrenNotify( (*it)->toDirInfo() );
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

    // Debug::dumpDirectChildren( _tree->root(), "root" );
}


QVariant DirTreeModel::formatPercent( float percent ) const
{
    if ( percent < 0.0 )        // Invalid percentage?
        return QVariant();

    QString text;
    text.sprintf( "%.1f%%", percent );

    return text;
}


