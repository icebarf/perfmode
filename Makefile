CC = gcc
FLAGS = -Wall -Wextra -g
FLAGS_PKG = -Wall -Wextra -march=native

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