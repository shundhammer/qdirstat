/*
 *   File name: FileTypeStatsWindow.cpp
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileTypeStatsWindow.h"
#include "DirTree.h"
#include "MimeCategorizer.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;

#define NO_SUFFIX "///"	 // Illegal in filenames in Linux filesystems


FileTypeStatsWindow::FileTypeStatsWindow( DirTree * tree,
					  QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FileTypeStatsWindow ),
    _tree( tree )
{
    logDebug() << "init" << endl;
    _ui->setupUi( this );
    _mimeCategorizer = new MimeCategorizer( this );
    calc();
}


FileTypeStatsWindow::~FileTypeStatsWindow()
{
    logDebug() << "destroying" << endl;
    clear();
}


void FileTypeStatsWindow::clear()
{
    _suffixSum.clear();
    _suffixCount.clear();
    _categorySum.clear();
    _categoryCount.clear();

    _ui->textEdit->clear();
}


void FileTypeStatsWindow::calc()
{
    clear();

    if ( ! _tree )
    {
	logWarning() << "No tree" << endl;
	return;
    }

    logDebug() << "Calculating" << endl;
    collect( _tree->root() );
    removeCruft();
    logDebug() << "Collecting done" << endl;
    populate();
}


void FileTypeStatsWindow::collect( FileInfo * dir )
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
		if ( item->name().contains( '.' ) )
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


void FileTypeStatsWindow::removeCruft()
{
    QMap<QString, int>::iterator it = _suffixCount.begin();

    while ( it != _suffixCount.end() )
    {
	QString suffix = it.key();

	if ( isCruft( suffix ) )
	{
	    logDebug() << "Removing cruft *." << suffix << endl;
	    it = _suffixCount.erase( it );
	    _suffixSum.remove( suffix );
	}
	else
	{
	    ++it;
	}
    }
}


bool FileTypeStatsWindow::isCruft( const QString & suffix )
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
    float lettersPercent = letters > 0 ? (100.0 * len) / letters : 0.0;

    if ( letters == 0 )
	return true;

    if ( suffix.contains( ' ' ) )
	return true;

    // The most common case: 3-letter suffix
    if ( len == 3 && letters == 3 && count > 3 )
	return false;

    if ( count < 2 )
	return true;

    if ( _suffixSum[ suffix ] < 1024LL )
	return true;

    if ( len > 6 && count < len )
	return true;

    // Forget long suffixes with mostly non-letters
    if ( lettersPercent < 70.0 && count < len )
	return true;

    return false;
}


void FileTypeStatsWindow::populate()
{
    QString lines;
    QString line;

    for ( QMap<MimeCategory *, FileSize>::const_iterator it = _categorySum.constBegin();
	  it != _categorySum.constEnd();
	  ++it )
    {
	MimeCategory * category = it.key();
	FileSize sum   = it.value();
	int	 count = _categoryCount[ category ];

	line.sprintf( "%-26s  %6d  %10s\n",
		      qPrintable( category->name() + ":" ),
		      count,
		      qPrintable( formatSize( sum ) ) );

	lines += line;
    }

    lines += "\n";

    for ( QMap<QString, FileSize>::const_iterator it = _suffixSum.constBegin();
	  it != _suffixSum.constEnd();
	  ++it )
    {
	QString	 suffix = it.key();
	FileSize sum	= it.value();
	int	 count	= _suffixCount[ suffix ];

	if ( suffix == NO_SUFFIX )
	    suffix = "<No extension> :";
	else
	    suffix = "*." + suffix + " :";

	line.sprintf( "%-15s  %6d  %10s\n",
		      qPrintable( suffix ),
		      count,
		      qPrintable( formatSize( sum ) ) );

	lines += line;
    }

    _ui->textEdit->setText( lines );
}


void FileTypeStatsWindow::reject()
{
    deleteLater();
}

