# qmake .pro file for qdirstat/man

isEmpty(INSTALL_PREFIX):INSTALL_PREFIX = /usr

TEMPLATE     = app
TARGET       = $(nothing)

MAN_SRC      = $$files( *.1 )
MAN_TARGET   = $$MAN_SRC
MAN_TARGET  ~= s/.1$/.1.gz/g

# message(src:    $$MAN_SRC)
# message(target: $$MAN_TARGET)

man.files    = $$MAN_TARGET
man.commands = gzip --keep --force $$MAN_SRC
man.path     = $$INSTALL_PREFIX/share/man/man1

INSTALLS    += man
QMAKE_CLEAN += $$MAN_TARGET
