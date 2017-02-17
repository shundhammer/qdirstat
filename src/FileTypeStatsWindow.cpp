/*
 *   File name: FileTypeStatsWindow.cpp
 *   Summary:   QDirStat file type statistics window
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:    Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "FileTypeStatsWindow.h"
#include "FileTypeStats.h"
#include "DirTree.h"
#include "MimeCategory.h"
#include "Logger.h"
#include "Exception.h"

#include <algorithm>


using namespace QDirStat;

// Number of suffixes in the "other" category
#define TOP_X   20

#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
#  define setSectionResizeMode setResizeMode
#endif


FileTypeStatsWindow::FileTypeStatsWindow( DirTree * tree,
                                          QWidget * parent ):
    QDialog( parent ),
    _ui( new Ui::FileTypeStatsWindow ),
    _tree( tree )
{
    logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    initWidgets();

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
    _ui->treeWidget->clear();
}


void FileTypeStatsWindow::initWidgets()
{
    _ui->treeWidget->setColumnCount( FT_ColumnCount );
    _ui->treeWidget->setHeaderLabels( QStringList()
                                      << tr( "Name" )
                                      << tr( "Number" )
                                      << tr( "Total Size" )
                                      << tr( "Percentage" ) );
    _ui->treeWidget->header()->setStretchLastSection( false );

    // Set auto-resize for all columns

    for ( int col = 0; col < FT_ColumnCount; ++col )
    {
        _ui->treeWidget->header()->setSectionResizeMode( col, QHeaderView::ResizeToContents );
    }
}


void FileTypeStatsWindow::calc()
{
    clear();
    _stats->calc();
    populate();
}


void FileTypeStatsWindow::populate()
{
    _ui->treeWidget->clear();

    if ( ! _tree || ! _tree->root() )
    {
        logWarning() << "No tree" << endl;
        return;
    }

    _ui->treeWidget->setSortingEnabled( false );

    //
    // Create toplevel items for the categories
    //

    QMap<MimeCategory *, FileTypeItem *> categoryItem;

    for ( CategoryFileSizeMapIterator it = _stats->categorySumBegin();
          it != _stats->categorySumEnd();
          ++it )
    {
        MimeCategory * category = it.key();

        if ( category && category )
        {
            QString  name       = category->name();
            FileSize sum        = it.value();
            int      count      = _stats->categoryCount( category );
            double   percentage = _stats->percentage( sum );

            FileTypeItem * item = new FileTypeItem( name, count, sum, percentage );
            CHECK_NEW( item );

            _ui->treeWidget->addTopLevelItem( item );
            item->setBold();
            categoryItem[ category ] = item;
        }
    }

    // Prepare to collect items for a category "other"

    QList<FileTypeItem *> otherItems;
    int      otherCount = 0;
    FileSize otherSum   = 0LL;


    //
    // Create items for each individual suffix (below a category)
    //

    for ( StringFileSizeMapIterator it = _stats->suffixSumBegin();
          it != _stats->suffixSumEnd();
          ++it )
    {
        QString  suffix     = it.key();
        FileSize sum        = it.value();
        int      count      = _stats->suffixCount( suffix );
        double   percentage = _stats->percentage( sum );

        MimeCategory * parentCategory = _stats->category( suffix );

        if ( suffix == NO_SUFFIX )
            suffix = tr( "<No extension>" );
        else
            suffix = "*." + suffix;

        FileTypeItem * item = new FileTypeItem( suffix, count, sum, percentage );
        CHECK_NEW( item );


        if ( parentCategory )
        {
            QTreeWidgetItem * parentItem = categoryItem.value( parentCategory, 0 );

            if ( parentItem )
                parentItem->addChild( item );
            else
            {
                logError() << "ERROR: No parent category item for " << suffix << endl;
                otherItems << item;
                otherCount += count;
                otherSum   += sum;
            }
        }
        else // No category for this suffix
        {
            otherItems << item;
            otherCount += count;
            otherSum   += sum;
        }
    }

    // Put remaining "other" items below a separate category

    if ( ! otherItems.isEmpty() )
    {
        FileTypeItemCompare cmp;
        std::sort( otherItems.begin(), otherItems.end(), cmp );
        QString name       = tr( "Other (Top %1)" ).arg( TOP_X );
        double  percentage = _stats->percentage( otherSum );

        FileTypeItem * otherCategoryItem =
            new FileTypeItem( name, otherCount, otherSum, percentage );
        CHECK_NEW( otherCategoryItem );

        _ui->treeWidget->addTopLevelItem( otherCategoryItem );
        otherCategoryItem->setBold();
        int top = qMin( otherItems.size(), TOP_X );

        for ( int i=0; i < top; ++i )
        {
            FileTypeItem * item = otherItems.takeFirst();
            otherCategoryItem->addChild( item );
        }

#if 1
        QStringList suffixes;

        foreach ( FileTypeItem * item, otherItems )
            suffixes << item->text(0);

        logDebug() << "Discarding " << otherItems.size()
                   << " suffixes below <other>: "
                   << suffixes.join( ", " )
                   << endl;
#endif
        qDeleteAll( otherItems );
    }

    _ui->treeWidget->setSortingEnabled( true );
    _ui->treeWidget->sortByColumn( FT_TotalSizeCol, Qt::DescendingOrder );
}


void FileTypeStatsWindow::reject()
{
    deleteLater();
}




FileTypeItem::FileTypeItem( const QString & name,
                            int             count,
                            FileSize        totalSize,
                            float           percentage ):
    QTreeWidgetItem( QTreeWidgetItem::UserType ),
    _name( name ),
    _count( count ),
    _totalSize( totalSize ),
    _percentage( percentage )
{
    QString percentStr;
    percentStr.sprintf( "%02.2f%%", percentage );

    setText( FT_NameCol,       name );
    setText( FT_CountCol,      QString( "%1" ).arg( count ) );
    setText( FT_TotalSizeCol,  formatSize( totalSize ) );
    setText( FT_PercentageCol, percentStr );

    setTextAlignment( FT_NameCol,       Qt::AlignLeft  );
    setTextAlignment( FT_CountCol,      Qt::AlignRight );
    setTextAlignment( FT_TotalSizeCol,  Qt::AlignRight );
    setTextAlignment( FT_PercentageCol, Qt::AlignRight );
}


bool FileTypeItem::operator<(const QTreeWidgetItem & rawOther) const
{
    // Since this is a reference, the dynamic_cast will throw a std::bad_cast
    // exception if it fails. Not catching this here since this is a genuine
    // error which should not be silently ignored.
    const FileTypeItem & other = dynamic_cast<const FileTypeItem &>( rawOther );

    int col = treeWidget() ? treeWidget()->sortColumn() : FT_TotalSizeCol;

    switch ( col )
    {
        case FT_NameCol:        return name()       < other.name();
        case FT_CountCol:       return count()      < other.count();
        case FT_TotalSizeCol:   return totalSize()  < other.totalSize();
        case FT_PercentageCol:  return percentage() < other.percentage();
        default:                return QTreeWidgetItem::operator<( rawOther );
    }
}


void FileTypeItem::setBold()
{
    QFont boldFont = font( 0 );
    boldFont.setBold( true );

    for ( int col=0; col < FT_ColumnCount; ++col )
        setFont( col, boldFont );
}
