#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

// ---------- Function Prototypes ----------

// Shell initialization
void init_shell();
void trim(char *str);
void reap_background_jobs();

// Core command handling
int execute_command(char *line);
int handle_if_block(char *line);

// Variable handling
void set_variable(const char *name, const char *value);
const char *get_variable(const char *name);
void print_all_variables();
char *expand_vars_in_string(const char *input);

#endif
