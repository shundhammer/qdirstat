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

#include "FileInfo.h"
#include "DirTree.h"


namespace QDirStat
{
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
    };

} // namespace QDirStat

#endif // BreadcrumbNavigator_h
