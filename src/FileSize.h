/*
 *   File name: FileSize.h
 *   Summary:	Basic typedefs for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef FileSize_h
#define FileSize_h


#define FileSizeMax   LLONG_MAX
// 0x7FFFFFFFFFFFFFFFLL == 9223372036854775807LL


namespace QDirStat
{
    typedef long long FileSize;
}


#endif  // FileSize_h
