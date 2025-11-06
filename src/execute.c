#include "shell.h"

int execute_pipeline(Command *cmds, int n) {
    if (n <= 0) return 1;

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
