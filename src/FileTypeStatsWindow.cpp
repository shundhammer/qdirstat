/*
 *   File name: FileTypeStatsWindow.cpp
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileTypeStatsWindow.h"
#include "FileTypeStats.h"
#include "MimeCategory.h"
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

    CHECK_NEW( _ui );
    _ui->setupUi( this );

    _stats = new FileTypeStats( tree, this );
    CHECK_NEW( _stats );

    populate();
}


FileTypeStatsWindow::~FileTypeStatsWindow()
{
    logDebug() << "destroying" << endl;
}


void FileTypeStatsWindow::clear()
{
    _stats->clear();
    _ui->textEdit->clear();
}


void FileTypeStatsWindow::calc()
{
    clear();
    _stats->calc();
    populate();
}


void FileTypeStatsWindow::populate()
{
    QString lines;
    QString line;

    for ( CategoryFileSizeMapIterator it = _stats->categorySumBegin();
	  it != _stats->categorySumEnd();
	  ++it )
    {
	MimeCategory * category = it.key();
	FileSize sum   = it.value();
	int	 count = _stats->categoryCount( category );

	line.sprintf( "%-26s  %6d  %10s\n",
		      qPrintable( category->name() + ":" ),
		      count,
		      qPrintable( formatSize( sum ) ) );

	lines += line;
    }

    lines += "\n";

    for ( StringFileSizeMapIterator it = _stats->suffixSumBegin();
	  it != _stats->suffixSumEnd();
	  ++it )
    {
	QString	 suffix = it.key();
	FileSize sum	= it.value();
	int	 count	= _stats->suffixCount( suffix );

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

