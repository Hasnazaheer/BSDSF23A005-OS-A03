#include <stdio.h>      // printf, snprintf, perror
#include <stdlib.h>     // exit, EXIT_FAILURE
#include <string.h>     // strlen, strcmp, strchr
#include <unistd.h>     // fork, execvp
#include <sys/wait.h>   // waitpid, WNOHANG
#include <signal.h>     // signal, SIGCHLD
#include "shell.h"      // Job struct, jobs array, job_count, trim

#define MAX_ARGS 64     // Maximum number of arguments per command

void execute_command(char *cmd_line) {
    char *args[MAX_ARGS];
    int background = 0;
    int i = 0;

    // Trim leading/trailing whitespace
    trim(cmd_line);
    if (strlen(cmd_line) == 0)
        return;

    // Check for background execution
    size_t len = strlen(cmd_line);
    if (cmd_line[len - 1] == '&') {
        background = 1;
        cmd_line[len - 1] = '\0';
        trim(cmd_line);
    }

    // Parse command with support for quoted arguments
    char *p = cmd_line;
    while (*p != '\0' && i < MAX_ARGS - 1) {
        // Skip spaces
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') break;

        if (*p == '"') {
            // Quoted string
            p++; // skip opening quote
            args[i++] = p;
            while (*p != '"' && *p != '\0') p++;
            if (*p == '"') *p++ = '\0'; // terminate and skip closing quote
        } else {
            // Non-quoted argument
            args[i++] = p;
            while (*p != ' ' && *p != '\t' && *p != '\0') p++;
            if (*p != '\0') *p++ = '\0';
        }
    }
    args[i] = NULL;

    if (args[0] == NULL)
        return;

    // Built-in commands
    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    if (strcmp(args[0], "jobs") == 0) {
        for (int j = 0; j < job_count; j++) {
            printf("[%d] PID=%d CMD=%s\n", j + 1, jobs[j].pid, jobs[j].cmd);
        }
        return;
    }

    // Fork and execute
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    } else if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        if (background) {
            printf("[Background] PID=%d CMD=%s\n", pid, args[0]);
            jobs[job_count].pid = pid;
            snprintf(jobs[job_count].cmd, sizeof(jobs[job_count].cmd), "%s", args[0]);
            job_count++;
        } else {
            // Wait for foreground command before moving to next command
            int status;
            waitpid(pid, &status, 0);
        }
    }
}
