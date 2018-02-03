# qmake .pro file for qdirstat/doc

isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr

TEMPLATE   = app
TARGET     = $(nothing)
doc.files  = *.txt *.md ../*.md

# Ubuntu / Debian pkg doc path
doc.path = $$INSTALL_PREFIX/share/doc/qdirstat

exists( /usr/share/doc/packages ) {
    # SUSE pkg doc path
    doc.path = $$INSTALL_PREFIX/share/doc/packages/qdirstat

    # SUSE wants the license installed for each package
    doc.files += ../LICENSE
}

INSTALLS  += doc
