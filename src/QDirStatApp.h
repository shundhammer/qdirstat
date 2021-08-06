/*
 *   File name: QDirStatApp.h
 *   Summary:	QDirStat application class for key objects
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */

#ifndef QDirStatApp_h
#define QDirStatApp_h


class QWidget;


namespace QDirStat
{
    class DirTreeModel;
    class DirTree;
    class SelectionModel;
    class CleanupCollection;
    class QDirStatApp;
    class FileInfo;


    /**
     * Access the singleton instance of the QDirStatApp class. If no instance
     * exists yet, this will create it and also the key objects that it manages
     * (see below).
     **/
    QDirStatApp * app();


    /**
     * This is the application object for the QDirStat application with a
     * similar approach to Qt's QCoreApplication. It does not create any window
     * or widget, so it is meant to be instantiated before any widgets or
     * anything GUI related.
     *
     * This class holds key objects and initializes them in the correct order,
     * and it allows access to those key objects from other classes without
     * having to pass every single one of them to each of the other classes
     * that needs them.
     *
     * This is a singleton class.
     **/
    class QDirStatApp
    {
    public:

        /**
         * Explicitly create the singleton instance of this class if it isn't
         * created yet. Do nothing if it already exists.
         **/
        static void createInstance();

        /**
         * Delete the singleton instance of this class and all the key objects
         * that it manages. It is important that the widgets that need any of
         * them are deleted BEFORE deleting this app instance.
         **/
        static void deleteInstance();

        /**
         * Access the singleton instance of the QDirStatApp class. If no
         * instance exists yet, this will create it and also the key objects
         * that it manages.
         *
         * Typically, you will want to use the global app() function instead.
         **/
        static QDirStatApp * instance();


        //
        // Access to key objects
        //

        /**
         * Return the directory tree model. This is the model part of Qt
         * model/view widgets such as the DirTreeView (QAbstractItemView) or
         * the TreemapView.
         *
         * It has a DirTree that actually holds the in-memory tree of FileInfo
         * / DirInfo nodes.
         **/
        DirTreeModel * dirTreeModel() const { return _dirTreeModel; }

        /**
         * Return the DirTree that is owned by the DirTreeModel.
         *
         * A DirTree is the in-memory representation of a directory tree
         * consisting of FileInfo nodes or more specialized classes derived
         * from FileInfo such as DirInfo, DotEntry, Attic, or even PkgInfo.
         *
         * A DirTree may start with PkgInfo nodes that each represent one
         * installed software package. A PkgInfo node typically has DirInfo /
         * FileInfo child nodes each representing a directory with files that
         * belong to that software package.
         **/
        DirTree * dirTree() const;

        /**
         * Return the SelectionModel that keeps track of what items are marked
         * as selected across the different connected views, i.e. the DirTreeView
         * and the TreemapView.
         **/
        SelectionModel * selectionModel() const { return _selectionModel; }

        /**
         * Return the CleanupCollection, i.e. the collection of actions that
         * the user can start to clean up after files or directories are found
         * that should be deleted or compressed or in general cleaned up.
         *
         * That includes actions that are purely inspecting files or
         * directories, such as starting an interactive shell in that directory
         * or showing the directory in a file manager window. Most cleanup
         * actions are started as external commands, and they can be configured
         * to the user's liking with the configuration dialog.
         **/
        CleanupCollection * cleanupCollection() const { return _cleanupCollection; }


        //
        // Convenience methods
        //



        /**
         * Return the (first) MainWindow instance of the running program that
         * is suitable as a widget parent for subwindows to maintain the
         * correct window stacking order (and avoid having subwindows disappear
         * behind the main window). Return 0 if there is no MainWindow (yet).
         **/
        QWidget * findMainWindow() const;

        /**
         * Return the first selected directory from the SelectionModel or, if
         * none is selected, the DirTree's root directory.
         *
         * Notice that this might still return 0 if the tree is completely
         * empty.
         **/
        FileInfo * selectedDirOrRoot() const;


    protected:

        /**
         * Constructor
         **/
        QDirStatApp();

        /**
         * Destructor.
         **/
        virtual ~QDirStatApp();


        //
        // Data members
        //

        DirTreeModel            * _dirTreeModel;
        SelectionModel          * _selectionModel;
        CleanupCollection       * _cleanupCollection;

        static QDirStatApp      * _instance;

    };  // class QDirStatApp

}       // namespace QDirStat

#endif  // class QDirStatApp_h
