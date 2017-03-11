/*
 *   File name: FileSizeStatsWindow.cpp
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileSizeStatsWindow.h"
#include "FileSizeStats.h"
#include "DirTree.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"

#include <algorithm>


using namespace QDirStat;


FileSizeStatsWindow::FileSizeStatsWindow( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FileSizeStatsWindow ),
    _subtree( 0 ),
    _suffix( "" ),
    _stats( 0 )
{
    logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    initWidgets();
    readWindowSettings( this, "FileSizeStatsWindow" );

    _stats = new FileSizeStats();
    CHECK_NEW( _stats );
}


FileSizeStatsWindow::~FileSizeStatsWindow()
{
    logDebug() << "destroying" << endl;
    writeWindowSettings( this, "FileSizeStatsWindow" );
}


void FileSizeStatsWindow::clear()
{
    _stats->clear();
    _ui->content->clear();
}


void FileSizeStatsWindow::initWidgets()
{
    _ui->content->clear();
}


void FileSizeStatsWindow::calc()
{
    _stats->clear();

    if ( _suffix.isEmpty() )
        _stats->collect( _subtree );
    else
        _stats->collect( _subtree, _suffix );

    _stats->sort();
}


void FileSizeStatsWindow::populate( FileInfo * subtree, const QString & suffix )
{
    _ui->content->clear();
    _ui->content->setText( tr( "Calculating statistics for %1" ).arg( subtree->debugUrl() ) );

    _subtree = subtree;
    _suffix  = suffix;

    if ( ! _subtree )
    {
	logWarning() << "No tree" << endl;
	return;
    }

    calc();


    QString text;
    text = tr( "File size statistics for\n%1\n\n" ).arg( subtree->debugUrl() );
    text += tr( "Median:  %1\n" ).arg( formatSize( _stats->median() ) );
    text += tr( "Min:     %1\n" ).arg( formatSize( _stats->min() ) );
    text += tr( "Max:     %1\n" ).arg( formatSize( _stats->max() ) );
    text += tr( "Files:   %1\n" ).arg( _stats->data().size() );

    if ( _subtree->totalItems() > 0 )
    {
        FileSize average = _subtree->totalSize() / _subtree->totalItems();
        text += tr( "Average: %1\n" ).arg( formatSize( average ) );
    }

    _ui->content->setText( text );
}


void FileSizeStatsWindow::reject()
{
    deleteLater();
}

