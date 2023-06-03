CC ?= gcc
CFLAGS += -Wall -Wextra -std=c2x

BIN := perfmode
PREFIX ?= /usr

.DEFAULT_GOAL := perfmode

$(BIN): src/perfmode.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(BIN)

install: $(BIN)
	install -Dm755 $< "$(DESTDIR)$(PREFIX)/bin/$<"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/$(BIN)"
