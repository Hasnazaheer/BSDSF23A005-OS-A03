#include "shell.h"

void execute_command(char *cmd_line) {
    char *args[MAX_ARGS];
    int background = 0;
    char *token;
    int i = 0;

    // Detect background execution (&)
    if (cmd_line[strlen(cmd_line) - 1] == '&') {
        background = 1;
        cmd_line[strlen(cmd_line) - 1] = '\0';
        trim(cmd_line);
    }

    // Tokenize command
    token = strtok(cmd_line, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
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
            waitpid(pid, NULL, 0);
        }
    }
}

