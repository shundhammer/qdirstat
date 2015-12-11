/*
 *   File name: DebugHelpers.cpp
 *   Summary:	Debugging helper functions for QDirstat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include <QAbstractItemModel>

#include "DebugHelpers.h"
#include "DirTree.h"
#include "FileInfoIterator.h"

namespace Debug
{
    using namespace QDirStat;

    void dumpDirectChildren( FileInfo * dir, const QString & dirName = QString() )
    {
        if ( ! dir )
            return;

        QString name = dirName.isEmpty() ? dir->debugUrl() : dirName;

        if ( name.isEmpty() && dir == dir->tree()->root() )
            name = "<root>";

        FileInfoIterator it( dir, DotEntryIsSubDir );

        if ( dir->hasChildren() )
        {
            logDebug() << "Children of " << name << endl;
            int count = 0;

            while ( *it )
            {
                logDebug() << "    #" << count++ << ": " << *it << endl;
                ++it;
            }
        }
        else
        {
            logDebug() << "\t\tNo children in " << name << endl;
        }
    }


    void dumpModelTree( const QModelIndex & index, const QString & indent )
    {
        const QAbstractItemModel * model = index.model();

        int rowCount = model->rowCount( index );
        QVariant data = model->data( index, Qt::DisplayRole );

        if ( data.isValid() )
        {
            if ( rowCount > 0 )
                logDebug() << indent << data.toString() << ": " << rowCount << " rows" << endl;
            else
                logDebug() << indent << data.toString() << endl;
        }
        else
        {
            logDebug() << "<No data> " << rowCount << " rows" << endl;
        }

        for ( int row=0; row < rowCount; row++ )
        {
            QModelIndex childIndex = model->index( row, 0, index );
            Debug::dumpModelTree( childIndex, indent + QString( 4, ' ' ) );
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
            {
                logDebug() << data.toString() << endl;
                parents.prepend( data.toString() );
            }

            parent = index.model()->parent( parent );
        }

        return parents;
    }

} // namespace
