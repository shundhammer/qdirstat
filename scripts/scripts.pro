# qmake .pro file for qdirstat/scripts

isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr

TEMPLATE       = app
TARGET         = $(nothing)
QMAKE_STRIP    = /bin/true # prevent stripping the script(s)

scripts.files  = qdirstat-cache-writer
scripts.path   = $$INSTALL_PREFIX/bin

INSTALLS      += scripts
