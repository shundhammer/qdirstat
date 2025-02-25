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

#define BookmarksFile           "~/.config/QDirStat/bookmarks.txt"
#define BookmarksManagerAction  "bookmarksManagerAction"


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

    clearMenu();
    QIcon bookmarkIcon( ":/icons/tree-medium/dir.png" );

    foreach ( const QString & bookmark, _bookmarks )
    {
        QAction * action = _bookmarksMenu->addAction( shortenedPath( bookmark ),
                                                      this, SLOT( navigateToBookmark() ) );
        if ( action )
        {
            action->setObjectName( BookmarksManagerAction );
            action->setData( expandedPath( bookmark ) );
            action->setIcon( bookmarkIcon );

            if ( ! _baseUrl.isEmpty() )
            {
                if ( ! bookmark.startsWith( _baseUrl ) )
                    action->setEnabled( false );
            }
            else // No base URL set -> probably no DirTree read yet
            {
                action->setEnabled( false );
            }
        }
    }

    if ( _bookmarksMenu->isEmpty() )
    {
        QAction * action = _bookmarksMenu->addAction( tr( "No Bookmarks" ) );

        if ( action )
        {
            action->setObjectName( BookmarksManagerAction );
            action->setEnabled( false );
        }
    }
}


void BookmarksManager::clearMenu()
{
    if ( ! _bookmarksMenu )
        return;

    QList<QAction *> actions =
        _bookmarksMenu->findChildren<QAction *>( BookmarksManagerAction );

    foreach ( QAction * action, actions )
        _bookmarksMenu->removeAction( action );
}


void BookmarksManager::navigateToBookmark()
{
    // Since we use the same slot for all bookmark menu entries, use the sender
    // of the signal to get more information.
    //
    // Notice that this wouldn't work if this were a normal function call, not
    // sent via a Qt signal / slot connection.

    QAction * action = qobject_cast<QAction *>( sender() );

    if ( action )
    {
        // The bookmark actions all contain the URL to go to in the action's
        // data() (a QVariant).

        QString bookmark = action->data().toString();

        // If data() was empty, try the menu text. This may or may not be the
        // same as the URL; future versions may shorten this or replace things
        // like the user's home directory with "~/" for brevity.

        if ( bookmark.isEmpty() )
            bookmark = action->text();

        if ( ! bookmark.isEmpty() )
        {
            logDebug() << bookmark << endl;
            emit navigateToUrl( expandedPath( bookmark ) );
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
        logInfo() << "Bookmarks file " << BookmarksFile
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

    while ( ! in.atEnd() )
    {
        QString line = in.readLine().trimmed();

        if ( ! line.isEmpty() && ! line.startsWith( "#" ) )
            _bookmarks << expandedPath( line );
    }

    sort();
    logInfo() << _bookmarks.size() << " bookmarks read from " << BookmarksFile << endl;

#if 0
    foreach ( const QString & bookmark, _bookmarks )
        logDebug() << "Read bookmark \"" << bookmark << "\"" << endl;
#endif
}


void BookmarksManager::write()
{
    if ( ! _dirty )
    {
        logDebug() << "No changes to write to " << BookmarksFile << endl;
        return;
    }

    QFile bookmarksFile( bookmarksFileName() );

    if ( ! bookmarksFile.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
	logError() << "Can't open " << bookmarksFileName() << endl;
	return;
    }

    QTextStream out( &bookmarksFile );

    foreach ( const QString & bookmark, _bookmarks )
    {
        out << shortenedPath( bookmark ) << "\n";
    }

    logInfo() << _bookmarks.size() << " bookmarks written to " << BookmarksFile << endl;
    _dirty = false;
}


QString BookmarksManager::bookmarksFileName()
{
    return expandedPath( BookmarksFile );
}


QString BookmarksManager::expandedPath( const QString & origPath )
{
    QString home = QDir::homePath();
    QString path = origPath;

    path.replace( "~",       home );
    path.replace( "$HOME",   home );
    path.replace( "${HOME}", home );

    return path;
}


QString BookmarksManager::shortenedPath( const QString & origPath )
{
    QString home = QDir::homePath();
    QString path = origPath;

    if ( ! home.isEmpty() && path.startsWith( home ) )
        path.replace( 0, home.size(), "~" );

    return path;
}
