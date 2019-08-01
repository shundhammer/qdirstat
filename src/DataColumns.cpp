/*
 *   File name: DataColumns.cpp
 *   Summary:	Data column mapping
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */



#include "DataColumns.h"
#include "Settings.h"
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
    _columns = defaultColumns();

#if 0
    // Reading and writing settings for the columns has been taken over by the
    // HeaderTweaker which works with the DirTreeView's QHeaderView. There, the
    // user can choose the columns interactively, move them around, resize them
    // etc. without any model->reset() which would be required with the
    // DataColumns.


    readSettings();

    // Write settings immediately back since the destructor of this singleton
    // is very likely never called.
    writeSettings();
#endif
}


const DataColumnList DataColumns::defaultColumns() const
{
    DataColumnList columns;

    columns << NameCol
	    << PercentBarCol
	    << PercentNumCol
	    << SizeCol
	    << TotalItemsCol
	    << TotalFilesCol
	    << TotalSubDirsCol
	    << LatestMTimeCol
            << OldestFileMTimeCol
	    << UserCol
	    << GroupCol
	    << PermissionsCol
	    << OctalPermissionsCol;

    return columns;
}


void DataColumns::readSettings()
{
    Settings settings;
    settings.beginGroup( "DataColumns" );
    QStringList strColList = settings.value( "Columns" ).toStringList();
    settings.endGroup();

    _columns = fromStringList( strColList );

    if ( _columns.isEmpty() )
	_columns = defaultColumns();
    else
	ensureNameColFirst( _columns );
}


void DataColumns::writeSettings()
{
    Settings settings;
    settings.beginGroup( "DataColumns" );
    settings.setValue( "Columns", toStringList( _columns ) );
    settings.endGroup();
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
	logError() << "Invalid view column no.: " << (int) viewCol << endl;
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
	case NameCol:			return "NameCol";
	case PercentBarCol:		return "PercentBarCol";
	case PercentNumCol:		return "PercentNumCol";
	case SizeCol:		        return "SizeCol";
	case TotalItemsCol:		return "TotalItemsCol";
	case TotalFilesCol:		return "TotalFilesCol";
	case TotalSubDirsCol:		return "TotalSubDirsCol";
	case LatestMTimeCol:		return "LatestMTimeCol";
        case OldestFileMTimeCol:        return "OldestFileMTimeCol";
	case UserCol:			return "UserCol";
	case GroupCol:			return "GroupCol";
	case PermissionsCol:		return "PermissionsCol";
	case OctalPermissionsCol:	return "OctalPermissionsCol";
	case ReadJobsCol:		return "ReadJobsCol";
	case UndefinedCol:		return "UndefinedCol";

	    // Intentionally omitting 'default' so the compiler
	    // can catch unhandled enum values
    }

    logError() << "Unknown DataColumn " << (int) col << endl;
    return QString( "<Unknown DataColumn %1>" ).arg( (int) col );
}


DataColumn DataColumns::fromString( const QString & str )
{
    if ( str == "TotalSizeCol" ) // Backwards compatibility for settings
        return SizeCol;

    for ( int i = DataColumnBegin; i <= DataColumnEnd; ++i )
    {
	DataColumn col = static_cast<DataColumn>( i );

	if ( str == toString( col ) )
	    return col;
    }

    logError() << "Undefined DataColumn \"" << str << "\"" << endl;
    return UndefinedCol;
}


QStringList DataColumns::toStringList( const DataColumnList & colList )
{
    QStringList strList;

    foreach ( DataColumn col, colList )
    {
	strList << toString( col );
    }

    return strList;
}


DataColumnList DataColumns::fromStringList( const QStringList & strList )
{
    DataColumnList colList;

    foreach ( const QString & str, strList )
    {
	DataColumn col = fromString( str );

	if ( col != UndefinedCol )
	    colList << col;
    }

    return colList;
}


void DataColumns::ensureNameColFirst( DataColumnList & colList )
{
    if ( colList.first() != NameCol )
    {
	logError() << "NameCol is required to be first!" << endl;
	colList.removeAll( NameCol );
	colList.prepend( NameCol );
	logError() << "Fixed column list: " << toStringList( colList ) << endl;
    }
}
