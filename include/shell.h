#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define MAX_JOBS 100
#define MAX_CMD_LEN 1024
#define MAX_ARGS 100

typedef struct {
    pid_t pid;
    char cmd[MAX_CMD_LEN];
} Job;

extern Job jobs[MAX_JOBS];
extern int job_count;

void init_shell();
void trim(char *str);
void reap_background_jobs();
void execute_command(char *cmd_line);

#endif
