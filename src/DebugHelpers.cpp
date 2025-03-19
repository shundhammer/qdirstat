/*
 *   File name: DebugHelpers.cpp
 *   Summary:	Debugging helper functions for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QAbstractItemModel>

#include "DebugHelpers.h"
#include "DirTree.h"
#include "FileInfoIterator.h"
#include "ExcludeRules.h"
#include "FormatUtil.h"


namespace Debug
{
    using namespace QDirStat;

    void dumpDirectChildren( FileInfo * dir )
    {
	if ( ! dir )
	    return;

	FileInfoIterator it( dir );

	if ( dir->hasChildren() )
	{
	    logDebug() << "Children of " << dir
		       << "  (" << (void *) dir << ")"
		       << ENDL;
	    int count = 0;

	    while ( *it )
	    {
		logDebug() << "	   #" << count++ << ": "
			   << (void *) *it
			   << "	 " << *it
			   << ENDL;
		++it;
	    }
	}
	else
	{
	    logDebug() << "    No children in " << dir << ENDL;
	}
    }


    void dumpChildrenList( FileInfo	      * dir,
			   const FileInfoList & children )
    {
	logDebug() << "Children of " << dir << ENDL;

	for ( int i=0; i < children.size(); ++i )
	{
	    logDebug() << "    #" << i << ": " << children.at(i) << ENDL;
	}
    }


    void dumpChildrenBySize( FileInfo * dir )
    {
	logDebug() << "Direct children of " << dir << " by size:" << ENDL;

	QDirStat::FileInfoSortedBySizeIterator it( dir );

	while ( *it )
	{
	    logDebug() << "  " << formatSize( (*it)->totalSize() ) << "	 " << *it << ENDL;
	    ++it;
	}
    }


    void dumpModelTree( const QAbstractItemModel * model,
			const QModelIndex	 & index,
			const QString		 & indent )
    {
	int rowCount = model->rowCount( index );
	QVariant data = model->data( index, Qt::DisplayRole );

	if ( data.isValid() )
	{
	    if ( rowCount > 0 )
		logDebug() << indent << data.toString() << ": " << rowCount << " rows" << ENDL;
	    else
		logDebug() << indent << data.toString() << ENDL;
	}
	else
	{
	    logDebug() << "<No data> " << rowCount << " rows" << ENDL;
	}

	for ( int row=0; row < rowCount; row++ )
	{
	    QModelIndex childIndex = model->index( row, 0, index );
	    Debug::dumpModelTree( model, childIndex, indent + QString( 4, ' ' ) );
	}
    }


    QStringList modelTreeAncestors( const QModelIndex & index )
    {
	QStringList parents;
	QModelIndex parent = index;

	while ( parent.isValid() )
	{
	    QVariant data = index.model()->data( parent, 0 );

	    if ( data.isValid() )
		parents.prepend( data.toString() );

	    parent = index.model()->parent( parent );
	}

	return parents;
    }


    void dumpExcludeRules()
    {
	if ( ExcludeRules::instance()->isEmpty() )
	    logDebug() << "No exclude rules defined" << ENDL;

	for ( ExcludeRuleListIterator it = ExcludeRules::instance()->begin();
	      it != ExcludeRules::instance()->end();
	      ++it )
	{
	    logDebug() << *it << ENDL;
	}
    }


} // namespace
