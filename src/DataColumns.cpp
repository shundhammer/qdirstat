/*
 *   File name: DataColumns.cpp
 *   Summary:	Data column mapping
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "DataColumns.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;

DataColumns * DataColumns::_instance = 0;


DataColumns * DataColumns::instance()
{
    if ( ! _instance )
    {
	_instance = new DataColumns();
	CHECK_NEW( _instance );
    }

    return _instance;
}


DataColumns::DataColumns():
    QObject()
{
    _columns << NameCol
	     << PercentBarCol
	     << PercentNumCol
	     << TotalSizeCol
	     << OwnSizeCol
	     << TotalItemsCol
	     << TotalFilesCol
	     << TotalSubDirsCol
	     << LatestMTimeCol;
}


void DataColumns::setColumns( const DataColumnList & newColumns )
{
    _columns = newColumns;
    emit columnsChanged();
}


DataColumn DataColumns::mappedCol( DataColumn viewCol ) const
{
    if ( viewCol < 0 || viewCol >= colCount() )
    {
	logError() << "Invalid view column no.: " << viewCol << endl;
	return UndefinedCol;
    }

    return _columns.at( viewCol );
}


DataColumn DataColumns::reverseMappedCol( DataColumn modelCol ) const
{
    if ( modelCol < 0 || modelCol >= colCount() )
	return UndefinedCol;

    return static_cast<DataColumn>( _columns.indexOf( modelCol ) );
}


QString DataColumns::toString( DataColumn col )
{
    switch ( col )
    {
	case NameCol:		return "NameCol";
	case PercentBarCol:	return "PercentBarCol";
	case PercentNumCol:	return "PercentNumCol";
	case TotalSizeCol:	return "TotalSizeCol";
	case OwnSizeCol:	return "OwnSizeCol";
	case TotalItemsCol:	return "TotalItemsCol";
	case TotalFilesCol:	return "TotalFilesCol";
	case TotalSubDirsCol:	return "TotalSubDirsCol";
	case LatestMTimeCol:	return "LatestMTimeCol";
	case ReadJobsCol:	return "ReadJobsCol";
	case UndefinedCol:	return "UndefinedCol";

            // Intentionally omitting 'default' so the compiler
            // can catch unhandled enum values
    }

    return QString( "<Unknown DataColumn %1" ).arg( (int) col );
}
