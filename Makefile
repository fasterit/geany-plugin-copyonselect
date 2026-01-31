# Makefile for Copy on Select Geany Plugin

# Plugin name
PLUGIN_NAME = copyonselect

# Source files
SRC_DIR = src
SRC = $(SRC_DIR)/$(PLUGIN_NAME).c

# Output
OUT = $(PLUGIN_NAME).so

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -fPIC -shared -Wl,--as-needed
PKG_CONFIG = pkg-config

# Geany plugin directory (can be overridden)
GEANY_PLUGIN_DIR ?= $(HOME)/.config/geany/plugins

# Get compiler and linker flags from pkg-config
# Get include paths from geany for geanyplugin.h header
# Link only against GDK and GLib to minimize dependencies (not full GTK+)
# glib-2 still pulls in pcre regardless :-/
GEANY_CFLAGS = $(shell $(PKG_CONFIG) --cflags geany 2>/dev/null || \
                       $(PKG_CONFIG) --cflags gdk-3.0 glib-2.0 2>/dev/null || \
                       $(PKG_CONFIG) --cflags gdk-2.0 glib-2.0)
GEANY_LIBS = $(shell $(PKG_CONFIG) --libs gdk-3.0 glib-2.0 2>/dev/null || \
                     $(PKG_CONFIG) --libs gdk-2.0 glib-2.0)

# Build target
all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(GEANY_CFLAGS) -o $@ $< $(GEANY_LIBS)

# Install plugin to user's Geany plugin directory
install: $(OUT)
	mkdir -p $(GEANY_PLUGIN_DIR)
	cp $(OUT) $(GEANY_PLUGIN_DIR)/

# Uninstall plugin
uninstall:
	rm -f $(GEANY_PLUGIN_DIR)/$(OUT)

# Clean build artifacts
clean:
	rm -f $(OUT)

# Help target
help:
	@echo "Geany Copy on Select Plugin - Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the plugin (default)"
	@echo "  install   - Install plugin to $(GEANY_PLUGIN_DIR)"
	@echo "  uninstall - Remove plugin from $(GEANY_PLUGIN_DIR)"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  GEANY_PLUGIN_DIR - Installation directory (default: ~/.config/geany/plugins)"
	@echo ""
	@echo "Example:"
	@echo "  make"
	@echo "  make install"
	@echo "  make GEANY_PLUGIN_DIR=/usr/local/lib/geany install"

.PHONY: all install uninstall clean help
