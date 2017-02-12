/*
 *   File name: FileTypeStats.cpp
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileTypeStats.h"
#include "DirTree.h"
#include "MimeCategorizer.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


FileTypeStats::FileTypeStats( DirTree * tree,
			      QObject * parent ):
    QObject( parent ),
    _tree( tree )
{
    logDebug() << "init" << endl;
    _mimeCategorizer = new MimeCategorizer( this );
    calc();
}


FileTypeStats::~FileTypeStats()
{
    logDebug() << "destroying" << endl;
    clear();
}


void FileTypeStats::clear()
{
    _relevanceThreshold = 50*1024LL;
    _suffixSum.clear();
    _suffixCount.clear();
    _categorySum.clear();
    _categoryCount.clear();
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


void FileTypeStats::calc()
{
    clear();

    if ( ! _tree || ! _tree->root() )
    {
	logWarning() << "No tree" << endl;
	return;
    }

    collect( _tree->root() );
    // _relevanceThreshold = qMax( _tree->root()->totalSize() / 10000LL, 1024LL );
    logDebug() << "relevance threshold: " << formatSize( _relevanceThreshold ) << endl;
    removeCruft();

    emit calcFinished();
}


void FileTypeStats::collect( FileInfo * dir )
{
    if ( ! dir )
	return;

    FileInfo * item = dir->firstChild();

    while ( item )
    {
	if ( item->isDirInfo() )
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

	    if ( category )
	    {
		_categorySum[ category ] += item->size();
		++_categoryCount[ category ];
	    }
	    else if ( suffix.isEmpty() )
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

	item = item->next();
    }
}


void FileTypeStats::removeCruft()
{
    {
	QMap<QString, int>::iterator it = _suffixCount.begin();

	while ( it != _suffixCount.end() )
	{
	    QString suffix = it.key();
	    bool remove = false;

	    if ( isCruft( suffix ) )
	    {
		// logVerbose() << "Removing cruft *." << suffix << endl;
		remove = true;
	    }
	    else if ( isIrrelevant( suffix ) )
	    {
		// logVerbose() << "Removing irrelevant *." << suffix << endl;
		remove = true;
	    }

	    if ( remove )
	    {
		it = _suffixCount.erase( it );
		_suffixSum.remove( suffix );
	    }
	    else
	    {
		++it;
	    }
	}
    }

    QMap<MimeCategory *, FileSize>::iterator it = _categorySum.begin();

    while ( it != _categorySum.end() )
    {
	MimeCategory * category = it.key();
	FileSize sum = it.value();
	bool remove = false;

	if ( sum < _relevanceThreshold )
	{
	    logDebug() << "Removing irrelevant " << category << endl;
	    remove = true;
	}

	if ( remove )
	{
	    it = _categorySum.erase( it );
	    _categoryCount.remove( category );
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


bool FileTypeStats::isIrrelevant( const QString & suffix ) const
{
    if ( _suffixSum[ suffix ] < _relevanceThreshold )
	return true;

    return false;
}

