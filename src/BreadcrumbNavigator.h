/*
 *   File name: BreadcrumbNavigator.h
 *   Summary:	Breadcrumb widget for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef BreadcrumbNavigator_h
#define BreadcrumbNavigator_h


#include <QLabel>
#include <QVector>

#include "FileInfo.h"
#include "DirTree.h"


namespace QDirStat
{
    /**
     * Helper class to represent one single breadcrumb
     **/
    struct Breadcrumb
    {
        Breadcrumb( const QString & path = QString() ):
            pathComponent( path )
            {}

        QString pathComponent;
        QString displayName;   // This may be shortened
        QString url;
    };


    /**
     * Widget for "breadcrumb" navigation in a directory tree:
     *
     * Show the current path with clickable components so the user can easily
     * see where in the tree the currently selected item is and can easily
     * navigate upwards in the tree.
     *
     * Each component is an individual hyperlink.
     *
     * Upwards navigation is limited to the root of the directory tree,
     * i.e. the user can only navigate inside the current tree.
     *
     * This widget does not hang on to any FileInfo or DirTree object; once a
     * current path is set, it deals only with strings internally.
     **/
    class BreadcrumbNavigator: public QLabel
    {
	Q_OBJECT

    public:

	/**
	 * Constructor.
	 **/
	BreadcrumbNavigator( QWidget * parent = 0 );

	/**
	 * Destructor.
	 **/
	virtual ~BreadcrumbNavigator();


    public slots:

	/**
	 * Set the path from a FileInfo item.
         * A null item clears the path.
	 **/
	void setPath( FileInfo * item );

    signals:

	/**
	 * Notification that the user activated a path.
	 *
	 * Usually this should be connected to some navigation slot to select
	 * the clicked directory in a view.
	 **/
	void pathClicked( const QString & path );


    protected slots:

        void logPathClicked( const QString & path );


    protected:

        /**
         * Split a path up into its base path (everything up to the last path
         * component) and its base name (the last path component).
         *
         * Both 'basePath_ret' and 'name_ret' are return parameters and will be
         * modified by this function. If nonempty, a trailing path separator
         * ("/") is added to 'basePath_ret'.
         **/
        void splitBasePath( const QString & path,
                            QString       & basePath_ret, // return parameter
                            QString       & name_ret );   // return parameter

        /**
         * Return the total display length of all breadcrumbs plus delimiters.
         **/
        int breadcrumbsLen() const;

        /**
         * Elide a text that would be longer than maxLen in the middle and
         * return the shortened text.
         **/
        QString ellideMiddle( const QString & text, int maxLen ) const;

        /**
         * Fill the internal _breadcrumbs with content by traversing up the
         * tree from 'item' to the toplevel.
         **/
        void fillBreadcrumbs( FileInfo * item );

        /**
         * Generate HTML from _breadcrumbs
         **/
        QString html() const;

        /**
         * Shorten exessively long _breadcrumbs so they have a better chance to
         * fit on the screen.
         **/
        void shortenBreadcrumbs();

        /**
         * Return the index of the longest breadcrumb that has not been
         * shortened yet or -1 if there is no more.
         **/
        int pickLongBreadcrumb();

        /**
         * Write the internal _breadcrumbs to the log.
         **/
        void logBreadcrumbs() const;


        //
        // Data members
        //

        QVector<Breadcrumb> _breadcrumbs;
    };

} // namespace QDirStat

#endif // BreadcrumbNavigator_h
