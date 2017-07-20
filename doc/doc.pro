# qmake .pro file for qdirstat/doc

TEMPLATE   = app
TARGET     = $(nothing)
doc.files  = *.txt *.md ../*.md

# Ubuntu / Debian pkg doc path
doc.path = /usr/share/doc/qdirstat

exists( /usr/share/doc/packages ) {
    # SUSE pkg doc path
    doc.path = /usr/share/doc/packages/qdirstat

    # SUSE wants the license installed for each package
    doc.files += ../LICENSE
}

INSTALLS  += doc
