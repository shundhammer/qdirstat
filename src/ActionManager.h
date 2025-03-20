/*
 *   File name: ActionManager.h
 *   Summary:	Common access to QActions defined in a .ui file
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef ActionManager_h
#define ActionManager_h

#include <QAction>
#include <QList>
#include <QPointer>

/**
 * Container class for QActions that are defined in a Qt Designer .ui file, but
 * that are also needed in context menus e.g. in context menus of views.
 *
 * This is a singleton class that is populated by the class that builds the
 * widget tree from the .ui file by simpling passing the toplevel widget of
 * that tree to this class; the ActionManager uses Qt's introspection to find
 * the matching QActions.
 **/
class ActionManager
{
public:

    /**
     * Return the singleton instance of this class.
     **/
    static ActionManager * instance();

    /**
     * Add a widget tree. This does not transfer ownership of that widget
     * tree. The ActionManager will keep the pointer of this tree (with a
     * guarded pointer so it doesn't matter if it is destroyed) to search
     * for QActions when requested.
     **/
    void addWidgetTree( QObject * tree );

    /**
     * Search the known widget trees for the first QAction with the Qt
     * object name 'actionName'. Return 0 if there is no such QAction.
     **/
    QAction * action( const QString & actionName );

    /**
     * Add all the actions in 'actionNames' to a widget (typically a
     * menu). Return 'true' if success, 'false' if any of the actions were
     * not found.
     *
     * If 'enabledOnly' is 'true', only those actions that are currently
     * enabled are added.
     *
     * If the widget is a menu, and an action name in actionNames starts
     * with "---", a separator is added to the menu instead of an action.
     *
     * Notice that this class already logs an error for action names that
     * were not found.
     **/
    bool addActions( QWidget *           widget,
                     const QStringList & actionNames,
                     bool                enabledOnly = false);

    /**
     * Add only the enabled actions in 'actionNames' to a widget.
     **/
    bool addEnabledActions( QWidget *           widget,
                            const QStringList & actionNames )
        { return addActions( widget, actionNames, true ); }


protected:

    /**
     * Constructor. Protected because this is a singleton class.
     * Use instance() instead.
     **/
    ActionManager() {}


    //
    // Data members
    //

    static ActionManager *    _instance;
    QList<QPointer<QObject> > _widgetTrees;

};	// class ActionManager


#endif	// ActionManager_h
