# -----------------------------------------------------------
# Makefile for OS Assignment 03 - Base Shell
# -----------------------------------------------------------

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -Iinclude

# Directory structure
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

# Target executable
TARGET = $(BIN_DIR)/myshell

# Source and object files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/shell.c $(SRC_DIR)/execute.c
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/shell.o $(OBJ_DIR)/execute.o

# Default rule
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "✅ Build complete! Run with ./bin/myshell"

# Rule to compile .c files into .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "🧹 Cleaned up all build files."

# Run rule
run: all
	./bin/myshell

# Phony targets (not actual files)
.PHONY: all clean run
