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
class DirTreeModel;


class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();

protected slots:
    void expandTree();
    void notImplemented();
    void itemClicked( const QModelIndex & index );

protected:

    void dumpModelTree( const QModelIndex & index, const QString & indent );
    QStringList ancestors( const QModelIndex & index );


    // Window close event ([x] icon in the top right window)
    virtual void closeEvent( QCloseEvent *event );

private:
    Ui::MainWindow *	     _ui;
    QDirStat::DirTreeModel * _dirTreeModel;
    QSortFilterProxyModel  * _sortModel;
    bool		     _modified;
};

#endif // MainWindow_H

