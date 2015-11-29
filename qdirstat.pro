# Toplevel qmake .pro file. Create a Makefile from this with
#
#     qmake
#
# Then build the program with
#
#     make

TEMPLATE = subdirs
CONFIG  += ordered
SUBDIRS  = src
