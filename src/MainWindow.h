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
#include <QTimer>
#include <QPointer>

#include "ui_main-window.h"
#include "FileTypeStatsWindow.h"
#include "PkgFilter.h"


class QCloseEvent;
class QSortFilterProxyModel;
class QSignalMapper;
class TreeLayout;


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
     * Open an URL (directory or package URL).
     **/
    void openUrl( const QString & url );

    /**
     * Open a directory URL (start reading that directory).
     **/
    void openDir( const QString & url );

    /**
     * Open a directory selection dialog and open the selected URL.
     **/
    void askOpenDir();

    /**
     * Open a package selection dialog and open the selected URL.
     **/
    void askOpenPkg();

    /**
     * Open a "show unpackaged files" dialog and start reading the selected
     * starting dir with the selected exclude dirs.
     **/
    void askShowUnpkgFiles();

    /**
     * Show unpackaged files: Start reading the specified URL with the
     * specified exclude dirs.
     *
     * The URL may start with "unpkg:".
     **/
    void showUnpkgFiles( const QString	   & url,
			 const QStringList & excludeDirs );

    /**
     * Show unpackaged files with the default exclude dirs.
     **/
    void showUnpkgFiles( const QString & url );

    /**
     * Return 'true' if the URL starts with "unpkg:/".
     **/
    static bool isUnpkgUrl( const QString & url );

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
     * Clear the current tree and replace it with the list of installed
     * packages from the system's package manager that match 'pkgUrl'.
     **/
    void readPkg( const QDirStat::PkgFilter & pkgFilter );

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
     * Update the window title: Show "[root]" if running as root and add the
     * URL if that is configured.
     **/
    void updateWindowTitle( const QString & url );

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
     * Show details about the current selection in the details view.
     **/
    void updateFileDetailsView();

    /**
     * Copy the path of the current item (if there is one) to the system
     * clipboard for use in other applications.
     **/
    void copyCurrentPathToClipboard();

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
     * Show online help.
     **/
    void showHelp();

    /**
     * Show online help about the packages view.
     **/
    void showPkgViewHelp();

    /**
     * Show the "about" dialog.
     **/
    void showAboutDialog();

    /**
     * Show the "Donate" dialog.
     **/
    void showDonateDialog();

    /**
     * Show the release notes for the last released version in a web browser.
     **/
    void showWhatsNew();

    /**
     * Read parameters from the settings file.
     **/
    void readSettings();

    /**
     * Write parameters to the settings file.
     **/
    void writeSettings();


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
     * Show file type statistics for the currently selected directory.
     **/
    void showFileTypeStats();

    /**
     * Show file size statistics for the currently selected directory.
     **/
    void showFileSizeStats();

    /**
     * Change the main window layout. If no name is passed, the function tries
     * to check if the sender is a QAction and use its data().
     **/
    void changeLayout( const QString & name = QString() );

    /**
     * Show the elapsed time while reading.
     **/
    void showElapsedTime();

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
     * Return the first selected directory or, if none is selected, the root
     * directory.
     **/
    FileInfo * selectedDirOrRoot() const;

    /**
     * Set up QObject connections (all except from QActions)
     **/
    void connectSignals();

    /**
     * Set up QObject connections from the actions from the .ui file
     **/
    void connectActions();

    /**
     * Set up the _treeLevelMapper to map an "expand tree to level x" action to
     * the correct slot.
     **/
    void mapTreeExpandAction( QAction * action, int level );

    /**
     * Initialize the layout actions.
     **/
    void initLayoutActions();

    /**
     * Create the different top layouts.
     **/
    void createLayouts();

    /**
     * Save the current settings in 'layout'.
     **/
    void saveLayout( TreeLayout * layout );

    /**
     * Apply a layout to the current settings.
     **/
    void applyLayout( TreeLayout * layout );

    /**
     * Read settings for one layout.
     **/
    void readLayoutSettings( TreeLayout * layout );

    /**
     * Write settings for one layout.
     **/
    void writeLayoutSettings( TreeLayout * layout );

    /**
     * Window close event ([x] icon in the top right window)
     **/
    virtual void closeEvent( QCloseEvent *event ) Q_DECL_OVERRIDE;

    /**
     * Format a millisecond-based time
     **/
    QString formatTime( qint64 millisec, bool showMillisec = true );


private:

    Ui::MainWindow		* _ui;
    QDirStat::DirTreeModel	* _dirTreeModel;
    QDirStat::SelectionModel	* _selectionModel;
    QDirStat::CleanupCollection * _cleanupCollection;
    QDirStat::MimeCategorizer	* _mimeCategorizer;
    QDirStat::ConfigDialog	* _configDialog;
    QActionGroup		* _layoutActionGroup;
    QPointer<FileTypeStatsWindow> _fileTypeStatsWindow;
    QString			  _dUrl;
    QElapsedTimer		  _stopWatch;
    bool			  _modified;
    bool			  _verboseSelection;
    bool			  _urlInWindowTitle;
    bool			  _useTreemapHover;
    QString			  _layoutName;
    int				  _statusBarTimeout; // millisec
    QSignalMapper	       *  _treeLevelMapper;
    QMap<QString, TreeLayout *>	  _layouts;
    TreeLayout *		  _currentLayout;
    QTimer			  _updateTimer;

}; // class MainWindow


/**
 * Helper class for the different layouts of the tree view layout.
 **/
class TreeLayout
{
public:
    TreeLayout( const QString & name ):
	name( name ),
	showCurrentPath( true ),
	showDetailsPanel( true )
	{}

    QString name;
    bool    showCurrentPath;
    bool    showDetailsPanel;

}; // class TreeLayout


#endif // MainWindow_H
