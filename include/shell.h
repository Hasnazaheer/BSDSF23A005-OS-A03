#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_JOBS 100
#define MAX_ARGS 100
#define MAX_CMDS 50

typedef struct {
    pid_t pid;
    char cmdline[256];
    int running;
} Job;

typedef struct {
    char *args[MAX_ARGS];
    char *input_file;
    char *output_file;
    int background;
} Command;

/* function declarations */
void init_shell();
void reap_background_jobs();
void add_job(pid_t pid, const char *cmd);
void list_jobs();
void trim(char *str);
int execute_command(char *line);
int execute_pipeline(Command *cmds, int n);
int handle_if_block(const char *first_line);

#endif
