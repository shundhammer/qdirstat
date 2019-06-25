/*
 *   File name: LocateFilesWindow.cpp
 *   Summary:	QDirStat file type statistics window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <algorithm>

#include "LocateFilesWindow.h"
#include "DirTree.h"
#include "DotEntry.h"
#include "SelectionModel.h"
#include "SettingsHelpers.h"
#include "HeaderTweaker.h"
#include "Logger.h"
#include "Exception.h"

using namespace QDirStat;


LocateFilesWindow::LocateFilesWindow( SelectionModel * selectionModel,
				      QWidget *	       parent ):
    QDialog( parent ),
    _ui( new Ui::LocateFilesWindow ),
    _selectionModel( selectionModel )
{
    // logDebug() << "init" << endl;

    CHECK_NEW( _ui );
    _ui->setupUi( this );
    initWidgets();
    readWindowSettings( this, "LocateFilesWindow" );

    connect( _ui->refreshButton, SIGNAL( clicked() ),
	     this,		 SLOT  ( refresh() ) );

    connect( _ui->treeWidget,	 SIGNAL( currentItemChanged( QTreeWidgetItem *,
							     QTreeWidgetItem * ) ),
	     this,		 SLOT  ( selectResult	   ( QTreeWidgetItem * ) ) );
}


LocateFilesWindow::~LocateFilesWindow()
{
    // logDebug() << "destroying" << endl;
    writeWindowSettings( this, "LocateFilesWindow" );
}


void LocateFilesWindow::clear()
{
    _searchSuffix.clear();
    _ui->treeWidget->clear();
}


void LocateFilesWindow::refresh()
{
    populate( searchSuffix(), _subtree() );
}


void LocateFilesWindow::initWidgets()
{
    QFont font = _ui->heading->font();
    font.setBold( true );
    _ui->heading->setFont( font );

    _ui->treeWidget->setColumnCount( SSR_ColumnCount );
    _ui->treeWidget->setHeaderLabels( QStringList()
				      << tr( "Number" )
				      << tr( "Total Size" )
				      << tr( "Directory" ) );
    _ui->treeWidget->header()->setStretchLastSection( false );
    HeaderTweaker::resizeToContents( _ui->treeWidget->header() );
}


void LocateFilesWindow::reject()
{
    deleteLater();
}


QString LocateFilesWindow::searchSuffix() const
{
    return QString( "*" ) + _searchSuffix;
}


void LocateFilesWindow::populate( const QString & suffix, FileInfo * newSubtree )
{
    clear();

    _searchSuffix = suffix;
    _subtree      = newSubtree;

    if ( _searchSuffix.startsWith( '*' ) )
	_searchSuffix.remove( 0, 1 ); // Remove the leading '*'

    if ( ! _searchSuffix.startsWith( '.' ) )
	_searchSuffix.prepend( '.' );

    _ui->heading->setText( tr( "Directories with %1 Files below %2" )
                           .arg( searchSuffix() )
                           .arg( _subtree.url() ) );

    logDebug() << "Locating all files ending with \""
	       << _searchSuffix << "\" below " << _subtree.url() << endl;

    // For better Performance: Disable sorting while inserting many items
    _ui->treeWidget->setSortingEnabled( false );

    locate( newSubtree ? newSubtree : _subtree() );

    _ui->treeWidget->setSortingEnabled( true );
    _ui->treeWidget->sortByColumn( SSR_PathCol, Qt::AscendingOrder );
    logDebug() << _ui->treeWidget->topLevelItemCount() << " directories" << endl;

    // Make sure something is selected, even if this window is not the active
    // one (for example because the user just clicked on another suffix in the
    // file type stats window). When the window is activated, the tree widget
    // automatically uses the topmost item as the current item, and in the
    // default selection mode, this item is also selected. When the window is
    // not active, this does not happen yet - until the window is activated.
    //
    // In the context of QDirStat, this means that this is also signaled to the
    // SelectionModel, the corresponding branch in the main window's dir tree
    // is opened, and the matching files are selected in the dir tree and in
    // the treemap.
    //
    // It is very irritating if this only happens sometimes - when the "locate
    // files" window is created, but not when it is just populated with new
    // content from the outside (from the file type stats window).
    //
    // So let's make sure the topmost item is always selected.

    _ui->treeWidget->setCurrentItem( _ui->treeWidget->topLevelItem( 0 ) );
}


void LocateFilesWindow::locate( FileInfo * dir )
{
    if ( ! dir )
	return;

    FileInfoSet matches = matchingFiles( dir );

    if ( ! matches.isEmpty() )
    {
	// Create a search result for this path

	FileSize totalSize = 0LL;

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
	if ( child->isFile() &&
	     child->name().endsWith( _searchSuffix, Qt::CaseInsensitive ) )
	{
	    result << child;
	}

	child = child->next();
    }

    return result;
}


void LocateFilesWindow::selectResult( QTreeWidgetItem * item )
{
    if ( ! item )
	return;

    SuffixSearchResultItem * searchResult =
	dynamic_cast<SuffixSearchResultItem *>( item );
    CHECK_DYNAMIC_CAST( searchResult, "SuffixSearchResultItem" );
    CHECK_PTR( _subtree.tree() );

    FileInfo * dir = _subtree.tree()->locate( searchResult->path() );
    FileInfoSet matches = matchingFiles( dir );

    // logDebug() << "Selecting " << searchResult->path() << " with " << matches.size() << " matches" << endl;

    if ( ! matches.isEmpty() )
	_selectionModel->setCurrentItem( matches.first(), true );

    _selectionModel->setSelectedItems( matches );
}






SuffixSearchResultItem::SuffixSearchResultItem( const QString & path,
						int		count,
						FileSize	totalSize ):
    QTreeWidgetItem( QTreeWidgetItem::UserType ),
    _path( path ),
    _count( count ),
    _totalSize( totalSize )
{
    setText( SSR_CountCol,	QString( "%1" ).arg( count ) );
    setText( SSR_TotalSizeCol,	formatSize( totalSize ) );
    setText( SSR_PathCol,	path );

    setTextAlignment( SSR_CountCol,	 Qt::AlignRight );
    setTextAlignment( SSR_TotalSizeCol,	 Qt::AlignRight );
    setTextAlignment( SSR_PathCol,	 Qt::AlignLeft	);
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
	case SSR_PathCol:      return path()	   < other.path();
	case SSR_CountCol:     return count()	   < other.count();
	case SSR_TotalSizeCol: return totalSize()  < other.totalSize();
	default:	       return QTreeWidgetItem::operator<( rawOther );
    }
}

