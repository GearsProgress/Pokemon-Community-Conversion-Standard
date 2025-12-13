# Makefile: build the bin2s utility

# Compiler and flags
CC      := gcc
CFLAGS  := -O2 -g -Wall -Wextra -DPACKAGE_STRING="\"bin2s PCCS 1.0\""

# Target binary
TARGET  := bin2s

# Source files
SRC     := bin2s.c

# Default rule: build the program
all: $(TARGET)

# Link object to produce executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Install rule (optional)
# Installs binary to PREFIX/bin (default /usr/local/bin)
PREFIX ?= /usr/local
install: $(TARGET)
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin/$(TARGET)

# Clean up build artifacts
clean:
	rm -f $(TARGET)

.PHONY: all install clean 
