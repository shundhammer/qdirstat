/*
 *   File name: BookmarksManager.cpp
 *   Summary:	Bookmarks Manager for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include <QMenu>
#include <QAction>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "BookmarksManager.h"
#include "DirTree.h"
#include "Logger.h"

#define BOOKMARKS_FILE  ".config/QDirStat/bookmarks.txt"


using namespace QDirStat;


BookmarksManager::BookmarksManager( QObject * parent ):
    QObject( parent ),
    _bookmarksMenu( 0 ),
    _dirty( false )
{

}


BookmarksManager::~BookmarksManager()
{
    // NOP
}


void BookmarksManager::clear()
{
    _bookmarks.clear();
}


void BookmarksManager::add( const QString & bookmark, bool update )
{
    if ( _bookmarks.contains( bookmark, Qt::CaseSensitive ) )
        return;

    logInfo() << "Adding bookmark \"" << bookmark << "\"" << endl;

    _bookmarks << bookmark;
    _dirty = true;

    if ( update )
    {
        sort();
        rebuildBookmarksMenu();
    }
}


void BookmarksManager::remove( const QString & bookmark, bool update )
{
    if ( ! _bookmarks.contains( bookmark, Qt::CaseSensitive ) )
        return;

    logInfo() << "Removing bookmark \"" << bookmark << "\"" << endl;

    _bookmarks.removeAll( bookmark );
    _dirty = true;

    if ( update )
        rebuildBookmarksMenu();
}


void BookmarksManager::rebuildBookmarksMenu()
{
    if ( ! _bookmarksMenu )
    {
        logError() << "NULL _bookmarksMenu" << endl;
        return;
    }

    _bookmarksMenu->clear();

    foreach( const QString & bookmark, _bookmarks )
    {
        QAction * action = _bookmarksMenu->addAction( bookmark,
                                                      this, SLOT( menuActionTriggered() ) );
        if ( action && ! _baseUrl.isEmpty() )
        {
            if ( ! bookmark.startsWith( _baseUrl ) )
                action->setEnabled( false );
        }
    }

    if ( _bookmarksMenu->isEmpty() )
    {
        QAction * action = _bookmarksMenu->addAction( tr( "No Bookmarks" ) );

        if ( action )
            action->setEnabled( false );
    }
}


void BookmarksManager::menuActionTriggered()
{
    QAction * action = qobject_cast<QAction *>( sender() );

    if ( action )
    {
        QString bookmark = action->data().toString();

        if ( bookmark.isEmpty() )
            bookmark = action->text();

        if ( ! bookmark.isEmpty() )
        {
            logDebug() << "Bookmark activated: " << bookmark << endl;
            emit navigateToUrl( bookmark );
        }
        else
        {
            logError() << "Empty bookmark activated" << endl;
        }
    }
}


void BookmarksManager::setBaseUrl( const QString & newBaseUrl )
{
    _baseUrl = newBaseUrl;
    // logDebug() << "Base URL: \"" << _baseUrl << "\"" << endl;

    rebuildBookmarksMenu();
}


void BookmarksManager::sort()
{
    _bookmarks.sort();
}


void BookmarksManager::read()
{
    QFile bookmarksFile( bookmarksFileName() );

    if ( ! bookmarksFile.exists() )
    {
        logInfo() << "Bookmarks file " << bookmarksFileName()
                  << " does not exist" << endl;

        return;
    }

    if ( ! bookmarksFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
	logError() << "Can't open " << bookmarksFileName() << endl;
	return;
    }

    _bookmarks.clear();
    QTextStream in( &bookmarksFile );
    QString line = in.readLine();

    while ( ! in.atEnd() )
    {
        line = line.trimmed();

        if ( ! line.isEmpty() && ! line.startsWith( "#" ) )
            _bookmarks << line;

	line = in.readLine();
    }

    sort();
    logInfo() << _bookmarks.size() << " bookmarks read from " << bookmarksFileName() << endl;

#if 0
    foreach( const QString & bookmark, _bookmarks )
        logDebug() << "Read bookmark \"" << bookmark << "\"" << endl;
#endif
}


void BookmarksManager::write()
{
    if ( ! _dirty )
    {
        logDebug() << "No changes to write to " << bookmarksFileName() << endl;
        return;
    }

    QFile bookmarksFile( bookmarksFileName() );

    if ( ! bookmarksFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
	logError() << "Can't open " << bookmarksFileName() << endl;
	return;
    }

    QTextStream out( &bookmarksFile );

    foreach( const QString & bookmark, _bookmarks )
    {
        out << bookmark << "\n";
    }

    logInfo() << _bookmarks.size() << " bookmarks written to " << bookmarksFileName() << endl;
    _dirty = false;
}


QString BookmarksManager::bookmarksFileName()
{
    return QDir::homePath() + "/" + BOOKMARKS_FILE;
}
