CC = gcc
WFLAGS = -Wall -Wextra -g -pedantic
FLAGS_PKG = -Wall -Wextra

DESTDIR= /usr/bin

.DEFAULT_GOAL := all

all: src/perfmode.c
	mkdir -p bin
	$(CC) $(FLAGS) src/perfmode.c -o bin/perfmode
	chmod +x bin/perfmode

pkg: src/perfmode.c
	$(CC) $(FLAGS_PKG) src/perfmode.c -o perfmode
	chmod +x perfmode

install: perfmode
	cp perfmode $(DESTDIR)

gui: src/perfmode.c src/gui.c
	mkdir -p bin
	$(CC) $(WFLAGS) src/*.c -o bin/perfmode_gui `pkg-config --cflags --libs gtk4`