/*
 *   File name: FileSizeStatsWindow.cpp
 *   Summary:	QDirStat size type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <algorithm>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QCommandLinkButton>
#include <QProcess>

#include "FileSizeStatsWindow.h"
#include "FileSizeStats.h"
#include "HistogramView.h"
#include "BucketsTableModel.h"
#include "DirTree.h"
#include "MainWindow.h"
#include "SettingsHelpers.h"
#include "HeaderTweaker.h"
#include "QDirStatApp.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


QPointer<FileSizeStatsWindow> FileSizeStatsWindow::_sharedInstance = 0;


FileSizeStatsWindow::FileSizeStatsWindow( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FileSizeStatsWindow ),
    _subtree( 0 ),
    _suffix( "" ),
    _stats( 0 )
{
    // logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    initWidgets();
    readWindowSettings( this, "FileSizeStatsWindow" );

    _stats = new FileSizeStats();
    CHECK_NEW( _stats );

    _bucketsTableModel = new BucketsTableModel( this, _ui->histogramView );
    CHECK_NEW( _bucketsTableModel );

    _ui->bucketsTable->setModel( _bucketsTableModel );

    QList<QCommandLinkButton *> helpButtons = _ui->helpPage->findChildren<QCommandLinkButton *>();

    foreach ( QCommandLinkButton * helpButton, helpButtons )
    {
	connect( helpButton, SIGNAL( clicked()	),
		 this,	     SLOT  ( showHelp() ) );
    }
}


FileSizeStatsWindow::~FileSizeStatsWindow()
{
    // logDebug() << "destroying" << endl;
    writeWindowSettings( this, "FileSizeStatsWindow" );
    delete _stats;
    delete _ui;
}


FileSizeStatsWindow * FileSizeStatsWindow::sharedInstance()
{
    if ( ! _sharedInstance )
    {
	_sharedInstance = new FileSizeStatsWindow( app()->findMainWindow() );
	CHECK_NEW( _sharedInstance );
    }

    return _sharedInstance;
}


void FileSizeStatsWindow::clear()
{
    _stats->clear();
}


void FileSizeStatsWindow::initWidgets()
{
    QFont font = _ui->heading->font();
    font.setBold( true );
    _ui->heading->setFont( font );
    _ui->optionsPanel->hide();

    connect( _ui->percentileFilterComboBox, SIGNAL( currentIndexChanged( int ) ),
	     this,			    SLOT  ( fillPercentileTable()      ) );

    connect( _ui->optionsButton, SIGNAL( clicked()	 ),
	     this,		 SLOT  ( toggleOptions() ) );

    connect( _ui->autoButton,	 SIGNAL( clicked()  ),
	     this,		 SLOT  ( autoPercentiles() ) );

    connect( _ui->startPercentileSlider,  SIGNAL( valueChanged( int ) ),
	     this,			  SLOT	( applyOptions()      ) );

    connect( _ui->startPercentileSpinBox, SIGNAL( valueChanged( int ) ),
	     this,			  SLOT	( applyOptions()      ) );

    connect( _ui->endPercentileSlider,	  SIGNAL( valueChanged( int ) ),
	     this,			  SLOT	( applyOptions()      ) );

    connect( _ui->endPercentileSpinBox,	  SIGNAL( valueChanged( int ) ),
	     this,			  SLOT	( applyOptions()      ) );
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


void FileSizeStatsWindow::populateSharedInstance( FileInfo *	  subtree,
						  const QString & suffix  )
{
    if ( ! subtree )
        return;

    sharedInstance()->populate( subtree, suffix );
    sharedInstance()->show();
}


void FileSizeStatsWindow::populate( FileInfo * subtree, const QString & suffix )
{
    _subtree = subtree;
    _suffix  = suffix;

    if ( _suffix.startsWith( "*." ) )
	_suffix.remove( 0, 1 );

    if ( ! _subtree )
    {
	logWarning() << "No tree" << endl;
	return;
    }

    QString url = subtree->debugUrl();

    if ( url == "<root>" )
	url = subtree->tree()->url();

    if ( _suffix.isEmpty() )
	_ui->heading->setText( tr( "File Size Statistics for %1" ).arg( url ) );
    else
	_ui->heading->setText( tr( "File Size Statistics for %1 in %2" )
			       .arg( suffix ).arg( url ) );
    calc();

    fillHistogram();
    fillPercentileTable();
}


void FileSizeStatsWindow::fillPercentileTable()
{
    int step = _ui->percentileFilterComboBox->currentIndex() == 0 ? 5 : 1;
    fillQuantileTable( _ui->percentileTable, 100, "P",
		       _stats->percentileSums(),
		       step, 2 );
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


void FileSizeStatsWindow::fillQuantileTable( QTableWidget *    table,
					     int	       order,
					     const QString &   namePrefix,
					     const QRealList & sums,
					     int	       step,
					     int	       extremesMargin )
{
    enum TableColumns
    {
	NumberCol,
	ValueCol,
	NameCol,
	SumCol
    };

    table->clear();
    table->setColumnCount( sums.isEmpty() ? 3 : 4 );
    table->setRowCount( order + 1 );

    QStringList header;

    switch ( order )
    {
	case 100:	header << tr( "Percentile" ); break;
	case  10:	header << tr( "Decile"	   ); break;
	case   4:	header << tr( "Quartile"   ); break;
	default:	header << tr( "%1-Quantile" ).arg( order ); break;
    }

    header << tr( "Value" ) << tr( "Name" );

    if ( ! sums.isEmpty() )
	header << tr( "Sum %1(n-1)..%2(n)" ).arg( namePrefix ).arg( namePrefix );

    for ( int col = 0; col < header.size(); ++col )
    {
	QString text = " " + header[ col ] + " ";
	table->setHorizontalHeaderItem( col, new QTableWidgetItem( text ) );
    }

    int median	   = order / 2;
    int quartile_1 = -1;
    int quartile_3 = -1;

    if ( order % 4 == 0 )
    {
	quartile_1 = order / 4;
	quartile_3 = quartile_1 * 3;
    }

    int row = 0;

    for ( int i=0; i <= order; ++i )
    {
	if ( step > 1 &&
	     i > extremesMargin && i < order - extremesMargin &&
	     i % step != 0 )
	{
	    continue;
	}

	addItem( table, row, NumberCol, namePrefix + QString::number( i ) );
	addItem( table, row, ValueCol, formatSize( _stats->quantile( order, i ) ) );

	if ( i > 0 && i < sums.size() )
	    addItem( table, row, SumCol, formatSize( sums.at( i ) ) );

	if ( i == 0 || i == median || i == order || i == quartile_1 || i == quartile_3 )
	{
	    QString text;

	    if	    ( i == 0 )		text = tr( "Min" );
	    else if ( i == order  )	text = tr( "Max" );
	    else if ( i == median )	text = tr( "Median" );
	    else if ( i == quartile_1 ) text = tr( "1. Quartile" );
	    else if ( i == quartile_3 ) text = tr( "3. Quartile" );

	    addItem( table, row, NameCol, text );
	    setRowBold( table, row );
	    setRowForeground( table, row, QBrush( QColor( Qt::blue ) ) );
	}
	else if ( order > 20 && i % 10 == 0 && step <= 1 )
	{
	    addItem( table, row, NameCol, "" ); // Fill the empty cell
	    setRowBackground( table, row, QBrush( QColor( 0xE0, 0xE0, 0xF0 ), Qt::SolidPattern ) );
	}

	++row;
    }

    table->setRowCount( row );

    setColAlignment( table, NumberCol, Qt::AlignRight  | Qt::AlignVCenter );
    setColAlignment( table, ValueCol,  Qt::AlignRight  | Qt::AlignVCenter );
    setColAlignment( table, NameCol,   Qt::AlignCenter | Qt::AlignVCenter );
    setColAlignment( table, SumCol,    Qt::AlignRight  | Qt::AlignVCenter );

    HeaderTweaker::resizeToContents( table->horizontalHeader() );
}


QTableWidgetItem *
FileSizeStatsWindow::addItem( QTableWidget *  table,
			      int	      row,
			      int	      col,
			      const QString & text )
{
    QTableWidgetItem * item = new QTableWidgetItem( text );
    CHECK_NEW( item );
    table->setItem( row, col, item );

    return item;
}


void FileSizeStatsWindow::setRowBold( QTableWidget * table, int row )
{
    for ( int col=0; col < table->columnCount(); ++col )
    {
	QTableWidgetItem * item = table->item( row, col );

	if ( item )
	{
	    QFont font = item->font();
	    font.setBold( true );
	    item->setFont( font );
	}
    }
}


void FileSizeStatsWindow::setRowForeground( QTableWidget * table, int row, const QBrush & brush )
{
    for ( int col=0; col < table->columnCount(); ++col )
    {
	QTableWidgetItem * item = table->item( row, col );

	if ( item )
	    item->setForeground( brush );
    }
}


void FileSizeStatsWindow::setRowBackground( QTableWidget * table, int row, const QBrush & brush )
{
    for ( int col=0; col < table->columnCount(); ++col )
    {
	QTableWidgetItem * item = table->item( row, col );

	if ( item )
	    item->setBackground( brush );
    }
}


void FileSizeStatsWindow::setColAlignment( QTableWidget * table, int col, int alignment )
{
    for ( int row=0; row < table->rowCount(); ++row )
    {
	QTableWidgetItem * item = table->item( row, col );

	if ( item )
	    item->setTextAlignment( alignment );
    }
}


void FileSizeStatsWindow::fillHistogram()
{
    HistogramView * histogram = _ui->histogramView;
    CHECK_PTR( histogram );

    histogram->clear();
    histogram->setPercentiles( _stats->percentileList() );
    histogram->setPercentileSums( _stats->percentileSums() );
    histogram->autoStartEndPercentiles();
    updateOptions();
    fillBuckets();
    histogram->autoLogHeightScale();
    histogram->rebuild();
}


void FileSizeStatsWindow::fillBuckets()
{
    HistogramView * histogram = _ui->histogramView;

    int startPercentile = histogram->startPercentile();
    int endPercentile	= histogram->endPercentile();

    int percentileCount = endPercentile - startPercentile;
    int dataCount	= _stats->dataSize() * ( percentileCount / 100.0 );
    int bucketCount	= histogram->bestBucketCount( dataCount );
    QRealList buckets	= _stats->fillBuckets( bucketCount, startPercentile, endPercentile );

    histogram->setBuckets( buckets );
    fillBucketsTable();
}


void FileSizeStatsWindow::fillBucketsTable()
{
    _bucketsTableModel->reset();
    HeaderTweaker::resizeToContents( _ui->bucketsTable->horizontalHeader() );
}


void FileSizeStatsWindow::reject()
{
    deleteLater();
}


void FileSizeStatsWindow::toggleOptions()
{
    if ( _ui->optionsPanel->isVisible() )
    {
	_ui->optionsPanel->hide();
	_ui->optionsButton->setText( tr( "&Options >>" ) );
    }
    else
    {
	_ui->optionsPanel->show();
	_ui->optionsButton->setText( tr( "<< &Options" ) );
	updateOptions();
    }
}


void FileSizeStatsWindow::applyOptions()
{
    HistogramView * histogram = _ui->histogramView;

    int newStart = _ui->startPercentileSlider->value();
    int newEnd	 = _ui->endPercentileSlider->value();

    if ( newStart != histogram->startPercentile() ||
	 newEnd	  != histogram->endPercentile()	    )
    {
	logDebug() << "New start: " << newStart << " new end: " << newEnd << endl;

	histogram->setStartPercentile( newStart );
	histogram->setEndPercentile  ( newEnd	);
	fillBuckets();
	histogram->autoLogHeightScale(); // FIXME
	histogram->rebuild();
    }
}


void FileSizeStatsWindow::autoPercentiles()
{
    _ui->histogramView->autoStartEndPercentiles();

    updateOptions();
    fillBuckets();
    _ui->histogramView->autoLogHeightScale(); // FIXME
    _ui->histogramView->rebuild();
}


void FileSizeStatsWindow::updateOptions()
{
    HistogramView * histogram = _ui->histogramView;

    _ui->startPercentileSlider->setValue ( histogram->startPercentile() );
    _ui->startPercentileSpinBox->setValue( histogram->startPercentile() );

    _ui->endPercentileSlider->setValue ( histogram->endPercentile() );
    _ui->endPercentileSpinBox->setValue( histogram->endPercentile() );
}


void FileSizeStatsWindow::showHelp()
{
    QString topic = "Statistics.md";
    QObject * button = sender();

    if	    ( button == _ui->medianPercentilesHelpButton   )  topic = "Median-Percentiles.md";
    else if ( button == _ui->histogramsInGeneralHelpButton )  topic = "Histograms-in-General.md";
    else if ( button == _ui->fileSizeHistogramHelpButton   )  topic = "File-Size-Histogram.md";
    else if ( button == _ui->overflowAreaHelpButton	   )  topic = "Overflow-Area.md";
    else if ( button == _ui->histogramOptionsHelpButton	   )  topic = "Histogram-Options.md";
    else if ( button == _ui->percentilesTableHelpButton	   )  topic = "Percentiles-Table.md";
    else if ( button == _ui->bucketsTableHelpButton	   )  topic = "Buckets-Table.md";

    logInfo() << "Help topic: " << topic << endl;
    QString helpUrl = "https://github.com/shundhammer/qdirstat/blob/master/doc/stats/" + topic;
    QString program = "/usr/bin/xdg-open";

    logInfo() << "Starting  " << program << " " << helpUrl << endl;
    QProcess::startDetached( program, QStringList() << helpUrl );
}
