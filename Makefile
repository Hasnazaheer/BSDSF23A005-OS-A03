CC = gcc
CFLAGS = -Wall -g -Iinclude
LDFLAGS = -lreadline

SRC = src/main.c src/shell.c src/execute.c
OBJ_DIR = obj
BIN_DIR = bin
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/shell.o $(OBJ_DIR)/execute.o
BIN = $(BIN_DIR)/myshell

all: dirs $(BIN)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean dirs
