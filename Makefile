# Makefile for identity project (build artifacts in build/)

# Detect OS for platform-specific settings
UNAME_S := $(shell uname -s)
RPATH_GSDK_REL = ../gsdk/build/lib
ifeq ($(UNAME_S),Darwin)
	SHARED_EXT = dylib
	RPATH_FLAGS = -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/$(RPATH_GSDK_REL)
else
	SHARED_EXT = so
	RPATH_FLAGS = -Wl,-rpath,'$$ORIGIN' -Wl,-rpath,'$$ORIGIN/$(RPATH_GSDK_REL)'
endif

# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -Iinclude -Igsdk/include -Igsdk/include/core -Igsdk/include/data -Igsdk/include/performance -Igsdk/include/reflection -std=c23 -g

# Directories
BUILD_DIR = build
GSDK_LIB_DIR = gsdk/build/lib
IDENTITY_SRC_DIR = src

# Sources / objects
IDENTITY_SRC = $(wildcard $(IDENTITY_SRC_DIR)/*.c)
IDENTITY_OBJ = $(patsubst $(IDENTITY_SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(IDENTITY_SRC))

# Library / executables (in build/)
IDENTITY_LIB_BASENAME = identity
IDENTITY_LIB = $(BUILD_DIR)/lib$(IDENTITY_LIB_BASENAME).$(SHARED_EXT)
SERVER = $(BUILD_DIR)/identity_server
CLIENT = $(BUILD_DIR)/identity_client

# Locate gsdk shared libraries (full paths)
GSDK_LIBS = $(wildcard $(GSDK_LIB_DIR)/*.$(SHARED_EXT))

# Default target
all: $(IDENTITY_LIB) $(SERVER) $(CLIENT)

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Object files
$(BUILD_DIR)/%.o: $(IDENTITY_SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

# Shared library
$(IDENTITY_LIB): $(IDENTITY_OBJ)
	$(CC) -shared -o $@ $^ $(GSDK_LIBS)

# Executables
$(SERVER): identity_server.c $(IDENTITY_LIB)
	$(CC) $(CFLAGS) -o $@ $< $(IDENTITY_LIB) $(GSDK_LIBS) $(RPATH_FLAGS)

$(CLIENT): identity_client.c $(IDENTITY_LIB)
	$(CC) $(CFLAGS) -o $@ $< $(IDENTITY_LIB) $(GSDK_LIBS) $(RPATH_FLAGS)

# Info
info:
	@echo "identity sources : $(IDENTITY_SRC)"
	@echo "identity objects : $(IDENTITY_OBJ)"
	@echo "identity library : $(IDENTITY_LIB)"
	@echo "gsdk libraries : $(GSDK_LIBS)"
	@echo "server executable : $(SERVER)"
	@echo "client exec    : $(CLIENT)"

# Clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean info
