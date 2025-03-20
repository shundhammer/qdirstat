/*
 *   File name: DiscoverActions.cpp
 *   Summary:	Actions for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "DiscoverActions.h"
#include "TreeWalker.h"
#include "LocateFilesWindow.h"
#include "FileSearchFilter.h"
#include "DirInfo.h"
#include "BusyPopup.h"
#include "QDirStatApp.h"
#include "Logger.h"
#include "Exception.h"



DiscoverActions::DiscoverActions( QObject * parent ):
    QObject( parent )
{
    // NOP
}


DiscoverActions::~DiscoverActions()
{
    // Notice that _locateFilesWindow gets the main window as its widget
    // parent, so it will automatically be destroyed when the main window and
    // its child widgets are destroyed.
}


void DiscoverActions::discoverLargestFiles()
{
    discoverFiles( new LargestFilesTreeWalker(),
                   tr( "Largest Files in %1" ) );
    _locateFilesWindow->sortByColumn( LocateListSizeCol, Qt::DescendingOrder );
}


void DiscoverActions::discoverNewestFiles()
{
    discoverFiles( new NewFilesTreeWalker(),
                   tr( "Newest Files in %1" ) );
    _locateFilesWindow->sortByColumn( LocateListMTimeCol, Qt::DescendingOrder );
}


void DiscoverActions::discoverOldestFiles()
{
    discoverFiles( new OldFilesTreeWalker(),
                   tr( "Oldest Files in %1" ) );
    _locateFilesWindow->sortByColumn( LocateListMTimeCol, Qt::AscendingOrder );
}


void DiscoverActions::discoverHardLinkedFiles()
{
    discoverFiles( new HardLinkedFilesTreeWalker(),
                   tr( "Files with Multiple Hard Links in %1" ) );
    _locateFilesWindow->sortByColumn( LocateListPathCol, Qt::AscendingOrder );
}


void DiscoverActions::discoverBrokenSymLinks()
{
    BusyPopup msg( tr( "Checking symlinks..." ), app()->findMainWindow() );
    discoverFiles( new BrokenSymLinksTreeWalker(),
                   tr( "Broken Symbolic Links in %1" ) );
    _locateFilesWindow->sortByColumn( LocateListPathCol, Qt::AscendingOrder );
}


void DiscoverActions::discoverSparseFiles()
{
    discoverFiles( new SparseFilesTreeWalker(),
                   tr( "Sparse Files in %1" ) );
    _locateFilesWindow->sortByColumn( LocateListSizeCol, Qt::DescendingOrder );
}


void DiscoverActions::discoverFilesFromYear( const QString & path, short year )
{
    QString headingText = tr( "Files from %1 in %2" ).arg( year ).arg( "%1");

    discoverFiles( new FilesFromYearTreeWalker( year ), headingText, path );
    _locateFilesWindow->sortByColumn( LocateListMTimeCol, Qt::DescendingOrder );
}


void DiscoverActions::discoverFilesFromMonth( const QString & path, short year, short month )
{
    QString headingText = tr( "Files from %1/%2 in %3" ).arg( month ).arg( year).arg( "%1");

    discoverFiles( new FilesFromMonthTreeWalker( year, month ), headingText, path );
    _locateFilesWindow->sortByColumn( LocateListMTimeCol, Qt::DescendingOrder );
}




void DiscoverActions::discoverFiles( TreeWalker *    treeWalker,
                                     const QString & headingText,
                                     const QString & path         )
{
    ensureLocateFilesWindow( treeWalker );
    FileInfo * sel = 0;

    if ( ! path.isEmpty() )
    {
        sel = app()->dirTree()->locate( path,
                                        true ); // findPseudoDirs
    }

    if ( ! sel )
        sel = app()->selectedDirInfoOrRoot();

    if ( sel )
    {
        if ( ! headingText.isEmpty() )
            _locateFilesWindow->setHeading( headingText.arg( sel->url() ) );

        _locateFilesWindow->populate( sel );
        _locateFilesWindow->show();
    }
}


void DiscoverActions::ensureLocateFilesWindow( TreeWalker * treeWalker )
{
    if ( ! _locateFilesWindow )
    {
	// This deletes itself when the user closes it. The associated QPointer
	// keeps track of that and sets the pointer to 0 when it happens.

	_locateFilesWindow = new LocateFilesWindow( treeWalker,
                                                    app()->findMainWindow() ); // parent
    }
    else
    {
        _locateFilesWindow->setTreeWalker( treeWalker );
    }
}


void DiscoverActions::findFiles( const FileSearchFilter & filter )
{

    ensureLocateFilesWindow( new FindFilesTreeWalker( filter ) );
    FileInfo * sel = filter.subtree();

    if ( ! sel )
        sel = app()->selectedDirInfoOrRoot();

    if ( sel )
    {
        QString headingText = tr( "Search Results for \"%1\"" ).arg( filter.pattern() );

        _locateFilesWindow->setHeading( headingText );
        _locateFilesWindow->populate( sel );
        _locateFilesWindow->show();
    }
}
