/*
 *   File name: FileTypeStats.cpp
 *   Summary:	Statistics classes for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileTypeStats.h"
#include "DirTree.h"
#include "FileInfoIterator.h"
#include "MimeCategorizer.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


FileTypeStats::FileTypeStats( QObject  * parent ):
    QObject( parent ),
    _totalSize( 0LL )
{
    _mimeCategorizer = MimeCategorizer::instance();
    CHECK_PTR( _mimeCategorizer );

    _otherCategory = new MimeCategory( tr( "Other" ) );
    CHECK_NEW( _otherCategory );
}


FileTypeStats::~FileTypeStats()
{
    clear();
    delete _otherCategory;
}


void FileTypeStats::clear()
{
    _suffixSum.clear();
    _suffixCount.clear();
    _categorySum.clear();
    _categoryCount.clear();
    _totalSize = 0LL;
}


int FileTypeStats::suffixCount( const QString & suffix ) const
{
    return _suffixCount.value( suffix, 0 );
}


FileSize FileTypeStats::suffixSum( const QString & suffix ) const
{
    return _suffixSum.value( suffix, 0LL );
}


int FileTypeStats::categoryCount( MimeCategory * category ) const
{
    return _categoryCount.value( category, 0 );
}


FileSize FileTypeStats::categorySum( MimeCategory * category ) const
{
    return _categorySum.value( category, 0LL );
}


MimeCategory * FileTypeStats::category( const QString & suffix ) const
{
    return _mimeCategorizer->category( "x." + suffix );
}


double FileTypeStats::percentage( FileSize size ) const
{
    FileSize total = totalSize();

    if ( total == 0LL )
	return 0.0;
    else
	return (100.0 * size) / (double) total;
}


void FileTypeStats::calc( FileInfo * subtree )
{
    clear();

    if ( subtree && subtree->checkMagicNumber() )
    {
        collect( subtree );
        _totalSize = subtree->totalSize();
        removeCruft();
        removeEmpty();
        sanityCheck();
    }

    emit calcFinished();
}


void FileTypeStats::collect( FileInfo * dir )
{
    if ( ! dir )
	return;

    FileInfoIterator it( dir );

    while ( *it )
    {
	FileInfo * item = *it;

	if ( item->hasChildren() )
	{
	    collect( item );
	}
	else if ( item->isFile() )
	{
	    QString suffix;

	    // First attempt: Try the MIME categorizer.
	    //
	    // If it knows the file's suffix, it can much easier find the
	    // correct one in case there are multiple to choose from, for
	    // example ".tar.bz2", not ".bz2" for a bzipped tarball. But on
	    // Linux systems, having multiple dots in filenames is very common,
	    // e.g. in .deb or .rpm packages, so the longest possible suffix is
	    // not always the useful one (because it might contain version
	    // numbers and all kinds of irrelevant information).
	    //
	    // The suffixes the MIME categorizer knows are carefully
	    // hand-crafted, so if it knows anything about a suffix, it's the
	    // best choice.

	    MimeCategory * category = _mimeCategorizer->category( item->name(), &suffix );

	    if ( ! category )
		category = _otherCategory;

	    _categorySum[ category ] += item->size();
	    ++_categoryCount[ category ];

	    if ( suffix.isEmpty() )
	    {
		if ( item->name().contains( '.' ) && ! item->name().startsWith( '.' ) )
		{
		    // Fall back to the last (i.e. the shortest) suffix if the
		    // MIME categorizer didn't know it: Use section -1 (the
		    // last one, ignoring any trailing '.' separator).
		    //
		    // The downside is that this would not find a ".tar.bz",
		    // but just the ".bz" for a compressed tarball. But it's
		    // much better than getting a ".eab7d88df-git.deb" rather
		    // than a ".deb".

		    suffix = item->name().section( '.', -1 );
		}
	    }

	    suffix = suffix.toLower();

	    if ( suffix.isEmpty() )
		suffix = NO_SUFFIX;

	    _suffixSum[ suffix ] += item->size();
	    ++_suffixCount[ suffix ];
	}
	// Disregard symlinks, block devices and other special files

	++it;
    }
}


void FileTypeStats::removeCruft()
{
    // Make sure those two already exist to avoid confusing the iterator
    // (QMap::operator[] auto-inserts with default ctor if not already there)

    _suffixSum	[ NO_SUFFIX ] += 0LL;
    _suffixCount[ NO_SUFFIX ] += 0;

    FileSize totalMergedSum   = 0LL;
    int	     totalMergedCount = 0;

    QStringList cruft;
    QMap<QString, int>::iterator it = _suffixCount.begin();

    while ( it != _suffixCount.end() )
    {
	QString suffix = it.key();
	bool merge = false;

	if ( isCruft( suffix ) )
	{
	    cruft << "*." + suffix;
	    merge = true;
	}

	if ( merge )
	{
	    _suffixSum	[ NO_SUFFIX ] += _suffixSum  [ suffix ];
	    _suffixCount[ NO_SUFFIX ] += _suffixCount[ suffix ];

	    totalMergedSum   += _suffixSum  [ suffix ];
	    totalMergedCount += _suffixCount[ suffix ];

	    it = _suffixCount.erase( it );
	    _suffixSum.remove( suffix );
	}
	else
	{
	    ++it;
	}
    }

#if 1
    logDebug() << "Merged " << cruft.size() << " suffixes to <NO SUFFIX>: "
	       << cruft.join( ", " ) << endl;
#endif
    logDebug() << "Merged: " << totalMergedCount << " files "
	       << "(" << formatSize( totalMergedSum ) << ")"
	       << endl;
}


void FileTypeStats::removeEmpty()
{
    QMap<QString, int>::iterator it = _suffixCount.begin();

    while ( it != _suffixCount.end() )
    {
	QString suffix = it.key();
	int	count  = it.value();
	bool	remove = count == 0;

	if ( remove )
	{
	    logDebug() << "Removing empty suffix *." << suffix << endl;
	    it = _suffixCount.erase( it );
	    _suffixSum.remove( suffix );
	}
	else
	{
	    ++it;
	}
    }
}


bool FileTypeStats::isCruft( const QString & suffix ) const
{
    if ( suffix == NO_SUFFIX )
	return false;

    // Whatever the MIME categorizer knows is good enough for us:
    // It is a preconfigured suffix for a well-known file type.

    if ( _mimeCategorizer->category( "x." + suffix ) )
	return false;

    int count	 = _suffixCount[ suffix ];
    int len	 = suffix.size();
    int letters	 = suffix.count( QRegExp( "[a-zA-Z]" ) );
    float lettersPercent = len > 0 ? (100.0 * letters) / len : 0.0;

    if ( letters == 0 )
	return true;

    if ( suffix.contains( ' ' ) )
	return true;

    // The most common case: 3-letter suffix
    if ( len == 3 && letters == 3 )
	return false;

    if ( len > 6 && count < len )
	return true;

    // Forget long suffixes with mostly non-letters
    if ( lettersPercent < 70.0 && count < len )
	return true;

    return false;
}


void FileTypeStats::sanityCheck()
{
    FileSize categoryTotal = 0LL;

    foreach ( FileSize sum, _categorySum )
	categoryTotal += sum;

    FileSize missing = totalSize() - categoryTotal;

    logDebug() << "Unaccounted in categories: " << formatSize( missing )
	       << " of " << formatSize( totalSize() )
	       << " (" << QString::number( percentage( missing ), 'f', 2 ) << "%)"
	       << endl;
}
