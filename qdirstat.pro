# -*- mode: makefile -*-
#
# Toplevel qmake .pro file. Create a Makefile from this with
#
#     qmake6
#
# Then build the program with
#
#     make
#
# Then install it with
#
#     sudo make install
#
# By default, everything is installed to /usr.
# If you want to install to, say, /usr/local, set INSTALL_PREFIX:
#
#     qmake6 INSTALL_PREFIX=/usr/local

!equals( QT_MAJOR_VERSION, 6 ) {
    message( "Trying to use Qt $${QT_VERSION}" )
    error( "This project rquires Qt 6!" )
}


TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS  = src scripts doc doc/stats man

# Not used by default
#
# SUBDIRS += metainfo
