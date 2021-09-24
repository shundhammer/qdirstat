# qmake .pro file for qdirstat/doc/stats

isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr

TEMPLATE   = app
TARGET     = $(nothing)
doc.files  = *.txt *.md

# Ubuntu / Debian pkg doc path
doc.path = $$INSTALL_PREFIX/share/doc/qdirstat/stats

exists( /usr/share/doc/packages ) {
    # SUSE pkg doc path
    doc.path = $$INSTALL_PREFIX/share/doc/packages/qdirstat/stats
}

INSTALLS  += doc
