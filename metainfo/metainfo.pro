# qmake .pro file for qdirstat/metainfo
#
# !!! This is NOT executed by default !!!

TEMPLATE     = aux
TARGET       = man

INSTALLS    += TARGET

appstream.files	= *.metainfo.xml
appstream.path	= $$INSTALL_PREFIX/share/metainfo
