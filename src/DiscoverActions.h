/*
 *   File name: DiscoverActions.h
 *   Summary:	Actions for QDirStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef DiscoverActions_h
#define DiscoverActions_h

#include <QObject>
#include <QPointer>
#include <QString>


namespace QDirStat
{
    class TreeWalker;
    class LocateFilesWindow;

    /**
     * Class to keep QDirStat's "discover" actions self-contained.
     *
     * They access the DirTree that was created via the DirTreeModel in the
     * QDirStatApp. They all start with the SelectionModel's currently selected
     * directory or, if none is selected, with the tree's root.
     *
     * They all use a TreeWalker to filter out FileInfo tree nodes and present
     * them as a list in a non-modal LocateFilesWindow. When the user clicks on
     * one of those results, it becomes the current item in the SelectionModel
     * which means that the main window's DirTreeView will scroll to it and
     * open branches until it is visible, and at the same time it will become
     * the current item in the TreemapView.
     *
     * All actions in this class share the same LocateFilesWindow, so any
     * subsequent call will replace any previous content of that window.
     **/
    class DiscoverActions: public QObject
    {
        Q_OBJECT

    public:

        DiscoverActions( QObject * parent = 0 );
        virtual ~DiscoverActions();

    public slots:

        //
        // Actions that can be connected directly to a QAction in one of the
        // menus in the main window
        //

        void discoverLargestFiles();
        void discoverNewestFiles();
        void discoverOldestFiles();
        void discoverHardLinkedFiles();
        void discoverBrokenSymLinks();
        void discoverSparseFiles();


        //
        // Actions that are meant to be connected to the FileAgeWindow's
        // 'locate...()' signals (but they can be used stand-alone as well).
        //

        void discoverFilesFromYear ( const QString & path, short year );
        void discoverFilesFromMonth( const QString & path, short year, short month );

    public:

        /**
         * Common part of all "discover" actions: Create or reuse a
         * LocateFilesWindow with the specified TreeWalker.
         *
         * 'headingText' should include a "%1" placeholder for the path of the
         * starting directory.
         *
         * 'path' can optionally (if non-empty) override the current selection of
         * the tree as the starting directory.
         **/
        void discoverFiles( QDirStat::TreeWalker * treeWalker,
                            const QString &        headingText,
                            const QString &        path = "" );

    protected:

        QPointer<LocateFilesWindow> _locateFilesWindow;

    };  // class DiscoverActions

}       // namespace QDirStat

#endif  // DiscoverActions_h
