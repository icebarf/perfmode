CC = gcc
FLAGS_PKG = -Wall -Wextra -pedantic -std=gnu99 -Werror

DESTDIR:= /usr/local/bin

.DEFAULT_GOAL := pkg

pkg: src/perfmode.c
	$(CC) $(FLAGS_PKG) src/perfmode.c -o perfmode

dbg: src/perfmode.c
	$(CC) $(FLAGS_PKG) -g -Og src/perfmode.c -o perfmode
	chmod +x perfmode

install: perfmode
	chmod +x perfmode
	mv perfmode $(DESTDIR)
