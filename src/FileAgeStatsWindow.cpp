/*
 *   File name: FileAgeStatsWindow.h
 *   Summary:	QDirStat "File Age Statistics" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileAgeStatsWindow.h"
#include "DirTree.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "HeaderTweaker.h"
#include "FormatUtil.h"
#include "Logger.h"
#include "Exception.h"

// Remember to adapt the tooltip text for the "Locate" button in the .ui file
// and the method docs in the .h file if this value is changed

#define MAX_LOCATE_FILES        1000

using namespace QDirStat;


FileAgeStatsWindow::FileAgeStatsWindow( QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FileAgeStatsWindow ),
    _stats( new FileAgeStats( 0 ) ),
    _filesPercentBarDelegate( 0 ),
    _sizePercentBarDelegate( 0 ),
    _startGapsWithCurrentYear( true )
{
    // logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    CHECK_NEW( _stats );

    _ui->setupUi( this );
    initWidgets();
    readSettings();
}


FileAgeStatsWindow::~FileAgeStatsWindow()
{
    writeSettings();

    delete _stats;
    delete _ui;
}


void FileAgeStatsWindow::clear()
{
    _stats->clear();
    _ui->treeWidget->clear();
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
            << tr( "Files %" )  // percent bar
            << tr( "%"       )  // percent value
            << tr( "Size"    )
            << tr( "Size %"  )  // percent bar
            << tr( "%"       ); // percent value


    _ui->treeWidget->setHeaderLabels( headers );
    _ui->treeWidget->header()->setStretchLastSection( false );


    // Delegates for the percent bars

    _filesPercentBarDelegate =
        new PercentBarDelegate( _ui->treeWidget, YearListFilesPercentBarCol );
    CHECK_NEW( _filesPercentBarDelegate );
    _filesPercentBarDelegate->setStartColorIndex( 7 );
    _ui->treeWidget->setItemDelegateForColumn( YearListFilesPercentBarCol, _filesPercentBarDelegate );

    _sizePercentBarDelegate =
        new PercentBarDelegate( _ui->treeWidget, YearListSizePercentBarCol );
    CHECK_NEW( _sizePercentBarDelegate );
    _sizePercentBarDelegate->setStartColorIndex( 1 );
    _ui->treeWidget->setItemDelegateForColumn( YearListSizePercentBarCol, _sizePercentBarDelegate );


    // Center the column headers

    QTreeWidgetItem * hItem = _ui->treeWidget->headerItem();

    for ( int col = 0; col < headers.size(); ++col )
	hItem->setTextAlignment( col, Qt::AlignHCenter );

    HeaderTweaker::resizeToContents( _ui->treeWidget->header() );


    // Signal/slot connections

    connect( _ui->refreshButton, SIGNAL( clicked() ),
	     this,		 SLOT  ( refresh() ) );

    connect( _ui->treeWidget,    SIGNAL( itemSelectionChanged() ),
             this,               SLOT  ( enableActions()        ) );

    connect( _ui->locateButton,  SIGNAL( clicked()     ),
             this,               SLOT  ( locateFiles() ) );
}


void FileAgeStatsWindow::reject()
{
    deleteLater();
}


void FileAgeStatsWindow::syncedPopulate( FileInfo * newSubtree )
{
    if ( _ui->syncCheckBox->isChecked() &&
         newSubtree && newSubtree->isDir() )
    {
        populate( newSubtree );
    }
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

    _stats->collect( _subtree() );
    populateListWidget();

    _ui->treeWidget->setSortingEnabled( true );
    _ui->treeWidget->sortByColumn( YearListYearCol, Qt::DescendingOrder );

    enableActions();
}


void FileAgeStatsWindow::populateListWidget()
{
    foreach ( short year, _stats->years() )
    {
        YearStats * yearStats = _stats->yearStats( year );

        if ( yearStats )
        {
            // Add a year item

            YearListItem * item = new YearListItem( *yearStats );
            CHECK_NEW( item );

            _ui->treeWidget->addTopLevelItem( item );


            // Add the month items if applicable

            if ( _stats->monthStatsAvailableFor( year ) )
            {
                for ( short month = 1; month <= 12; month++ )
                {
                    YearStats * monthStats = _stats->monthStats( year, month );

                    if ( monthStats )
                    {
                        YearListItem * monthItem = new YearListItem( *monthStats );
                        CHECK_NEW( monthItem );

                        if ( monthStats->filesCount == 0 )
                            monthItem->setFlags( Qt::NoItemFlags ); // disabled

                        item->addChild( monthItem );
                    }
                }
            }
        }
    }

    fillGaps();
}


void FileAgeStatsWindow::fillGaps()
{
    foreach ( short year, findGaps() )
    {
        YearListItem * item = new YearListItem( YearStats( year ) );
        CHECK_NEW( item );

        item->setFlags( Qt::NoItemFlags ); // disabled
        _ui->treeWidget->addTopLevelItem( item );
    }
}


YearsList FileAgeStatsWindow::findGaps()
{
    YearsList gaps;
    const YearsList & years = _stats->years(); // sorted in ascending order

    if ( years.isEmpty() )
        return gaps;

    short lastYear = _startGapsWithCurrentYear ? FileAgeStats::thisYear() : years.last();

    if ( lastYear - years.first() == years.count() - 1 )
        return gaps;

    for ( short yr = years.first(); yr <= lastYear; yr++ )
    {
        if ( ! years.contains( yr ) )
            gaps << yr;
    }

    return gaps;
}


YearListItem * FileAgeStatsWindow::selectedItem() const
{
    QTreeWidgetItem *currentItem = _ui->treeWidget->currentItem();

    return currentItem ? dynamic_cast<YearListItem *>( currentItem ) : 0;
}


void FileAgeStatsWindow::locateFiles()
{
    YearListItem * item = selectedItem();

    if ( item )
    {
        short month = item->stats().month;
        short year  = item->stats().year;

    if ( month > 0 && year > 0 )
        emit locateFilesFromMonth( _subtree.url(), year, month );
    else if ( year > 0 )
        emit locateFilesFromYear( _subtree.url(), year );
    }
}


void FileAgeStatsWindow::enableActions()
{
    bool locateEnabled = false;

    YearListItem * sel = selectedItem();

    if ( sel )
    {
        locateEnabled =
            sel->stats().filesCount > 0 &&
            sel->stats().filesCount <= MAX_LOCATE_FILES;
    }

    _ui->locateButton->setEnabled( locateEnabled );
}


void FileAgeStatsWindow::readSettings()
{
    Settings settings;

    settings.beginGroup( "FileAgeStatsWindow" );
    _ui->syncCheckBox->setChecked( settings.value( "SyncWithMainWindow", true ).toBool() );
    _startGapsWithCurrentYear = settings.value( "StartGapsWithCurrentYear", true ).toBool();
    settings.endGroup();

    readWindowSettings( this, "FileAgeStatsWindow" );
}


void FileAgeStatsWindow::writeSettings()
{
    Settings settings;

    settings.beginGroup( "FileAgeStatsWindow" );
    settings.setValue( "SyncWithMainWindow",       _ui->syncCheckBox->isChecked() );
    settings.setValue( "StartGapsWithCurrentYear", _startGapsWithCurrentYear      );
    settings.endGroup();

    writeWindowSettings( this, "FileAgeStatsWindow" );
}






YearListItem::YearListItem( const YearStats & yearStats ) :
    QTreeWidgetItem( QTreeWidgetItem::UserType ),
    _stats( yearStats )
{
    if ( _stats.month > 0 )
        setText( YearListYearCol,            monthName( yearStats.month ) );
    else
        setText( YearListYearCol,            QString::number( _stats.year         ) + " " );

    if ( _stats.filesCount > 0 )
    {
        QString pre( 4, ' ' );

        setText( YearListFilesCountCol,      QString::number( _stats.filesCount   ) + " " );
        setText( YearListFilesPercentBarCol, formatPercent  ( _stats.filesPercent ) + " " );
        setText( YearListFilesPercentCol,    formatPercent  ( _stats.filesPercent ) + " " );
        setText( YearListSizeCol,            pre + formatSize( _stats.size        ) + " " );
        setText( YearListSizePercentBarCol,  formatPercent  ( _stats.sizePercent  ) + " " );
        setText( YearListSizePercentCol,     formatPercent  ( _stats.sizePercent  ) + " " );
    }
}


QString YearListItem::monthName( short month ) const
{
    switch ( month )
    {
        case  1: return QObject::tr( "Jan." );
        case  2: return QObject::tr( "Feb." );
        case  3: return QObject::tr( "Mar." );
        case  4: return QObject::tr( "Apr." );
        case  5: return QObject::tr( "May"  );
        case  6: return QObject::tr( "Jun." );
        case  7: return QObject::tr( "Jul." );
        case  8: return QObject::tr( "Aug." );
        case  9: return QObject::tr( "Sep." );
        case 10: return QObject::tr( "Oct." );
        case 11: return QObject::tr( "Nov." );
        case 12: return QObject::tr( "Dec." );
    }

    return "";
}


QVariant YearListItem::data( int column, int role ) const
{
    if ( role == Qt::TextAlignmentRole )
    {
        // Vertical alignment can't be set in any easier way (?)

        int alignment = Qt::AlignVCenter;

        if ( column == YearListYearCol )
            alignment |= Qt::AlignLeft;
        else
            alignment |= Qt::AlignRight;

        return alignment;
    }

    return QTreeWidgetItem::data( column, role );
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
	case YearListYearCol:
            {
                if ( _stats.month > 0 )
                    return _stats.month < other.stats().month;
                else
                    return _stats.year  < other.stats().year;
            }

	case YearListFilesCountCol:	return _stats.filesCount   < other.stats().filesCount;
        case YearListFilesPercentBarCol:
	case YearListFilesPercentCol:	return _stats.filesPercent < other.stats().filesPercent;
	case YearListSizeCol:           return _stats.size         < other.stats().size;
        case YearListSizePercentBarCol:
	case YearListSizePercentCol:	return _stats.sizePercent  < other.stats().sizePercent;
	default:		        return QTreeWidgetItem::operator<( rawOther );
    }
}
