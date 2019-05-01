/*
 *   File name: PkgInfo.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "PkgInfo.h"
#include "DirTree.h"
#include "FileInfoIterator.h"

using namespace QDirStat;


PkgInfo::PkgInfo( const QString & name,
                  const QString & version,
                  const QString & arch,
                  PkgManager    * pkgManager ):
    DirInfo( 0,   // tree,
             0,   // parent
             name,
             0,   // mode
             0,   // size
             0 ), // mtime
    _baseName( name ),
    _version( version ),
    _arch( arch ),
    _pkgManager( pkgManager ),
    _multiVersion( false ),
    _multiArch( false )
{
    // logDebug() << "Creating " << this << endl;
}


PkgInfo::PkgInfo( DirTree *       tree,
                  DirInfo *       parent,
                  const QString & name,
                  PkgManager    * pkgManager ):
    DirInfo( tree,
             parent,
             name,
             0,   // mode
             0,   // size
             0 ), // mtime
    _baseName( name ),
    _pkgManager( pkgManager ),
    _multiVersion( false ),
    _multiArch( false )
{
    // logDebug() << "Creating " << this << endl;
}


PkgInfo::~PkgInfo()
{
    // NOP
}


QString PkgInfo::url() const
{
    QString name = _name;

    if ( isPkgUrl( name ) )
        name = "";

    return QString( "Pkg:/%1" ).arg( name );
}


bool PkgInfo::isPkgUrl( const QString & url )
{
    return url.startsWith( "Pkg:" );
}


QString PkgInfo::pkgUrl( const QString & path ) const
{
    if ( isPkgUrl( path ) )
        return path;
    else
        return url() + path;
}


FileInfo * PkgInfo::locate( const QString & path )
{
    QStringList components = path.split( "/", QString::SkipEmptyParts );

    if ( isPkgUrl( path ) )
    {
        components.removeFirst();       // Remove the leading "Pkg:"

        if ( components.isEmpty() )
            return ( this == _tree->root() ) ? this : 0;

        QString pkgName = components.takeFirst();

        if ( pkgName != _name )
        {
            logError() << "Path " << path << " does not belong to " << this << endl;
            return 0;
        }

        if ( components.isEmpty() )
            return this;
    }

    return locate( this, components );
}


FileInfo * PkgInfo::locate( const QStringList & pathComponents )
{
    return locate( this, pathComponents );
}


FileInfo * PkgInfo::locate( DirInfo *           subtree,
                            const QStringList & pathComponents )
{
    // logDebug() << "Locating /" << pathComponents.join( "/" ) << " in " << subtree << endl;

    if ( ! subtree || pathComponents.isEmpty() )
        return 0;

    QStringList components = pathComponents;
    QString     wanted     = components.takeFirst();

    FileInfoIterator it( subtree );

    while ( *it )
    {
        // logDebug() << "Checking " << (*it)->name() << " in " << subtree << " for " << wanted << endl;

        if ( (*it)->name() == wanted )
        {
            if ( components.isEmpty() )
            {
                // logDebug() << "  Found " << *it << endl;
                return *it;
            }
            else
            {
                if ( ! (*it)->isDirInfo() )
                    return 0;
                else
                    return locate( (*it)->toDirInfo(), components );
            }
        }

        ++it;
    }

    return 0;
}
