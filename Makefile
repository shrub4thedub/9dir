CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lX11 -lm

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin
CONFIGDIR = /etc/9dir

TARGET = 9dir
SOURCES = 9dir.c theme.c icons.c dialogs.c config.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = theme.h icons.h dialogs.h config.h

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	install -d $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/9dir
	@echo "Installation complete. Run '9dir' to start the file manager."
	@echo "Optional: Create config file at ~/.config/9dir/config"

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/9dir
	rmdir $(DESTDIR)$(CONFIGDIR) 2>/dev/null || true

# Development targets
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)

# Create example config file  
config-example: $(TARGET)
	@./$(TARGET) --create-config 2>/dev/null || echo "Config will be created on first run"
	@echo "Example config will be created at ~/.config/vdir/config on first run"

help:
	@echo "9dir (Plan 9 vdir port) - Makefile targets:"
	@echo "  all          - Build 9dir (default)"
	@echo "  clean        - Remove build files"
	@echo "  install      - Install to $(PREFIX)"
	@echo "  uninstall    - Remove installed files"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release"
	@echo "  config-example - Create example config file"
	@echo "  help         - Show this help"