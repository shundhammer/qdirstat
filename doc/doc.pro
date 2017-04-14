# qmake .pro file for qdirstat/doc

TEMPLATE   = app
TARGET     = $(nothing)
doc.files  = *.txt *.md ../*.md ../LICENSE

# Ubuntu / Debian pkg doc path
doc.path = /usr/share/doc/qdirstat

exists( /usr/share/doc/packages ) {
    # SUSE pkg doc path
    doc.path = /usr/share/doc/packages/qdirstat
}

INSTALLS  += doc
