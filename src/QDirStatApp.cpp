/*
 *   File name: QDirStatApp.cpp
 *   Summary:	QDirStat application class for key objects
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include <QWidget>
#include <QList>

#include "QDirStatApp.h"
#include "DirTreeModel.h"
#include "DirTree.h"
#include "FileInfoSet.h"
#include "SelectionModel.h"
#include "CleanupCollection.h"
#include "MainWindow.h"
#include "Logger.h"
#include "Exception.h"


using namespace QDirStat;


QDirStatApp * QDirStatApp::_instance = 0;


QDirStatApp * QDirStatApp::instance()
{
    if ( ! _instance )
        _instance = new QDirStatApp();

    return _instance;
}


void QDirStatApp::createInstance()
{
    if ( ! _instance )
        _instance = new QDirStatApp();
}


void QDirStatApp::deleteInstance()
{
    if ( _instance )
    {
        delete _instance;
        _instance = 0;
    }
}


DirTree * QDirStatApp::dirTree() const
{
    return _dirTreeModel ? _dirTreeModel->tree() : 0;
}


QDirStatApp::QDirStatApp()
{
    // logDebug() << "Creating app" << endl;

    _dirTreeModel = new DirTreeModel();
    CHECK_NEW( _dirTreeModel );

    _selectionModel = new SelectionModel( _dirTreeModel );
    CHECK_NEW( _selectionModel );

    _dirTreeModel->setSelectionModel( _selectionModel );

    _cleanupCollection = new CleanupCollection( _selectionModel );
    CHECK_NEW( _cleanupCollection );
}


QDirStatApp::~QDirStatApp()
{
    // logDebug() << "Destroying app" << endl;

    delete _cleanupCollection;
    delete _selectionModel;
    delete _dirTreeModel;

    // logDebug() << "App destroyed." << endl;
}


QWidget * QDirStatApp::findMainWindow() const
{
    QWidget * mainWin = 0;
    QWidgetList toplevel = QApplication::topLevelWidgets();

    for ( QWidgetList::const_iterator it = toplevel.constBegin();
          it != toplevel.constEnd() && ! mainWin;
          ++it )
    {
        mainWin = qobject_cast<MainWindow *>( *it );
    }

    if ( ! mainWin )
        logWarning() << "NULL mainWin for shared instance" << endl;

    return mainWin;
}


FileInfo * QDirStatApp::selectedDirOrRoot() const
{
    FileInfoSet selectedItems = app()->selectionModel()->selectedItems();
    FileInfo * sel = selectedItems.first();

    if ( ! sel || ! sel->isDir() )
	sel = app()->dirTree()->firstToplevel();

    return sel;
}




QDirStatApp * QDirStat::app()
{
    return QDirStatApp::instance();
}
