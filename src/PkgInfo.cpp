/*
 *   File name: PkgInfo.cpp
 *   Summary:	Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "PkgInfo.h"
#include "DirTree.h"

using namespace QDirStat;


PkgInfo::PkgInfo( const QString & name,
                  const QString & version,
                  const QString & arch    ):
    DirInfo( 0,   // tree,
             0,   // parent
             name,
             0,   // mode
             0,   // size
             0 ), // mtime
    _version( version ),
    _arch( arch )
{
    logDebug() << "Creating " << this << endl;
}


PkgInfo::~PkgInfo()
{
    // NOP
}
