/*
 *   File name: ActionManager.h
 *   Summary:	Common access to QActions defined in a .ui file
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QMenu>

#include "ActionManager.h"
#include "Exception.h"
#include "Logger.h"

using namespace QDirStat;


ActionManager * ActionManager::_instance = 0;


ActionManager * ActionManager::instance()
{
    if ( ! _instance )
    {
	_instance = new ActionManager();
	CHECK_NEW( _instance );
    }

    return _instance;
}


void ActionManager::addWidgetTree( QObject * tree )
{
    CHECK_PTR( tree );

    _widgetTrees << QPointer<QObject>( tree );
}


QAction * ActionManager::action( const QString & actionName )
{
    foreach ( QPointer<QObject> tree, _widgetTrees )
    {
	if ( tree ) // might be destroyed in the meantime
	{
	    QAction * action = tree->findChild<QAction *>( actionName );

	    if ( action )
		return action;
	}
    }

    logError() << "No action with name " << actionName << " found" << endl;
    return 0;
}


bool ActionManager::addActions( QMenu * menu, const QStringList & actionNames )
{
    bool foundAll = true;
    CHECK_PTR( menu );

    foreach ( const QString & actionName, actionNames )
    {
	if ( actionName.startsWith( "---" ) )
	    menu->addSeparator();
	else
	{
	    QAction * act = action( actionName );

	    if ( act )
		menu->addAction( act );
	    else
		foundAll = false;
	}
    }

    return foundAll;
}

