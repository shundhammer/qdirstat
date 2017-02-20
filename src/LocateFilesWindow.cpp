/*
 *   File name: LocateFilesWindow.cpp
 *   Summary:   QDirStat file type statistics window
 *   License:   GPL V2 - See file LICENSE for details.
 *
 *   Author:    Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "LocateFilesWindow.h"
#include "DirTree.h"
#include "SelectionModel.h"
#include "FileInfoIterator.h"
#include "Logger.h"
#include "Exception.h"

#include <algorithm>


using namespace QDirStat;


#if (QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 ))
#  define setSectionResizeMode setResizeMode
#endif


LocateFilesWindow::LocateFilesWindow( DirTree *        tree,
                                      SelectionModel * selectionModel,
                                      QWidget *        parent ):
    QDialog( parent ),
    _ui( new Ui::LocateFilesWindow ),
    _tree( tree ),
    _selectionModel( selectionModel )
{
    logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    initWidgets();

    connect( _ui->treeWidget,   SIGNAL( currentItemChanged( QTreeWidgetItem *,
                                                            QTreeWidgetItem * ) ),
             this,              SLOT  ( selectResult      ( QTreeWidgetItem * ) ) );
}


LocateFilesWindow::~LocateFilesWindow()
{
    logDebug() << "destroying" << endl;
}


void LocateFilesWindow::clear()
{
    _searchSuffix.clear();
    _ui->treeWidget->clear();
}


void LocateFilesWindow::initWidgets()
{
    _ui->treeWidget->setColumnCount( SSR_ColumnCount );
    _ui->treeWidget->setHeaderLabels( QStringList()
                                      << tr( "Number" )
                                      << tr( "Total Size" )
                                      << tr( "Directory" ) );
    _ui->treeWidget->header()->setStretchLastSection( false );

    // Set auto-resize for all columns

    for ( int col = 0; col < SSR_ColumnCount; ++col )
    {
        _ui->treeWidget->header()->setSectionResizeMode( col, QHeaderView::ResizeToContents );
    }
}


void LocateFilesWindow::reject()
{
    deleteLater();
}


QString LocateFilesWindow::searchSuffix() const
{
    return QString( "*" ) + _searchSuffix;
}


void LocateFilesWindow::locate( const QString & suffix )
{
    clear();
    _searchSuffix = suffix;

    if ( _searchSuffix.startsWith( '*' ) )
        _searchSuffix.remove( 0, 1 ); // Remove the leading '*'

    if ( ! _searchSuffix.startsWith( '.' ) )
        _searchSuffix.prepend( '.' );

    logDebug() << "Locating all files ending with \""
               << _searchSuffix << "\"" << endl;

    locate( _tree->root() );
}


void LocateFilesWindow::locate( FileInfo * dir )
{
    if ( ! dir )
        return;

    FileInfoSet matches = matchingFiles( dir );

    if ( ! matches.isEmpty() )
    {
        // Create a search result for this path

        int totalSize = 0LL;

        foreach ( FileInfo * file, matches )
            totalSize += file->size();

        SuffixSearchResultItem * searchResultItem =
            new SuffixSearchResultItem( dir->url(), matches.size(), totalSize );
        CHECK_NEW( searchResultItem );

        _ui->treeWidget->addTopLevelItem( searchResultItem );
    }

    
    // Recurse through any subdirectories
    
    FileInfo* child = dir->firstChild();

    while ( child )
    {
        if ( child->isDir() )
            locate( child );
        
        child = child->next();
    }

    // Notice that unlike in FileTypeStats, there is no need to recurse through
    // any dot entries: They are handled in matchingFiles() already.
}


FileInfoSet LocateFilesWindow::matchingFiles( FileInfo * item )
{
    FileInfoSet result;
    
    if ( ! item || ! item->isDirInfo() )
        return result;

    DirInfo * dir = item->toDirInfo();

    if ( dir->dotEntry() )
        dir = dir->dotEntry();

    FileInfo * child = dir->firstChild();

    while ( child )
    {
        if ( child->isFile() && child->name().endsWith( _searchSuffix ) )
            result << child;
        
        child = child->next();
    }

    return result;
}


void LocateFilesWindow::selectResult( QTreeWidgetItem * item )
{
    SuffixSearchResultItem * searchResult =
        dynamic_cast<SuffixSearchResultItem *>( item );

    CHECK_DYNAMIC_CAST( searchResult, "SuffixSearchResultItem" );
    
    FileInfo * dir = _tree->locate( searchResult->path() );
    FileInfoSet matches = matchingFiles( dir );
    _selectionModel->setSelectedItems( matchingFiles( dir ) );

    if ( ! matches.isEmpty() )
        _selectionModel->setCurrentBranch( matches.first() );
}






SuffixSearchResultItem::SuffixSearchResultItem( const QString & path,
                                                int             count,
                                                FileSize        totalSize ):
    QTreeWidgetItem( QTreeWidgetItem::UserType ),
    _path( path ),
    _count( count ),
    _totalSize( totalSize )
{
    setText( SSR_CountCol,      QString( "%1" ).arg( count ) );
    setText( SSR_TotalSizeCol,  formatSize( totalSize ) );
    setText( SSR_PathCol,       path );

    setTextAlignment( SSR_CountCol,      Qt::AlignRight );
    setTextAlignment( SSR_TotalSizeCol,  Qt::AlignRight );
    setTextAlignment( SSR_PathCol,       Qt::AlignLeft  );
}


bool SuffixSearchResultItem::operator<(const QTreeWidgetItem & rawOther) const
{
    // Since this is a reference, the dynamic_cast will throw a std::bad_cast
    // exception if it fails. Not catching this here since this is a genuine
    // error which should not be silently ignored.
    const SuffixSearchResultItem & other = dynamic_cast<const SuffixSearchResultItem &>( rawOther );

    int col = treeWidget() ? treeWidget()->sortColumn() : SSR_PathCol;

    switch ( col )
    {
        case SSR_PathCol:      return path()       < other.path();
        case SSR_CountCol:     return count()      < other.count();
        case SSR_TotalSizeCol: return totalSize()  < other.totalSize();
        default:               return QTreeWidgetItem::operator<( rawOther );
    }
}

