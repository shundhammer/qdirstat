/*
 *   File name: BreadcrumbNavigator.cpp
 *   Summary:	Breadcrumb widget for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#include "Qt4Compat.h" // qHtmlEscape()
#include "BreadcrumbNavigator.h"
#include "Logger.h"

using namespace QDirStat;


BreadcrumbNavigator::BreadcrumbNavigator( QWidget * parent ):
    QLabel( parent )
{
    clear();
    setTextFormat( Qt::RichText );

    connect( this, SIGNAL( linkActivated ( QString ) ),
             this, SLOT  ( logPathClicked( QString ) ) );

    connect( this, SIGNAL( linkActivated ( QString ) ),
             this, SIGNAL( pathClicked   ( QString ) ) );
}


BreadcrumbNavigator::~BreadcrumbNavigator()
{
    // NOP
}


void BreadcrumbNavigator::setPath( FileInfo * item )
{
    QString html;

    while ( item )
    {
        // Stop at the DirTree's <root> pseudo item
        if ( item->tree() && item == item->tree()->root() )
            break;

        if ( item->isDirInfo() )
        {
            QString basePath;
            QString name;

            if ( item->isDotEntry() )
                name = FileInfo::dotEntryName();
            else if ( item->isAttic() )
                name = FileInfo::atticName();
            else
                splitBasePath( item->name(), basePath, name );

            QString href= QString( "<a href=\"%1\">%2</a>" )
                .arg( item->debugUrl() )
                .arg( qHtmlEscape( name ) );

            if ( name != "/" )
                href += "/";

            html = qHtmlEscape( basePath ) + href + html;
        }

        item = item->parent();
    }

    setText( html );
}


void BreadcrumbNavigator::splitBasePath( const QString & path,
                                         QString       & basePath_ret, // return parameter
                                         QString       & name_ret )    // return parameter
{
    basePath_ret = "";
    name_ret = path;

    if ( path != "/" && path.contains( "/" ) )
    {
        QStringList components = path.split( "/", QString::SkipEmptyParts );

        if ( ! components.empty() )
            name_ret = components.takeLast();

        if ( ! components.empty() )
            basePath_ret = components.join( "/" ) + "/";

        if ( path.startsWith( "/" ) )
            basePath_ret.prepend( "/" );
    }
}


void BreadcrumbNavigator::logPathClicked( const QString & path )
{
    logInfo() << "Clicked path " << path << endl;
}
