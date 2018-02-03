# Toplevel qmake .pro file. Create a Makefile from this with
#
#     qmake
#
# Then build the program with
#
#     make
#
# Then install it with
#
#     sudo make install
#
# By default, everyting is installed to /usr.
# If you want to install to, say, /usr/local, set INSTALL_PREFIX:
#
#     qmake INSTALL_PREFIX=/usr/local

TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS  = src scripts doc doc/stats man
