/*
 *   File name: FileAgeStatsWindow.h
 *   Summary:	QDirStat "File Age Statistics" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <algorithm>    // std::sort()

#include "FileAgeStatsWindow.h"
#include "DirTree.h"
#include "FileInfoIterator.h"
#include "SettingsHelpers.h"
#include "HeaderTweaker.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


FileAgeStatsWindow::FileAgeStatsWindow( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FileAgeStatsWindow )
{
    // logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    initWidgets();
    readWindowSettings( this, "FileAgeStatsWindow" );

    connect( _ui->refreshButton, SIGNAL( clicked() ),
	     this,		 SLOT  ( refresh() ) );
}


FileAgeStatsWindow::~FileAgeStatsWindow()
{
    // logDebug() << "destroying" << endl;

    writeWindowSettings( this, "FileAgeStatsWindow" );
    delete _ui;
}


void FileAgeStatsWindow::clear()
{
    _ui->treeWidget->clear();
    _yearStats.clear();
    _totalFilesCount = 0;
    _totalFilesSize  = 0;
}


void FileAgeStatsWindow::refresh()
{
    populate( _subtree() );
}


void FileAgeStatsWindow::initWidgets()
{
    QFont font = _ui->heading->font();
    font.setBold( true );
    _ui->heading->setFont( font );

    // _ui->heading->setText( text );

    // _ui->treeWidget->setColumnCount( YearListColumnCount );

    QStringList headers;

    headers << tr( "Year"    )
            << tr( "Files"   )
            << tr( "Files %" )
            << tr( "Size"    )
            << tr( "Size %"  );


    _ui->treeWidget->setHeaderLabels( headers );
    _ui->treeWidget->header()->setStretchLastSection( false );

    // Center the column headers

    QTreeWidgetItem * hItem = _ui->treeWidget->headerItem();

    for ( int col = 0; col < headers.size(); ++col )
	hItem->setTextAlignment( col, Qt::AlignHCenter );

    HeaderTweaker::resizeToContents( _ui->treeWidget->header() );
}


void FileAgeStatsWindow::reject()
{
    deleteLater();
}


void FileAgeStatsWindow::populate( FileInfo * newSubtree )
{
    // logDebug() << "populating with " << newSubtree << endl;

    clear();
    _subtree = newSubtree;

    _ui->heading->setText( tr( "File Age Statistics for %1" )
                           .arg( _subtree.url() ) );

    // For better Performance: Disable sorting while inserting many items
    _ui->treeWidget->setSortingEnabled( false );

    collect( _subtree() );
    calcPercentages();
    populateListWidget();

    _ui->treeWidget->setSortingEnabled( true );
    _ui->treeWidget->sortByColumn( YearListYearCol, Qt::DescendingOrder );
}


void FileAgeStatsWindow::collect( FileInfo * dir )
{
    if ( ! dir )
	return;

    FileInfoIterator it( dir );

    while ( *it )
    {
	FileInfo * item = *it;

        if ( item && item->isFile() )
        {
            short year = item->mtimeYear();
            YearStats &stats = _yearStats[ year ];

            stats.year = year;
            stats.filesCount++;
            stats.size += item->size();
        }

	if ( item->hasChildren() )
	{
	    collect( item );
	}

        ++it;
    }
}


void FileAgeStatsWindow::calcPercentages()
{
    // Sum up the totals over all years

    _totalFilesCount = 0;
    _totalFilesSize  = 0;

    foreach ( const YearStats & stats, _yearStats )
    {
        _totalFilesCount += stats.filesCount;
        _totalFilesSize  += stats.size;
    }

    for ( YearStatsHash::iterator it = _yearStats.begin();
          it != _yearStats.end();
          ++it )
    {
        YearStats & stats = it.value();

        if ( _totalFilesCount > 0 )
            stats.filesPercent = ( 100.0 * stats.filesCount ) / _totalFilesCount;

        if ( _totalFilesSize > 0 )
            stats.sizePercent = ( 100.0 * stats.size ) / _totalFilesSize;
    }
}


void FileAgeStatsWindow::populateListWidget()
{
    QList<short> years = _yearStats.keys();
    std::sort( years.begin(), years.end() );

    foreach ( short year, years )
    {
        YearListItem * item = new YearListItem( _yearStats[ year ] );
        CHECK_NEW( item );

        _ui->treeWidget->addTopLevelItem( item );
    }
}






YearListItem::YearListItem( const YearStats & yearStats ) :
    QTreeWidgetItem( QTreeWidgetItem::UserType ),
    _stats( yearStats )
{
    setText( YearListYearCol,         QString::number( _stats.year         ) + " " );
    setText( YearListFilesCountCol,   QString::number( _stats.filesCount   ) + " " );
    setText( YearListFilesPercentCol, formatPercent  ( _stats.filesPercent ) + " " );
    setText( YearListSizeCol,         formatSize     ( _stats.size         ) + " " );
    setText( YearListSizePercentCol,  formatPercent  ( _stats.sizePercent  ) + " " );

    for ( int col = 0; col < YearListColumnCount; col++ )
        setTextAlignment( col, Qt::AlignRight );
}


bool YearListItem::operator<( const QTreeWidgetItem & rawOther ) const
{
    // Since this is a reference, the dynamic_cast will throw a std::bad_cast
    // exception if it fails. Not catching this here since this is a genuine
    // error which should not be silently ignored.
    const YearListItem & other = dynamic_cast<const YearListItem &>( rawOther );

    int col = treeWidget() ? treeWidget()->sortColumn() : YearListYearCol;

    switch ( col )
    {
	case YearListYearCol:		return _stats.year         < other.stats().year;
	case YearListFilesCountCol:	return _stats.filesCount   < other.stats().filesCount;
	case YearListFilesPercentCol:	return _stats.filesPercent < other.stats().filesPercent;
	case YearListSizeCol:           return _stats.size         < other.stats().size;
	case YearListSizePercentCol:	return _stats.sizePercent  < other.stats().sizePercent;
	default:		        return QTreeWidgetItem::operator<( rawOther );
    }
}

