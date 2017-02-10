/*
 *   File name: FileTypeStatsWindow.cpp
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileTypeStatsWindow.h"
#include "DirTree.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;

#define NO_SUFFIX "///"  // Illegal in filenames in Linux filesystems


FileTypeStatsWindow::FileTypeStatsWindow( DirTree * tree,
                                          QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FileTypeStatsWindow ),
    _tree( tree )
{
    logDebug() << "init" << endl;
    _ui->setupUi( this );
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
            // Find the filename suffix:
            // Section #1 (ignoring any leading '.' separator)
            //
            // This takes only the longest extension into account, i.e.
            // ".tar.bz2", not ".bz2" for a bzipped tarball.
            QString suffix = item->name().section( '.', 1 );
            suffix = suffix.toLower();

            if ( suffix.isEmpty() )
                suffix = NO_SUFFIX;

            _suffixSum[ suffix ] += item->size();
            ++_suffixCount[ suffix ];
        }
        // Disregarding symlinks, block devices etc.

        item = item->next();
    }
}


void FileTypeStatsWindow::populate()
{
    QString content;
    QTextStream str( &content );

    for ( QMap<QString, FileSize>::const_iterator it = _suffixSum.constBegin();
          it != _suffixSum.constEnd();
          ++it )
    {
        QString  suffix = it.key();
        FileSize sum    = it.value();
        int      count  = _suffixCount[ suffix ];

        str << "*." << suffix << ": " << count << "\t" << formatSize( sum ) << endl;
    }

    _ui->textEdit->setText( content );
}


void FileTypeStatsWindow::reject()
{
    deleteLater();
}

