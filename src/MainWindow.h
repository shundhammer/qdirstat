/*
 *   File name: MainWindow.h
 *   Summary:	QDirStat main window
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#ifndef MainWindow_h
#define MainWindow_h

#include <QMainWindow>
#include <QString>

#include "DirTreeModel.h"
#include "ui_main-window.h"

class QCloseEvent;
class QSortFilterProxyModel;
class QSignalMapper;
class DirTreeModel;


class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();

public slots:
    /**
     * Open an URL (start reading that directory).
     **/
    void openUrl( const QString & url );

    /**
     * Open a directory selection dialog and open the selected URL.
     **/
    void askOpenUrl();

    /**
     * Expand the directory tree's branches to depth 'level'.
     **/
    void expandTreeToLevel( int level );


protected slots:

    void expandTree();
    void notImplemented();
    void itemClicked( const QModelIndex & index );


protected:

    /**
     * Set up QObject connections to the actions from the .ui file
     **/
    void connectActions();

    /**
     * Set up the _treeLevelMapper to map an "expand tree to level x" action to
     * the correct slot.
     **/
    void mapTreeExpandAction( QAction * action, int level );

    /**
     * Window close event ([x] icon in the top right window)
     **/
    virtual void closeEvent( QCloseEvent *event ) Q_DECL_OVERRIDE;


private:

    Ui::MainWindow *	     _ui;
    QDirStat::DirTreeModel * _dirTreeModel;
    QSortFilterProxyModel  * _sortModel;
    bool		     _modified;
    QSignalMapper          * _treeLevelMapper;
};

#endif // MainWindow_H

