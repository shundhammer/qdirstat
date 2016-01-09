# qmake .pro file for qdirstat/doc

TEMPLATE   = app
TARGET     =
doc.files  = *.txt ../*.md ../LICENSE

# Ubuntu / Debian pkg doc path
doc.path = /usr/share/doc/qdirstat

exists( /usr/share/doc/packages ) {
    # SUSE pkg doc path
    doc.path = /usr/share/doc/packages/qdirstat
}

INSTALLS  += doc

