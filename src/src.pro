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

TARGET	             = qdirstat
TARGET.files         = qdirstat
TARGET.path          = /usr/bin
INSTALLS            += TARGET

SOURCES	  = main.cpp			\
	    DirInfo.cpp			\
	    DirReadJob.cpp		\
	    DirSaver.cpp		\
	    DirTreeCache.cpp		\
	    DirTree.cpp			\
	    DirTreeIterator.cpp		\
	    Exception.cpp		\
	    ExcludeRules.cpp		\
	    FileInfo.cpp		\
	    Logger.cpp			\
	    MainWindow.cpp		\


HEADERS	  =				\
	    DirInfo.h			\
	    DirReadJob.h		\
	    DirSaver.h			\
	    DirTreeCache.h		\
	    DirTree.h			\
	    DirTreeIterator.h		\
	    Exception.h			\
	    ExcludeRules.h		\
	    FileInfo.h			\
	    Logger.h			\
	    MainWindow.h		\
	    SignalBlocker.h		\



FORMS	  = main-window.ui

RESOURCES = icons.qrc



