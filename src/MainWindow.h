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
#include "SelectionModel.h"
#include "FileInfo.h"
#include "ui_main-window.h"

class QCloseEvent;
class QSortFilterProxyModel;
class QSignalMapper;

using QDirStat::FileInfo;


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
     * Re-read the complete directory tree.
     **/
    void refreshAll();

    /**
     * Stop reading if reading is in process.
     **/
    void stopReading();

    /**
     * Open a file selection dialog to ask for a cache file, clear the
     * current tree and replace it with the content of the cache file.
     **/
    void askReadCache();

    /**
     * Open a file selection dialog and save the current tree to the selected
     * file.
     **/
    void askWriteCache();

    /**
     * Expand the directory tree's branches to depth 'level'.
     **/
    void expandTreeToLevel( int level );

    /**
     * Show progress text in the status bar for a few seconds.
     **/
    void showProgress( const QString & text );

    /**
     * Show the URL of 'item' and its total size in the status line.
     **/
    void showCurrent( FileInfo * item );

    /**
     * Show a summary of the current selection in the status line.
     **/
    void showSummary();

    /**
     * Copy the URL of the current item (if there is one) to the system
     * clipboard for use in other applications.
     **/
    void copyCurrentUrlToClipboard();

    /**
     * Navigate one directory level up.
     **/
    void navigateUp();

    /**
     * Navigate to the toplevel directory of this tree.
     **/
    void navigateToToplevel();

    /**
     * Show the "about" dialog.
     **/
    void showAboutDialog();


protected slots:

    /**
     * Finalize display after reading is finished.
     **/
    void readingFinished();

    /**
     * Enable or disable actions depending on current status.
     **/
    void updateActions();

    /**
     * Open a popup dialog with a message that this feature is not implemented.
     **/
    void notImplemented();

#if 1
    //
    // Debugging slots
    //

    /**
     * Debug: Item clicked in the tree widget.
     **/
    void itemClicked( const QModelIndex & index );

    void selectionChanged();

    void currentItemChanged( FileInfo * newCurrent, FileInfo * oldCurrent );

    void currentChanged( const QModelIndex & newCurrent,
			 const QModelIndex & oldCurrent );

    void selectionChanged( const QItemSelection & selected,
			   const QItemSelection & deselected );
#endif


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

    Ui::MainWindow	     * _ui;
    QDirStat::DirTreeModel   * _dirTreeModel;
    QDirStat::SelectionModel * _selectionModel;
    bool		       _modified;
    int			       _statusBarTimeOut; // millisec
    QSignalMapper	     * _treeLevelMapper;
};

#endif // MainWindow_H

