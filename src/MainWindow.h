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
#include <QElapsedTimer>
#include <QPointer>

#include "ui_main-window.h"
#include "FileTypeStatsWindow.h"

class QCloseEvent;
class QSortFilterProxyModel;
class QSignalMapper;


namespace QDirStat
{
    class CleanupCollection;
    class ConfigDialog;
    class DirTreeModel;
    class FileInfo;
    class MimeCategorizer;
    class SelectionModel;
}

using QDirStat::FileInfo;
using QDirStat::FileTypeStatsWindow;



class MainWindow: public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();
    virtual ~MainWindow();

    /**
     * Return the DirTreeModel used in this window.
     **/
    QDirStat::DirTreeModel * dirTreeModel() const { return _dirTreeModel; }

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
     * Re-read the selected branch of the tree.
     **/
    void refreshSelected();

    /**
     * Stop reading if reading is in process.
     **/
    void stopReading();

    /**
     * Clear the current tree and replace it with the content of the specified
     * cache file.
     **/
    void readCache( const QString & cacheFileName );

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
     * Move the selected items to the trash bin.
     **/
    void moveToTrash();

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

    /**
     * Read parameters from the settings file.
     **/
    void readSettings();

    /**
     * Write parameters to the settings file.
     **/
    void writeSettings();

    /**
     * Show online help.
     **/
    void showHelp();


protected slots:

    /**
     * Switch display to "busy display" after reading was started and restart
     * the stopwatch.
     **/
    void startingReading();

    /**
     * Finalize display after reading is finished.
     **/
    void readingFinished();

    /**
     * Finalize display after reading has been aborted.
     **/
    void readingAborted();

    /**
     * Change display mode to "busy" (while reading a directory tree):
     * Sort tree view by read jobs, hide treemap view.
     **/
    void busyDisplay();

    /**
     * Change display mode to "idle" (after reading the directory tree is
     * finished): If the tree view is still sorted by read jobs, now sort it by
     * subtree percent, show the treemap view if enabled.
     **/
    void idleDisplay();

    /**
     * Enable or disable actions depending on current status.
     **/
    void updateActions();

    /**
     * Enable or disable the treemap view, depending on the value of
     * the corresponding action.
     **/
    void showTreemapView();

    /**
     * Switch between showing the treemap view beside the file directory
     * or below it, depending on the corresponding action.
     **/
    void treemapAsSidePanel();

    /**
     * Notification that a cleanup action was started.
     **/
    void startingCleanup( const QString & cleanupName );

    /**
     * Notification that the last process of a cleanup action is finished.
     *
     * 'errorCount' is the total number of errors reported by all processes
     * that were started.
     **/
    void cleanupFinished( int errorCount );

    /**
     * Open the config dialog.
     **/
    void openConfigDialog();

    /**
     * Open or close the file type statistics window.
     **/
    void toggleFileTypeStats();

    /**
     * Switch verbose logging for selection changes on or off.
     *
     * This is normally done by the invisible checkable action
     * _ui->actionVerboseSelection in the main window UI file.
     *
     * The hotkey for this is Shift-F7.
     **/
    void toggleVerboseSelection();

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

    /**
     * Format a millisecond-based time
     **/
    QString formatTime( qint64 millisec );


private:

    Ui::MainWindow		* _ui;
    QDirStat::DirTreeModel	* _dirTreeModel;
    QDirStat::SelectionModel	* _selectionModel;
    QDirStat::CleanupCollection * _cleanupCollection;
    QDirStat::MimeCategorizer	* _mimeCategorizer;
    QDirStat::ConfigDialog	* _configDialog;
    QPointer<FileTypeStatsWindow> _fileTypeStatsWindow;
    QElapsedTimer		  _stopWatch;
    bool			  _modified;
    bool			  _verboseSelection;
    int				  _statusBarTimeout; // millisec
    QSignalMapper	       * _treeLevelMapper;
};

#endif // MainWindow_H

