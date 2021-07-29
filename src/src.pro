# qmake .pro file for qdirstat/src
#
# Go to the project toplevel dir and build all Makefiles:
#
#     qmake
#
# Then build with
#
#     make
#

TEMPLATE	 = app

QT		+= widgets
# Commented out to get -O2 optimization by default (issue #160)
# CONFIG	+= debug
DEPENDPATH	+= .
MOC_DIR		 = .moc
OBJECTS_DIR	 = .obj
LIBS		+= -lz

major_is_less_5 = $$find(QT_MAJOR_VERSION, [234])
!isEmpty(major_is_less_5):DEFINES += 'Q_DECL_OVERRIDE=""'
isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr

TARGET		 = qdirstat
TARGET.files	 = qdirstat
TARGET.path	 = $$INSTALL_PREFIX/bin
INSTALLS	+= TARGET desktop icons


# Fix the train wreck that Qt 5.15 is.
#
# Some genius decided to mark everything as deprecated that will change in
# the upcoming Qt 6.0, but they had not considered that
# THE PROPOSED CHANGES ARE COMPLETELY INCOMPATIBLE TO Qt 5.14 OR EARLIER!!
#
# Some enum values moved from the QString namespace to the Qt namespace; you
# can write code that will compile with Qt 5.1 - 5.14 OR for Qt 5.15, but not both.
#
# Great move, guys. In the 20+ years (since 1998 or so) that I have been
# working with Qt, this level of complete and utter fuck-up never happened
# before, much less between minor versions.
#
# So there is only one solution: STFU, Qt!

QMAKE_CXXFLAGS	+=  -Wno-deprecated -Wno-deprecated-declarations


SOURCES	  = main.cpp			\
	    ActionManager.cpp		\
	    AdaptiveTimer.cpp		\
	    Attic.cpp			\
	    BreadcrumbNavigator.cpp	\
	    BucketsTableModel.cpp	\
	    BusyPopup.cpp		\
	    Cleanup.cpp			\
	    CleanupCollection.cpp	\
	    CleanupConfigPage.cpp	\
	    ConfigDialog.cpp		\
	    DataColumns.cpp		\
	    DebugHelpers.cpp		\
	    DelayedRebuilder.cpp	\
	    DirInfo.cpp			\
	    DirReadJob.cpp		\
	    DirSaver.cpp		\
	    DirTree.cpp			\
	    DirTreeCache.cpp		\
	    DirTreeModel.cpp		\
	    DirTreePatternFilter.cpp	\
	    DirTreePkgFilter.cpp	\
	    DirTreeView.cpp		\
	    DotEntry.cpp		\
	    DpkgPkgManager.cpp		\
	    Exception.cpp		\
	    ExcludeRules.cpp		\
	    ExcludeRulesConfigPage.cpp	\
	    ExistingDirCompleter.cpp	\
            ExistingDirValidator.cpp	\
            FileAgeStats.cpp		\
            FileAgeStatsWindow.cpp      \
	    FileDetailsView.cpp		\
	    FileInfo.cpp		\
	    FileInfoIterator.cpp	\
	    FileInfoSet.cpp		\
	    FileInfoSorter.cpp		\
	    FileSizeLabel.cpp		\
	    FileMTimeStats.cpp		\
	    FileSizeStats.cpp		\
	    FileSizeStatsWindow.cpp	\
	    FileSystemsWindow.cpp	\
	    FileTypeStats.cpp		\
	    FileTypeStatsWindow.cpp	\
	    GeneralConfigPage.cpp	\
	    HeaderTweaker.cpp		\
	    HistogramDraw.cpp		\
	    HistogramItems.cpp		\
	    HistogramOverflowPanel.cpp	\
            HistogramView.cpp		\
            History.cpp                 \
	    ListEditor.cpp		\
	    LocateFilesWindow.cpp	\
	    LocateFileTypeWindow.cpp	\
	    Logger.cpp			\
	    MainWindow.cpp		\
	    MessagePanel.cpp		\
	    MimeCategorizer.cpp		\
	    MimeCategory.cpp		\
	    MimeCategoryConfigPage.cpp	\
	    MountPoints.cpp		\
	    OpenDirDialog.cpp		\
	    OpenPkgDialog.cpp		\
	    OutputWindow.cpp		\
	    PacManPkgManager.cpp	\
	    PanelMessage.cpp		\
	    PathSelector.cpp		\
	    PercentBar.cpp		\
	    PercentileStats.cpp		\
	    PkgFileListCache.cpp	\
	    PkgFilter.cpp		\
	    PkgInfo.cpp			\
	    PkgManager.cpp		\
	    PkgQuery.cpp		\
	    PkgReader.cpp		\
	    PopupLabel.cpp		\
	    Process.cpp			\
	    ProcessStarter.cpp		\
	    Refresher.cpp		\
	    RpmPkgManager.cpp		\
	    SelectionModel.cpp		\
	    Settings.cpp		\
	    SettingsHelpers.cpp		\
	    ShowUnpkgFilesDialog.cpp	\
	    SizeColDelegate.cpp		\
            StdCleanup.cpp		\
	    Subtree.cpp			\
	    SysUtil.cpp			\
	    SystemFileChecker.cpp	\
	    Trash.cpp			\
	    TreemapTile.cpp		\
	    TreemapView.cpp		\
            TreeWalker.cpp              \
	    UnpkgSettings.cpp		\
	    UnreadableDirsWindow.cpp


