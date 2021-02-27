# qmake .pro file for qdirstat/man

isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr

TEMPLATE     = aux
TARGET       = man                 

MAN_SRC      = qdirstat.1                 \
               qdirstat-cache-writer.1

MAN_TARGET   = qdirstat.1.gz              \
               qdirstat-cache-writer.1.gz

man.files    = $$MAN_TARGET
man.path     = $$INSTALL_PREFIX/share/man/man1
man.extra    = \
  gzip --keep --force $$MAN_SRC; \
  install $$MAN_TARGET $$INSTALL_PREFIX/share/man/man1

INSTALLS    += man
QMAKE_CLEAN += $$MAN_TARGET
