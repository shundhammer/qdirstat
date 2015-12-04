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

TARGET	  = qdirstat

SOURCES	  = main.cpp			\
	    DirInfo.cpp			\
	    DirSaver.cpp		\
            DirTreeIterator.cpp         \
	    FileInfo.cpp		\
	    Exception.cpp		\
	    Logger.cpp			\
	    MainWindow.cpp		\


#	    DirTree.cpp			\


HEADERS	  = \
	    DirInfo.h			\
	    DirSaver.h			\
	    DirTreeIterator.h		\
	    FileInfo.h			\
	    Exception.h			\
	    Logger.h			\
	    MainWindow.h		\
	    SignalBlocker.h		\


#	    DirTree.h			\

FORMS	  = main-window.ui

RESOURCES = icons.qrc