HEADERS	  =				\
	    ActionManager.h		\
	    AdaptiveTimer.h		\
	    Attic.h			\
	    BreadcrumbNavigator.h	\
	    BucketsTableModel.h		\
	    BusyPopup.h			\
	    Cleanup.h			\
	    CleanupCollection.h		\
	    CleanupConfigPage.h		\
	    ConfigDialog.h		\
	    DataColumns.h		\
	    DebugHelpers.h		\
	    DelayedRebuilder.h		\
	    DirInfo.h			\
	    DirReadJob.h		\
	    DirSaver.h			\
	    DirTree.h			\
	    DirTreeCache.h		\
	    DirTreeFilter.h		\
	    DirTreeModel.h		\
	    DirTreePatternFilter.h	\
	    DirTreePkgFilter.h		\
	    DirTreeView.h		\
	    DotEntry.h			\
	    DpkgPkgManager.h		\
	    Exception.h			\
	    ExcludeRules.h		\
	    ExcludeRulesConfigPage.h	\
	    ExistingDirCompleter.h	\
	    ExistingDirValidator.h	\
            FileAgeStats.h		\
            FileAgeStatsWindow.h        \
	    FileDetailsView.h		\
	    FileInfo.h			\
	    FileInfoIterator.h		\
	    FileInfoSet.h		\
	    FileInfoSorter.h		\
	    FileSizeLabel.h		\
	    FileMTimeStats.h		\
	    FileSizeStats.h		\
	    FileSizeStatsWindow.h	\
	    FileTypeStats.h		\
	    FileSystemsWindow.h		\
	    FileTypeStatsWindow.h	\
	    GeneralConfigPage.h		\
	    HeaderTweaker.h		\
	    HistogramItems.h		\
	    HistogramView.h		\
            History.h                   \
	    ListEditor.h		\
	    ListMover.h			\
	    LocateFilesWindow.h         \
	    LocateFileTypeWindow.h	\
	    Logger.h			\
	    MainWindow.h		\
	    MessagePanel.h		\
	    MimeCategorizer.h		\
	    MimeCategory.h		\
	    MimeCategoryConfigPage.h	\
	    MountPoints.h		\
	    OpenDirDialog.h		\
	    OpenPkgDialog.h		\
	    OutputWindow.h		\
	    PacManPkgManager.h		\
	    PanelMessage.h		\
	    PathSelector.h		\
	    PercentBar.h		\
	    PercentileStats.h		\
	    PkgFileListCache.h		\
	    PkgFilter.h			\
	    PkgInfo.h			\
	    PkgManager.h		\
	    PkgQuery.h			\
	    PkgReader.h			\
	    PopupLabel.h		\
	    Process.h			\
	    ProcessStarter.h		\
	    Qt4Compat.h			\
	    Refresher.h			\
	    RpmPkgManager.h		\
	    SelectionModel.h		\
	    Settings.h			\
	    SettingsHelpers.h		\
	    ShowUnpkgFilesDialog.h	\
	    SignalBlocker.h		\
	    SizeColDelegate.h		\
	    StdCleanup.h		\
	    Subtree.h			\
	    SysUtil.h			\
	    SystemFileChecker.h		\
	    Trash.h			\
	    TreemapTile.h		\
            TreemapView.h		\
            TreeWalker.h                \
	    UnpkgSettings.cpp		\
	    UnreadableDirsWindow.h	\
	    Version.h



FORMS	  = main-window.ui		   \
	    output-window.ui		   \
	    config-dialog.ui		   \
	    cleanup-config-page.ui	   \
	    general-config-page.ui	   \
	    mime-category-config-page.ui   \
	    exclude-rules-config-page.ui   \
            file-age-stats-window.ui       \
	    file-size-stats-window.ui	   \
            file-type-stats-window.ui	   \
	    filesystems-window.ui	   \
	    locate-files-window.ui	   \
	    locate-file-type-window.ui	   \
	    open-dir-dialog.ui		   \
	    open-pkg-dialog.ui		   \
	    show-unpkg-files-dialog.ui	   \
	    file-details-view.ui	   \
	    message-panel.ui		   \
	    panel-message.ui		   \
	    unreadable-dirs-window.ui



RESOURCES = icons.qrc

desktop.files	= *.desktop
desktop.path	= $$INSTALL_PREFIX/share/applications

icons.files	= icons/qdirstat.svg
icons.path	= $$INSTALL_PREFIX/share/icons/hicolor/scalable/apps


mac:ICON	= icons/qdirstat.icns

# Regenerate this from the .png file with
#   sudo apt install icnsutils
#   png2icns qdirstat.icns qdirstat.png

