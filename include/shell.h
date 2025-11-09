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

#define TOKEN_DELIM " \t\r\n"
#define MAX_TOKENS 128

/* Command structure for a single pipeline stage */
typedef struct {
    char **args;      /* NULL-terminated argv */
    char *infile;     /* filename for input redirection or NULL */
    char *outfile;    /* filename for output redirection or NULL */
    int background;   /* 1 if & at end */
} command_t;

/* Parser / shell API */
char *read_line_rl(void);
char **tokenize(char *line);
command_t *parse_pipeline(char *line, int *out_count);
void free_commands(command_t *cmds, int count);

/* Builtins */

int handle_builtin(char **args);  /* returns 1 if handled, 0 otherwise */
int handle_if_structure(char **lines, int n_lines); 
void check_bg_jobs(void);

/* Executor */
int execute_pipeline(command_t *cmds, int n);

#endif /* SHELL_H */
