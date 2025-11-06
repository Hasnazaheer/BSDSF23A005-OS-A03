CC = gcc
CFLAGS = -Wall -g -Iinclude
SRC = src/main.c src/shell.c src/execute.c
OBJ = $(SRC:src/%.c=obj/%.o)
BIN = bin/myshell

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lreadline

obj/%.o: src/%.c
	mkdir -p obj bin
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj bin
