CC = gcc
WFLAGS = -Wall -Wextra -pedantic
FLAGS_PKG = -Wall -Wextra -pedantic

DESTDIR= /usr/bin

.DEFAULT_GOAL := gui

pkg: src/perfmode.c
	$(CC) $(FLAGS_PKG) src/*.c -o perfmode
	chmod +x perfmode

install: perfmode
	cp perfmode $(DESTDIR)

gui: src/perfmode.c
	mkdir -p bin
	$(CC) $(WFLAGS) src/perfmode.c -o bin/perfmode
	chmod +x bin/perfmode
