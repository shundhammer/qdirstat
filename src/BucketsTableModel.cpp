/*
 *   File name: BucketsTableModel.h
 *   Summary:	Data model for buckets table
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "BucketsTableModel.h"
#include "HistogramView.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


BucketsTableModel::BucketsTableModel( QWidget * parent, HistogramView * histogram ):
    QAbstractTableModel( parent ),
    _histogram( histogram )
{
    // logDebug() << "init" << endl;
}


BucketsTableModel::~BucketsTableModel()
{
    // logDebug() << "destroying" << endl;
}


void BucketsTableModel::reset()
{
    beginResetModel();
    endResetModel();
}


int BucketsTableModel::rowCount( const QModelIndex & parent ) const
{
    return parent.isValid() ? 0 : _histogram->bucketCount();
}


int BucketsTableModel::columnCount( const QModelIndex & parent ) const
{
    Q_UNUSED( parent );

    return ColCount;
}


QVariant BucketsTableModel::data( const QModelIndex & index, int role ) const
{
    if ( ! index.isValid() )
	return QVariant();

    switch ( role )
    {
	case Qt::DisplayRole:
	    {
                int row = index.row();

                if ( row < 0 || row >= _histogram->bucketCount() )
                    return QVariant();

                QString result;

                switch ( index.column() )
                {
                    case StartCol:  result = formatSize( _histogram->bucketStart( row ) ); break;
                    case EndCol:    result = formatSize( _histogram->bucketEnd  ( row ) ); break;
                    case ValueCol:  result = QString::number( _histogram->bucket( row ) ); break;

                    default:        return QVariant();
                }

                return " " + result + " "; // Maintain some margin
	    }

	case Qt::TextAlignmentRole:
            return (int) Qt::AlignVCenter | Qt::AlignRight;

	default:
	    return QVariant();
    }

    /*NOTREACHED*/
    return QVariant();
}


QVariant BucketsTableModel::headerData( int	        section,
                                        Qt::Orientation orientation,
                                        int	        role ) const
{
    switch ( role )
    {
	case Qt::DisplayRole:
            if ( orientation == Qt::Horizontal )
            {
                QString result;

                switch ( section )
                {
                    case StartCol:	result = tr( "Start"         ); break;
                    case EndCol:	result = tr( "End"           ); break;
                    case ValueCol:	result = tr( "Files"         ); break;

                    default: return QVariant();
                }

                return " " + result + " "; // Maintain some margin
            }
            else
            {
                if ( section < _histogram->bucketCount() )
                    return " " + QString::number( section + 1 ) + " ";
                else
                    return QVariant();
            }

	case Qt::TextAlignmentRole:
            {
                if ( orientation == Qt::Horizontal )
                    return (int) Qt::AlignVCenter | Qt::AlignHCenter;
                else
                    return (int) Qt::AlignVCenter | Qt::AlignRight;
            }

	default:
	    return QVariant();
    }
}


Qt::ItemFlags BucketsTableModel::flags( const QModelIndex &index ) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags( index );
    flags |= Qt::ItemIsSelectable;

    return flags;
}

