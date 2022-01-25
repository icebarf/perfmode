CC = gcc
FLAGS_PKG = -Wall -Wextra -pedantic

DESTDIR= /usr/local/bin

.DEFAULT_GOAL := pkg

pkg: src/perfmode.c
	$(CC) $(FLAGS_PKG) src/perfmode.c -o perfmode
	chmod +x perfmode

install: perfmode
	mv perfmode $(DESTDIR)
