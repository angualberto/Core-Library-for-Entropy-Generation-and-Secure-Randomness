# AGLE Library Makefile
# Cryptographic RNG and Secure Communication Library
# Build options: static library, shared library, and examples

# ============================================================================
# Compiler Configuration
# ============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O3 -fPIC -I$(INCLUDE_DIR)
LDFLAGS = -lssl -lcrypto
DEBUG_FLAGS = -g -O0 -DDEBUG

# Architecture detection for optimization
ARCH := $(shell uname -m)
SYSTEM := $(shell uname -s)

ifeq ($(ARCH),x86_64)
    CFLAGS += -march=native
endif

# ============================================================================
# Directory Structure
# ============================================================================

SRC_DIR = src
BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj
INCLUDE_DIR = include

# ============================================================================
# Files
# ============================================================================

AGLE_H = $(INCLUDE_DIR)/agle.h
AGLE_C = $(SRC_DIR)/agle.c
AGLE_OBJ = $(OBJ_DIR)/agle.o

EXAMPLES_C = examples/agle_examples.c
EXAMPLES_OBJ = $(OBJ_DIR)/agle_examples.o
EXAMPLES_BIN = $(BIN_DIR)/agle_examples

# Authentication Server (Security Hardened)
SERVER_C = servidor_auth.c
SERVER_OBJ = $(OBJ_DIR)/servidor_auth.o
SERVER_BIN = servidor_auth

STATIC_LIB = $(LIB_DIR)/libagle.a
SHARED_LIB = $(LIB_DIR)/libagle.so

# ============================================================================
# Default Target
# ============================================================================

.PHONY: all clean install help static shared examples server debug release

all: static shared examples server

# ============================================================================
# Directory Creation
# ============================================================================

$(BUILD_DIR) $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	@mkdir -p $@

# ============================================================================
# Static Library
# ============================================================================

$(AGLE_OBJ): $(AGLE_C) $(AGLE_H) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(AGLE_C) -o $@

static: $(LIB_DIR) $(AGLE_OBJ)
	ar rcs $(STATIC_LIB) $(AGLE_OBJ)
	@echo "✓ Static library built: $(STATIC_LIB)"

# ============================================================================
# Shared Library
# ============================================================================

shared: $(LIB_DIR) $(AGLE_OBJ)
	$(CC) $(CFLAGS) -shared -Wl,-soname,libagle.so -o $(SHARED_LIB) $(AGLE_OBJ) $(LDFLAGS)
	@echo "✓ Shared library built: $(SHARED_LIB)"

# ============================================================================
# Examples
# ============================================================================

$(EXAMPLES_OBJ): $(EXAMPLES_C) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(EXAMPLES_C) -o $@

examples: static $(EXAMPLES_OBJ) $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(EXAMPLES_BIN) $(EXAMPLES_OBJ) $(AGLE_OBJ) $(LDFLAGS)
	@echo "✓ Examples built: $(EXAMPLES_BIN)"

# ============================================================================
# Authentication Server (Security Hardened v2.0)
# ============================================================================

$(SERVER_OBJ): $(SERVER_C) $(AGLE_H) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SERVER_C) -o $@

server: $(AGLE_OBJ) $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_OBJ) $(AGLE_OBJ) $(LDFLAGS)
	@echo "✓ Authentication server built: $(SERVER_BIN)"
	@echo "  • Security features: constant-time crypto, rate limiting, strong passwords"
	@echo "  • Run with: ./$(SERVER_BIN) or ./iniciar_auth.sh"

# ============================================================================
# Debug Build
# ============================================================================

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all
	@echo "✓ Debug build complete"

# ============================================================================
# Release Build
# ============================================================================

release: CFLAGS += -O3 -DNDEBUG
release: clean all
	@echo "✓ Release build complete"
	@strip $(STATIC_LIB) 2>/dev/null || true
	@strip $(SHARED_LIB) 2>/dev/null || true

# ============================================================================
# Test / Run Examples
# ============================================================================

.PHONY: run test

run: examples
	$(EXAMPLES_BIN)

test: run

# ============================================================================
# Installation
# ============================================================================

PREFIX ?= /usr/local

.PHONY: install uninstall

install: static shared
	@echo "Installing AGLE to $(PREFIX)..."
	@mkdir -p $(PREFIX)/lib $(PREFIX)/include
	@cp $(STATIC_LIB) $(PREFIX)/lib/
	@cp $(SHARED_LIB) $(PREFIX)/lib/
	@cp $(AGLE_H) $(PREFIX)/include/
	@echo "✓ Installation complete"
	@echo "  Header: $(PREFIX)/include/agle.h"
	@echo "  Static:  $(PREFIX)/lib/libagle.a"
	@echo "  Shared:  $(PREFIX)/lib/libagle.so"

uninstall:
	@echo "Uninstalling AGLE from $(PREFIX)..."
	@rm -f $(PREFIX)/lib/libagle.a $(PREFIX)/lib/libagle.so
	@rm -f $(PREFIX)/include/agle.h
	@echo "✓ Uninstall complete"

# ============================================================================
# Cleaning
# ============================================================================

clean:
	@rm -rf $(BUILD_DIR)
	@echo "✓ Build directory cleaned"

distclean: clean
	@find . -name "*.o" -delete
	@find . -name "*.so" -delete
	@find . -name "*.a" -delete
	@echo "✓ All build artifacts removed"

# ============================================================================
# Information
# ============================================================================

.PHONY: info

info:
	@echo "================================"
	@echo "  AGLE Library Build System"
	@echo "================================"
	@echo "Compiler:   $(CC)"
	@echo "System:     $(SYSTEM)"
	@echo "Architecture: $(ARCH)"
	@echo "CFLAGS:     $(CFLAGS)"
	@echo "LDFLAGS:    $(LDFLAGS)"
	@echo ""
	@echo "Targets:"
	@echo "  make all       - Build everything (default)"
	@echo "  make static    - Build static library"
	@echo "  make shared    - Build shared library"
	@echo "  make examples  - Build examples"
	@echo "  make run/test  - Run examples"
	@echo "  make debug     - Debug build"
	@echo "  make release   - Optimized release build"
	@echo "  make install   - Install to system"
	@echo "  make clean     - Remove build directory"
	@echo "  make distclean - Remove all artifacts"
	@echo "  make help      - Show this message"
	@echo ""

help: info

# ============================================================================
# Code Quality / Linting (optional)
# ============================================================================

.PHONY: lint format

lint:
	@command -v cppcheck >/dev/null 2>&1 || { echo "cppcheck not found"; exit 1; }
	cppcheck --enable=all $(AGLE_C) $(EXAMPLES_C)

format:
	@command -v clang-format >/dev/null 2>&1 || { echo "clang-format not found"; exit 1; }
	clang-format -i $(AGLE_H) $(AGLE_C) $(EXAMPLES_C)

# ============================================================================
# Dependencies
# ============================================================================

.PHONY: check-deps

check-deps:
	@echo "Checking dependencies..."
	@pkg-config --exists openssl || { echo "Error: OpenSSL not found"; exit 1; }
	@echo "✓ OpenSSL found"
	@echo "✓ All dependencies satisfied"

# ============================================================================
# Verbose Output
# ============================================================================

ifdef VERBOSE
    MAKEFLAGS =
else
    MAKEFLAGS += --no-print-directory
endif

# ============================================================================
