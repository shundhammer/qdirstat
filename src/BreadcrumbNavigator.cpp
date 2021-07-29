/*
 *   File name: BreadcrumbNavigator.cpp
 *   Summary:	Breadcrumb widget for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "Qt4Compat.h" // qHtmlEscape()

#include "BreadcrumbNavigator.h"
#include "DirInfo.h"
#include "Logger.h"

#define MAX_TOTAL_LEN           150
#define SHORTENED_LEN            12

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
    fillBreadcrumbs( item );
    shortenBreadcrumbs();
    setText( html() );
}


void BreadcrumbNavigator::fillBreadcrumbs( FileInfo * item )
{
    _breadcrumbs.clear();

    while ( item && ! item->isDirInfo() )
        item = item->parent();

    if ( ! item || ! item->tree() )
        return;

    // logDebug() << item->debugUrl() << endl;
    int depth = item->treeLevel();
    _breadcrumbs = QVector<Breadcrumb>( depth + 1, Breadcrumb() );

    QString name;
    QString basePath;

    FileInfo * toplevel = item->tree()->firstToplevel();
    splitBasePath( toplevel->name(), basePath, name );

    if ( ! basePath.isEmpty() )
        _breadcrumbs[ 0 ].pathComponent = basePath;

    while ( item && depth > 0 )
    {
        // Stop at the DirTree's <root> pseudo item
        if ( item->tree() && item == item->tree()->root() )
            break;

        if ( item->isDirInfo() )
        {
            splitBasePath( item->name(), basePath, name );

            _breadcrumbs[ depth ].pathComponent = name;
            _breadcrumbs[ depth ].url = item->debugUrl();
        }

        item = item->parent();
        --depth;
    }

    if ( _breadcrumbs[ 0 ].pathComponent.isEmpty() )
        _breadcrumbs.removeFirst();

    // logBreadcrumbs();
}


QString BreadcrumbNavigator::html() const
{
    QString html;
    QString name;

    for ( int i=0; i < _breadcrumbs.size(); ++i )
    {
        const Breadcrumb & crumb = _breadcrumbs[ i ];
        name = crumb.displayName;

        if ( name.isEmpty() )
            name = crumb.pathComponent;

        if ( ! name.isEmpty() )
        {
            if ( crumb.url.isEmpty() )
            {
                html += qHtmlEscape( name );
            }
            else
            {
                html += QString( "<a href=\"%1\">%2</a>" )
                    .arg( crumb.url )
                    .arg( qHtmlEscape( name ) );
            }

            if ( ! name.endsWith( "/" ) )
                html += "/";
        }
    }

    return html;
}


void BreadcrumbNavigator::shortenBreadcrumbs()
{
    while ( breadcrumbsLen() > MAX_TOTAL_LEN )
    {
        int index = pickLongBreadcrumb();

        if ( index < 0 )
            return;

        Breadcrumb & crumb = _breadcrumbs[ index ];
        crumb.displayName = ellideMiddle( crumb.pathComponent, SHORTENED_LEN );

#if 0
        logDebug() << "Shortened #" << index
                   << " from " << crumb.pathComponent.length()
                   << " to " <<  crumb.displayName.length()
                   << ": " << crumb.pathComponent
                   << endl;
#endif
    }
}


int BreadcrumbNavigator::pickLongBreadcrumb()
{
    int longestIndex = -1;
    int maxLen       = 0;

    for ( int i=0; i < _breadcrumbs.size(); ++i )
    {
        const Breadcrumb & crumb = _breadcrumbs[ i ];

        if ( crumb.displayName.isEmpty() &&
             crumb.pathComponent.length() > maxLen )
        {
            longestIndex = i;
            maxLen = crumb.pathComponent.length();
        }
    }

    return longestIndex;
}


int BreadcrumbNavigator::breadcrumbsLen() const
{
    int len = 0;

    for ( int i=0; i < _breadcrumbs.size(); ++i )
    {
        const Breadcrumb & crumb = _breadcrumbs[ i ];
        const QString &    name  = crumb.displayName.isEmpty() ?
            crumb.pathComponent : crumb.displayName;

        len += name.length();

        if ( ! name.endsWith( "/" ) )
            ++len;      // For the "/" delimiter
    }

    return len;
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


void BreadcrumbNavigator::logBreadcrumbs() const
{
    logNewline();

    for ( int i=0; i < _breadcrumbs.size(); ++i )
    {
        const Breadcrumb & crumb = _breadcrumbs[ i ];

        logDebug() << "_breadcrumb[ " << i << " ]: "
                   << " pathComponent: \"" << crumb.pathComponent
                   << "\" displayName: \"" << crumb.displayName
                   << "\" url: " << crumb.url << "\""
                   << endl;
    }

    logNewline();
}

