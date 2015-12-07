/*
 *   File name: DirTreeModel.h
 *   Summary:	Qt data model for directory tree
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DirTreeModel.h"
#include "DirTree.h"
#include "DirTreeIterator.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


DirTreeModel::DirTreeModel( QObject * parent ):
    QAbstractItemModel( parent ),
    _tree(0),
    _dotEntryPolicy( DotEntryIsSubDir )
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
	delete _tree;

	// TO DO: Send model signals
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
    _colMapping = columns;
    // TO DO: reset view and send signals
}


int DirTreeModel::mappedCol( unsigned viewCol ) const
{
    if ( (int) viewCol >= colCount() )
    {
	logError() << "Invalid view column no.: " << viewCol << endl;
	return 0;
    }

    return _colMapping.at( (int) viewCol );
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

    FileInfo * parentItem;

    if ( parentIndex.isValid() )
	parentItem = static_cast<FileInfo *>( parentIndex.internalPointer() );
    else
	parentItem = _tree->root();

    return countDirectChildren( parentItem );
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
	return Qt::NoItemFlags;

    switch ( mappedCol( index.column() ) )
    {
	case NameCol:
	    return Qt::ItemIsUserCheckable & Qt::ItemIsSelectable & Qt::ItemIsEnabled;

	case PercentBarCol:
	    return Qt::ItemIsEnabled;

	default:
	    return Qt::ItemIsSelectable & Qt::ItemIsEnabled;
    }
}


QModelIndex DirTreeModel::index( int row, int column, const QModelIndex & parentIndex ) const
{
    if ( ( ! _tree ) || ! hasIndex( row, column, parentIndex ) )
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

    if ( ! parent )
	return QModelIndex();

    int row = childIndex( parent );
    return createIndex( row, 0, parent );
}


//---------------------------------------------------------------------------


QVariant DirTreeModel::columnText( FileInfo * item, int col ) const
{
    CHECK_PTR( item );
    
    switch ( col )
    {
        case NameCol:
            {
                if ( item->isDotEntry() )
                    return FileInfo::dotEntryName();
                else
                    return item->name();
            }

#if TODO
            // TO DO
            // TO DO
            // TO DO
            // Check out src-unported/DirTreeView.cpp
            
        case PercentBarCol:	return QVariant();
        case PercentNumCol:	return "TO DO";
        case TotalSizeCol:	return "TO DO";
        case OwnSizeCol:	return "TO DO";
        case TotalItemsCol:	return "TO DO";
        case TotalFilesCol:	return "TO DO";
        case TotalSubDirsCol:   return "TO DO";
        case LatestMTimeCol:    return "TO DO";
            // TO DO
            // TO DO
            // TO DO
#endif
        default: return QVariant();
    }
}
