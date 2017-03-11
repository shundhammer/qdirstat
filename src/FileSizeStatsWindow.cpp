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

    QStringList text;

    text << tr( "File size statistics for %1" ).arg( subtree->debugUrl() );
    text << "";
    text << tr( "Median:  %1" ).arg( formatSize( _stats->median() ) );
    text << tr( "Average: %1\n" ).arg( formatSize( _stats->average() ) );
    text << tr( "Min:     %1" ).arg( formatSize( _stats->min() ) );
    text << tr( "Max:     %1" ).arg( formatSize( _stats->max() ) );
    text << "";
    text << tr( "Files:   %1" ).arg( _stats->data().size() );
    text << "";

    text << quantile( 4,   "quartile"   );
    text << quantile( 10,  "centile"    );
    text << quantile( 100, "percentile" );

    _ui->content->setText( text.join( "\n" ) );
}


QStringList FileSizeStatsWindow::quantile( int order, const QString & name )
{
    QStringList text;

    if ( _stats->data().size() < order )
        return text;

    for ( int i=1; i < order; ++i )
    {
        text << QString( "%1. %2: %3" ).arg( i )
            .arg( name )
            .arg( formatSize( _stats->quantile( order, i ) ) );
    }

    text << "";

    return text;
}


void FileSizeStatsWindow::reject()
{
    deleteLater();
}

