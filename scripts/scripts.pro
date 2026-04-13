# qmake .pro file for qdirstat/scripts

isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr

TEMPLATE       = app
TARGET         = $(nothing)
QMAKE_STRIP    = /bin/true # prevent stripping the script(s)

scripts.path   = $$INSTALL_PREFIX/bin
scripts.files  = qdirstat-cache-writer

# Not automatically adding to keep dependencies down,
# otherwise the packaging purists will complain
# See https://github.com/shundhammer/qdirstat/pull/304
# scripts.files += rclone2qdirstat.py

INSTALLS      += scripts
