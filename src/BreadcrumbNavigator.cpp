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

#define SHORTEN_PATH_THRESHOLD          150
#define SHORTEN_COMPONENT_THRESHOLD      16

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
    bool    doShorten = pathLen( item ) > SHORTEN_PATH_THRESHOLD;

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

            if ( doShorten )
                name = ellideMiddle( name, SHORTEN_COMPONENT_THRESHOLD );

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


int BreadcrumbNavigator::pathLen( FileInfo * item ) const
{
    if ( ! item )
        return 0;

    if ( ! item->isDirInfo() && item->parent() )
        return item->parent()->debugUrl().length();
    else
        return item->debugUrl().length();
}


QString BreadcrumbNavigator::ellideMiddle( const QString & longText, int maxLen ) const
{
    if ( maxLen < 1 || longText.size() < maxLen )
	return longText;

    QString ellided = longText.left( maxLen / 2 - 2 );
    ellided += "...";
    ellided += longText.right( maxLen / 2 - 1 );

    return ellided;
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
