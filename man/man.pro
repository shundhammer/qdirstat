# qmake .pro file for qdirstat/man

isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr

TEMPLATE     = aux
TARGET       = man

MAN_SRC      = qdirstat.1                 \
               qdirstat-cache-writer.1

MAN_TARGET   = qdirstat.1.gz              \
               qdirstat-cache-writer.1.gz

MAN_PATH     = $$INSTALL_PREFIX/share/man/man1

man.files    = $$MAN_TARGET
man.path     = $$MAN_PATH
man.extra    = \
	gzip -k -f $$MAN_SRC; \
	install $$MAN_TARGET $(INSTALL_ROOT)$$MAN_PATH
# Not using the --keep and --force long options to support simplistic busybox gzip

# The INSTALL_ROOT environment variable is used for rpm builds in the spec file
# to allow for a build root that is separate from the system directories:
#
#   make install INSTALL_ROOT=%{buildroot}

INSTALLS    += man
QMAKE_CLEAN += $$MAN_TARGET
