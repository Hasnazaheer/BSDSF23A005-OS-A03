#include "shell.h"

int execute_command(char *line) {
    if (line == NULL || strlen(line) == 0)
        return 0;

    // Split command into args
    char *args[64];
    int argc = 0;

    char *token = strtok(line, " ");
    while (token != NULL && argc < 63) {
        args[argc++] = token;
        token = strtok(NULL, " ");
    }
    args[argc] = NULL;

    if (args[0] == NULL)
        return 0;

    pid_t pid = fork();

    if (pid == 0) {
        execvp(args[0], args);
        perror("Command failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("Fork failed");
    }

    return 0;
}
