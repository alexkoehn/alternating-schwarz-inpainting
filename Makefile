# Compiler
CC = gcc

# Compiler flags
CFLAGS = -c -Wall -pedantic

# Linker flags
LFLAGS = -lm

# Source and build directory
BUILD_DIR = build
SRC_DIR = src examples

# Executable
EXE = $(BUILD_DIR)/bin/inpainting_example

# Find source files in specified directories
SRC_FILES = $(shell find $(SRC_DIR) -name *.c)

# For each .c file, we want to have an object file in the build directory
OBJ_FILES = $(addprefix $(BUILD_DIR)/, $(SRC_FILES:.c=.o))

.PHONY: all clean prep_build

all: prep_build $(EXE)

prep_build:
	mkdir -p $(BUILD_DIR)/bin
	mkdir -p $(addprefix $(BUILD_DIR)/, $(SRC_DIR))

$(EXE): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LFLAGS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -r $(BUILD_DIR)

