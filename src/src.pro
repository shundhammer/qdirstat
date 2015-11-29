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

QT                  += widgets
CONFIG		    += debug
DEPENDPATH	    += .
MOC_DIR		     = .moc
OBJECTS_DIR	     = .obj

TARGET	  = qdirstat

SOURCES	  = main.cpp                    \
            MainWindow.cpp              \
            Logger.cpp

HEADERS   = MainWindow.h                \
            Logger.h                    \
            SignalBlocker.h

FORMS     = main-window.ui

RESOURCES = icons.qrc
