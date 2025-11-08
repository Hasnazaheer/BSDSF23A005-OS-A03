#include "shell.h"
#include <errno.h>

// Execute built-ins or external commands
int execute_command(char *line) {
    if (!line) return 1;
    trim(line);
    if (line[0] == '\0') return 1;

    Command cmd;
    memset(&cmd, 0, sizeof(Command));

    int i = 0;
    char *token = strtok(line, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        cmd.args[i++] = token;
        token = strtok(NULL, " ");
    }
    cmd.args[i] = NULL;

    // handle built-in commands
    if (handle_builtin(cmd.args)) return 1;

    // execute external command
    return execute_pipeline(&cmd, 1);
}

// Executes a pipeline of commands
int execute_pipeline(Command *cmds, int n) {
    if (n <= 0) return 1;

    // Single command case: already handled built-ins
    if (n == 1 && handle_builtin(cmds[0].args)) return 1;

    int (*pipes)[2] = NULL;
    if (n > 1) {
        pipes = malloc((n - 1) * sizeof(int[2]));
        for (int i = 0; i < n - 1; ++i) pipe(pipes[i]);
    }

    pid_t *pids = malloc(n * sizeof(pid_t));
    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i < n - 1) dup2(pipes[i][1], STDOUT_FILENO);

            if (pipes) {
                for (int k = 0; k < n - 1; ++k) {
                    close(pipes[k][0]);
                    close(pipes[k][1]);
                }
            }

            execvp(cmds[i].args[0], cmds[i].args);
            fprintf(stderr, "myshell: %s: %s\n", cmds[i].args[0], strerror(errno));
            exit(EXIT_FAILURE);
        } else {
            pids[i] = pid;
        }
    }

    if (pipes) {
        for (int k = 0; k < n - 1; ++k) {
            close(pipes[k][0]);
            close(pipes[k][1]);
        }
        free(pipes);
    }

    for (int i = 0; i < n; ++i) waitpid(pids[i], NULL, 0);
    free(pids);

    return 1;
}
