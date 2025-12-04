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

TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS  = src scripts doc doc/stats man

# Not used by default
#
# SUBDIRS += metainfo

macx {
    # FIXME: Prevent build failure because of missing main() (issue #131)
    # This is a pretty radical approach, and you won't get any of the scripts
    # or any of the documentation on MacOS X; but it works.
    #
    # If anybody with good MacOS X know-how has a better idea that is still
    # robust enough to prevent that same build failure to reappear, please open
    # a pull request.

    SUBDIRS = src
}
