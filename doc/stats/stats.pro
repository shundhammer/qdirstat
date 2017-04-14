# qmake .pro file for qdirstat/doc/stats

TEMPLATE   = app
TARGET     = $(nothing)
doc.files  = *.txt *.md

# Ubuntu / Debian pkg doc path
doc.path = /usr/share/doc/qdirstat/stats

exists( /usr/share/doc/packages ) {
    # SUSE pkg doc path
    doc.path = /usr/share/doc/packages/qdirstat/stats
}

INSTALLS  += doc

