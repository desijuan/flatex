# flatex

CC = gcc
CFLAGS = -g -Wall

PREFIX = /usr/local

flatex: flatex.c
	${CC} ${CFLAGS} -o $@ $<

clean:
	rm -f flatex

release: CFLAGS := -Wall -O2
release: clean flatex

install: release
	mv flatex ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/flatex

uninstall:
	rm ${PREFIX}/bin/flatex

.PHONY: clean release install uninstall
