/*
 *   File name: FileAgeStatsWindow.h
 *   Summary:	QDirStat "File Age Statistics" window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


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
    logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    initWidgets();
    readWindowSettings( this, "FileAgeStatsWindow" );

    connect( _ui->refreshButton, SIGNAL( clicked() ),
	     this,		 SLOT  ( refresh() ) );
}


FileAgeStatsWindow::~FileAgeStatsWindow()
{
    logDebug() << "destroying" << endl;

    writeWindowSettings( this, "FileAgeStatsWindow" );
    delete _ui;
}


void FileAgeStatsWindow::clear()
{
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

    _ui->treeWidget->setColumnCount( YearListColumnCount );
    _ui->treeWidget->setHeaderLabels( QStringList()
				      << tr( "Year" )
                                      << tr( "Files" )
                                      << tr( "Files %" )
                                      << tr( "Size" )
                                      << tr( "Size %" ) );
    _ui->treeWidget->header()->setStretchLastSection( false );
    HeaderTweaker::resizeToContents( _ui->treeWidget->header() );
}


void FileAgeStatsWindow::reject()
{
    deleteLater();
}


void FileAgeStatsWindow::populate( FileInfo * newSubtree )
{
    logDebug() << "populating with " << newSubtree << endl;

    clear();

    // For better Performance: Disable sorting while inserting many items
    _ui->treeWidget->setSortingEnabled( false );


    _ui->treeWidget->setSortingEnabled( true );
    _ui->treeWidget->sortByColumn( YearListYearCol, Qt::DescendingOrder );
}






YearListItem::YearListItem( const YearStats & yearStats ) :
    QTreeWidgetItem( QTreeWidgetItem::UserType ),
    _stats( yearStats )
{
    setText( YearListYearCol,         QString::number( _stats.year         ) );
    setText( YearListFilesCountCol,   QString::number( _stats.filesCount   ) );
    setText( YearListFilesPercentCol, formatPercent  ( _stats.filesPercent ) );
    setText( YearListSizeCol,         formatSize     ( _stats.size         ) );
    setText( YearListSizePercentCol,  formatPercent  ( _stats.sizePercent  ) );

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

