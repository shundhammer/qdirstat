/*
 *   File name: FileInfoSorter.cpp
 *   Summary:	Functor to handle sorting FileInfo objects
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <algorithm>
#include "FileInfoSorter.h"

using namespace QDirStat;


bool FileInfoSorter::operator() ( FileInfo * a, FileInfo * b )
{
    if ( !a || !b ) return false;

    if ( _sortOrder == Qt::DescendingOrder )
	std::swap( a, b ); // Now we only need to handle the a < b case

    switch ( _sortCol )
    {
	case NameCol:
	    {
		// Sort ignored items last
		if ( a->isIgnored() != b->isIgnored() ) return b->isIgnored();

		// The dot entry (there can only be one) should always come last
		if ( a->isDotEntry() ) return false;
		if ( b->isDotEntry() ) return true;
		return a->name() < b->name();
	    }

	case PercentBarCol:	  return a->subtreePercent()  < b->subtreePercent();
	case PercentNumCol:	  return a->subtreePercent()  < b->subtreePercent();
	case SizeCol:		  return a->totalSize()	      < b->totalSize();
	case TotalItemsCol:	  return a->totalItems()      < b->totalItems();
	case TotalFilesCol:	  return a->totalFiles()      < b->totalFiles();
	case TotalSubDirsCol:	  return a->totalSubDirs()    < b->totalSubDirs();
	case LatestMTimeCol:	  return a->latestMtime()     < b->latestMtime();
	case OldestFileMTimeCol:
            {
                time_t a_time = a->oldestFileMtime();
                time_t b_time = b->oldestFileMtime();

                if ( a_time == 0 ) return false;
                if ( b_time == 0 ) return true;

                return a_time < b_time;
            }

	case UserCol:		  return a->uid()	      < b->uid();
	case GroupCol:		  return a->gid()	      < b->gid();
	case PermissionsCol:	  return a->mode()	      < b->mode();
	case OctalPermissionsCol: return a->mode()	      < b->mode();
	case ReadJobsCol:	  return a->pendingReadJobs() < b->pendingReadJobs();
	case UndefinedCol:	  return false;
	    // Intentionally omitting the 'default' branch
	    // so the compiler can warn about unhandled enum values
    }

    return false;
}
