# qmake .pro file for qdirstat/scripts

TEMPLATE       = app
TARGET         = $(nothing)
QMAKE_STRIP    = /bin/true # prevent stripping the script(s)

scripts.files  = qdirstat-cache-writer
scripts.path   = /usr/bin

INSTALLS      += scripts

