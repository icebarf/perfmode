CC = gcc
FLAGS = -Wall -Wextra -g

.DEFAULT_GOAL := all

all: src/perfmode.c
	mkdir -p bin
	$(CC) $(FLAGS) src/perfmode.c -o bin/perfmode
	chmod +x bin/perfmode