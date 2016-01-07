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

TEMPLATE	     = app

QT		    += widgets
CONFIG		    += debug
DEPENDPATH	    += .
MOC_DIR		     = .moc
OBJECTS_DIR	     = .obj
LIBS		    += -lz

TARGET		     = qdirstat
TARGET.files	     = qdirstat
TARGET.path	     = /usr/bin
INSTALLS	    += TARGET

SOURCES	  = main.cpp			\
	    ActionManager.cpp		\
            ConfigDialog.cpp            \
            CleanupConfigPage.cpp       \
	    Cleanup.cpp			\
	    CleanupCollection.cpp	\
	    StdCleanup.cpp		\
	    DebugHelpers.cpp		\
	    DataColumns.cpp		\
	    DirInfo.cpp			\
	    DirReadJob.cpp		\
	    DirSaver.cpp		\
	    DirTree.cpp			\
	    DirTreeCache.cpp		\
	    DirTreeModel.cpp		\
	    DirTreeView.cpp		\
	    Exception.cpp		\
	    ExcludeRules.cpp		\
	    FileInfo.cpp		\
	    FileInfoIterator.cpp	\
	    FileInfoSorter.cpp		\
	    Logger.cpp			\
	    MainWindow.cpp		\
	    OutputWindow.cpp		\
	    PercentBar.cpp		\
	    Refresher.cpp		\
	    SelectionModel.cpp		\
	    SettingsHelpers.cpp		\
	    TreemapTile.cpp		\
	    TreemapView.cpp		\


HEADERS	  =				\
	    ActionManager.h		\
            ConfigDialog.h              \
            CleanupConfigPage.h         \
	    Cleanup.h			\
	    CleanupCollection.h		\
	    StdCleanup.h		\
	    DebugHelpers.h		\
	    DataColumns.h		\
	    DirInfo.h			\
	    DirReadJob.h		\
	    DirSaver.h			\
	    DirTree.h			\
	    DirTreeCache.h		\
	    DirTreeModel.h		\
	    DirTreeView.h		\
	    Exception.h			\
	    ExcludeRules.h		\
	    FileInfo.h			\
	    FileInfoIterator.h		\
	    FileInfoSorter.h		\
	    Logger.h			\
	    MainWindow.h		\
	    OutputWindow.h		\
	    PercentBar.h		\
	    Refresher.h			\
	    SelectionModel.h		\
	    SettingsHelpers.h		\
	    SignalBlocker.h		\
	    TreemapTile.h		\
	    TreemapView.h		\
	    Version.h			\



FORMS	  = main-window.ui		\
	    output-window.ui		\
	    config-dialog.ui		\
	    cleanup-config-page.ui	\
	    general-config-page.ui	\


RESOURCES = icons.qrc
