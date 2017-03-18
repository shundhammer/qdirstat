/*
 *   File name: FileSizeStatsWindow.cpp
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <algorithm>
#include <QTableWidget>
#include <QTableWidgetItem>

#include "FileSizeStatsWindow.h"
#include "FileSizeStats.h"
#include "HistogramView.h"
#include "DirTree.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"


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
    QFont font = _ui->heading->font();
    font.setBold( true );
    _ui->heading->setFont( font );

    connect( _ui->percentileFilterComboBox, SIGNAL( currentIndexChanged( int ) ),
             this,                          SLOT  ( fillPercentileTable()      ) );

#if 1
    // DEBUG
    // DEBUG
    // DEBUG

    _ui->histogramView->setStartPercentile(  0 );
    _ui->histogramView->setEndPercentile  ( 95 );
    _ui->histogramView->setUseLogHeightScale( false );

    // DEBUG
    // DEBUG
    // DEBUG
#endif
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

    _subtree = subtree;
    _suffix  = suffix;

    if ( ! _subtree )
    {
	logWarning() << "No tree" << endl;
	return;
    }

    _ui->heading->setText( tr( "File size statistics for %1" ).arg( subtree->debugUrl() ) );
    calc();

    QStringList text;

    FileSize q1 = _stats->quartile( 1 );
    FileSize q3 = _stats->quartile( 3 );

    text << tr( "Median:   %1" ).arg( formatSize( _stats->median() ) );
    text << tr( "Average:  %1" ).arg( formatSize( _stats->average() ) );
    text << "";
    text << tr( "Q1:       %1" ).arg( formatSize( q1 ) );
    text << tr( "Q3:       %1" ).arg( formatSize( q3 ) );
    text << tr( "Q3 - Q1:  %1" ).arg( formatSize( q3 - q1 ) );
    text << "";
    text << tr( "Min:      %1" ).arg( formatSize( _stats->min() ) );
    text << tr( "Max:      %1" ).arg( formatSize( _stats->max() ) );
    text << "";
    text << tr( "Files:    %1" ).arg( _stats->dataSize() );

    fillPercentileTable();
    fillHistogram();

    _ui->content->setText( text.join( "\n" ) );
}


void FileSizeStatsWindow::fillPercentileTable()
{
    int step = _ui->percentileFilterComboBox->currentIndex() == 0 ? 5 : 1;
    fillQuantileTable( _ui->percentileTable, 100, "P", step, 2 );
}


QStringList FileSizeStatsWindow::quantile( int order, const QString & name )
{
    QStringList text;

    if ( _stats->dataSize() < 2 * order )
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


void FileSizeStatsWindow::fillQuantileTable( QTableWidget *  table,
                                             int             order,
                                             const QString & namePrefix,
                                             int             step,
                                             int             extremesMargin )
{
    table->clear();
    table->setColumnCount( 3 );
    table->setRowCount( order + 1 );
    int row = 0;

    int median     = order / 2;
    int quartile_1 = -1;
    int quartile_3 = -1;

    if ( order % 4 == 0 )
    {
        quartile_1 = order / 4;
        quartile_3 = quartile_1 * 3;
    }


    for ( int i=0; i <= order; ++i )
    {
        if ( step > 1 &&
             i > extremesMargin && i < order - extremesMargin &&
             i % step != 0 )
        {
            continue;
        }


        QTableWidgetItem * numberItem = new QTableWidgetItem( namePrefix + QString::number( i ) );
        QTableWidgetItem * valueItem  = new QTableWidgetItem( formatSize( _stats->quantile( order, i ) ) );

        CHECK_NEW( numberItem );
        CHECK_NEW( valueItem );

        numberItem->setTextAlignment( Qt::AlignRight | Qt::AlignVCenter );
        valueItem->setTextAlignment ( Qt::AlignRight | Qt::AlignVCenter );

        table->setItem( row, 0, numberItem );
        table->setItem( row, 1, valueItem  );

        if ( i == 0 || i == median || i == order || i == quartile_1 || i == quartile_3 )
        {
            QString text;

            if      ( i == 0 )          text = "Min";
            else if ( i == order  )     text = "Max";
            else if ( i == median )     text = "Median";
            else if ( i == quartile_1 ) text = "1. Quartile";
            else if ( i == quartile_3 ) text = "3. Quartile";

            QTableWidgetItem * nameItem = new QTableWidgetItem( text );
            CHECK_NEW( nameItem );

            nameItem->setTextAlignment( Qt::AlignCenter | Qt::AlignVCenter );
            table->setItem( row, 2, nameItem );

            QFont font = nameItem->font();
            font.setBold( true );

            numberItem->setFont( font );
            valueItem->setFont( font );
            nameItem->setFont( font );

            QBrush brush = nameItem->foreground();
            brush.setColor( Qt::blue );

            numberItem->setForeground( brush );
            valueItem->setForeground( brush );
            nameItem->setForeground( brush );
        }
        else if ( order > 20 && i % 10 == 0 && step <= 1 )
        {
            QTableWidgetItem * emptyItem = new QTableWidgetItem( "" );
            CHECK_NEW( emptyItem );

            table->setItem( i, 2, emptyItem );

            QBrush brush( QColor( 0xE0, 0xE0, 0xF0 ), Qt::SolidPattern );
            numberItem->setBackground( brush );
            valueItem->setBackground( brush );
            emptyItem->setBackground( brush );
        }

        ++row;
    }

    table->setRowCount( row );
}


void FileSizeStatsWindow::fillHistogram()
{
    HistogramView * histogram = _ui->histogramView;
    CHECK_PTR( histogram );

    int startPercentile = histogram->startPercentile();
    int endPercentile   = histogram->endPercentile();
    int percentileCount = endPercentile - startPercentile;
    int dataCount       = _stats->dataSize() * ( percentileCount / 100.0 );
    int bucketCount     = histogram->bestBucketCount( dataCount );
    QRealList buckets   = _stats->fillBuckets( bucketCount, startPercentile, endPercentile );

    histogram->setBuckets( buckets );
    histogram->setPercentiles( _stats->percentileList() );
    histogram->redisplay();
}


void FileSizeStatsWindow::reject()
{
    deleteLater();
}

