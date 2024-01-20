/*
 *   File name: FileSearchFilter.h
 *   Summary:	Package manager Support classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "FileSearchFilter.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


FileSearchFilter::FileSearchFilter( DirInfo *       subtree,
                                    const QString & pattern,
                                    FilterMode      filterMode ):
    SearchFilter( pattern,
                  filterMode,
                  Contains ),   // defaultFilterMode
    _subtree( subtree ),
    _findFiles( true ),
    _findDirs( true ),
    _findSymLinks( true ),
    _findPkg( true )
{
    if ( _filterMode == Auto )
        guessFilterMode();

    if ( _filterMode == Wildcard )
        _regexp.setPatternSyntax( QRegExp::Wildcard );

    _regexp.setCaseSensitivity( Qt::CaseInsensitive );
}
