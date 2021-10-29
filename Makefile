CC = gcc
WFLAGS = -Wall -Wextra -g -pedantic
FLAGS_PKG = -Wall -Wextra -g -pedantic

DESTDIR= /usr/bin

.DEFAULT_GOAL := gui

pkg: src/perfmode.c
	$(CC) $(FLAGS_PKG) src/*.c -o perfmode `pkg-config --cflags --libs gtk4`
	chmod +x perfmode

install: perfmode
	cp perfmode $(DESTDIR)

gui: src/perfmode.c src/gui.c
	mkdir -p bin
	$(CC) $(WFLAGS) src/*.c -o bin/perfmode `pkg-config --cflags --libs gtk4`
	chmod +x bin/perfmode