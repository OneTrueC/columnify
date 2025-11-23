.POSIX:
.PHONY: clean install uninstall

VERSION = 0.1.1

PREFIX = /usr/local
BINDIR = ${PREFIX}/bin
MANDIR = ${PREFIX}/share/man/man1

INC = -I. -I/usr/include
DEF = -DVERSION=\"${VERSION}\"

CFLAGS = -Wall -Wextra -std=c99 -pedantic ${INC} -Os ${DEF}
LDFLAGS = -s

all: columnify

columnify: main.c
	${CC} ${CFLAGS} ${LDFLAGS} main.c -o $@

clean:
	rm -f columnify

install: columnify
	cp columnify ${BINDIR}/columnify
	sed columnify.1 -e "s/VERSION/v${VERSION}/" > "${MANDIR}/columnify.1"

uninstall:
	rm -f ${BINDIR}/columnify
	rm -f ${MANDIR}/columnify.1

