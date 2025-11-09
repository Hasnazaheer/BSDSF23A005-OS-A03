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
#define MAX_BG_JOBS 64

/* Command structure for a single pipeline stage */
typedef struct {
    char **args;      /* NULL-terminated argv */
    char *infile;     /* filename for input redirection or NULL */
    char *outfile;    /* filename for output redirection or NULL */
} command_t;

/* Background job structure */
typedef struct {
    pid_t pid;
    char *cmdline;
} bg_job_t;

/* Parser / shell API */
char *read_line_rl(void);
char **tokenize(char *line);
command_t *parse_pipeline(char *line, int *out_count);
void free_commands(command_t *cmds, int count);

/* Command chaining and background */
char **split_commands(char *line, int *count);
int is_background(char *cmd);

/* Builtins */
int handle_builtin(char **args);

/* Executor */
int execute_pipeline(command_t *cmds, int n);

/* Global background job list */
extern bg_job_t bg_jobs[MAX_BG_JOBS];
extern int bg_job_count;

#endif /* SHELL_H */
