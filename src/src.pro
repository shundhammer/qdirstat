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
CONFIG		+= debug
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

SOURCES	  = main.cpp			\
	    ActionManager.cpp		\
	    AdaptiveTimer.cpp		\
	    Attic.cpp			\
	    BreadcrumbNavigator.cpp	\
	    BucketsTableModel.cpp	\
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
	    DirTreePkgFilter.cpp	\
	    DirTreeView.cpp		\
	    DotEntry.cpp		\
	    DpkgPkgManager.cpp		\
	    Exception.cpp		\
	    ExcludeRules.cpp		\
	    ExcludeRulesConfigPage.cpp	\
	    ExistingDirCompleter.cpp	\
	    ExistingDirValidator.cpp	\
	    FileDetailsView.cpp		\
	    FileInfo.cpp		\
	    FileInfoIterator.cpp	\
	    FileInfoSet.cpp		\
	    FileInfoSorter.cpp		\
	    FileSizeStats.cpp		\
	    FileSizeStatsWindow.cpp	\
	    FileTypeStats.cpp		\
	    FileTypeStatsWindow.cpp	\
	    GeneralConfigPage.cpp	\
	    HeaderTweaker.cpp		\
	    HistogramDraw.cpp		\
	    HistogramItems.cpp		\
	    HistogramOverflowPanel.cpp	\
	    HistogramView.cpp		\
	    ListEditor.cpp		\
	    LocateFilesWindow.cpp	\
	    Logger.cpp			\
	    MainWindow.cpp		\
	    MimeCategorizer.cpp		\
	    MimeCategory.cpp		\
	    MimeCategoryConfigPage.cpp	\
	    MountPoints.cpp		\
	    OpenPkgDialog.cpp		\
	    OutputWindow.cpp		\
	    PacManPkgManager.cpp	\
	    PercentBar.cpp		\
	    PkgFileListCache.cpp	\
	    PkgFilter.cpp		\
	    PkgInfo.cpp			\
	    PkgManager.cpp		\
	    PkgQuery.cpp		\
	    PkgReader.cpp		\
	    Process.cpp			\
	    ProcessStarter.cpp		\
	    Refresher.cpp		\
	    RpmPkgManager.cpp		\
	    SelectionModel.cpp		\
	    Settings.cpp		\
	    SettingsHelpers.cpp		\
	    ShowUnpkgFilesDialog.cpp	\
	    StdCleanup.cpp		\
	    Subtree.cpp			\
	    SysUtil.cpp			\
	    SystemFileChecker.cpp	\
	    Trash.cpp			\
	    TreemapTile.cpp		\
	    TreemapView.cpp		\


HEADERS	  =				\
	    ActionManager.h		\
	    AdaptiveTimer.h		\
	    Attic.h			\
	    BreadcrumbNavigator.h	\
	    BucketsTableModel.h		\
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
	    DirTreePkgFilter.h		\
	    DirTreeView.h		\
	    DotEntry.h			\
	    DpkgPkgManager.h		\
	    Exception.h			\
	    ExcludeRules.h		\
	    ExcludeRulesConfigPage.h	\
	    ExistingDirCompleter.h	\
	    ExistingDirValidator.h	\
	    FileDetailsView.h		\
	    FileInfo.h			\
	    FileInfoIterator.h		\
	    FileInfoSet.h		\
	    FileInfoSorter.h		\
	    FileSizeStats.h		\
	    FileSizeStatsWindow.h	\
	    FileTypeStats.h		\
	    FileTypeStatsWindow.h	\
	    GeneralConfigPage.h		\
	    HeaderTweaker.h		\
	    HistogramItems.h		\
	    HistogramView.h		\
	    ListEditor.h		\
	    ListMover.h			\
	    LocateFilesWindow.h		\
	    Logger.h			\
	    MainWindow.h		\
	    MimeCategorizer.h		\
	    MimeCategory.h		\
	    MimeCategoryConfigPage.h	\
	    MountPoints.h		\
	    OpenPkgDialog.h		\
	    OutputWindow.h		\
	    PacManPkgManager.h		\
	    PercentBar.h		\
	    PkgFileListCache.h		\
	    PkgFilter.h			\
	    PkgInfo.h			\
	    PkgManager.h		\
	    PkgQuery.h			\
	    PkgReader.h			\
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
	    StdCleanup.h		\
	    Subtree.h			\
	    SysUtil.h			\
	    SystemFileChecker.h		\
	    Trash.h			\
	    TreemapTile.h		\
	    TreemapView.h		\
	    Version.h			\



FORMS	  = main-window.ui		   \
	    output-window.ui		   \
	    config-dialog.ui		   \
	    cleanup-config-page.ui	   \
	    general-config-page.ui	   \
	    mime-category-config-page.ui   \
	    exclude-rules-config-page.ui   \
	    file-size-stats-window.ui	   \
	    file-type-stats-window.ui	   \
	    locate-files-window.ui	   \
	    open-pkg-dialog.ui		   \
	    show-unpkg-files-dialog.ui	   \
	    file-details-view.ui



RESOURCES = icons.qrc

desktop.files	= *.desktop
desktop.path	= $$INSTALL_PREFIX/share/applications

icons.files	= icons/qdirstat.svg
icons.path	= $$INSTALL_PREFIX/share/icons/hicolor/scalable/apps


mac:ICON	= icons/qdirstat.icns

# Regenerate this from the .png file with
#   sudo apt install icnsutils
#   png2icns qdirstat.icns qdirstat.png

