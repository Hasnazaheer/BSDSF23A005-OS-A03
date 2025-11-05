#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_ARGS 100
#define HISTORY_SIZE 20

// Function prototypes
void shell_loop(void);
char **parse_line(char *line);
int execute(char **args);
int handle_builtin(char **args);
void add_to_history(const char *cmd);
const char *get_history_entry(int index);

#endif
