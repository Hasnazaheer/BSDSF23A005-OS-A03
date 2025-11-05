#include "shell.h"
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int execute_pipeline(command_t *cmds, int n) {
    if (n <= 0) return 1;

    int (*pipes)[2] = NULL;
    pid_t *pids = malloc(n * sizeof(pid_t));
    if (!pids) {
        perror("malloc");
        return 1;
    }

    /* Create pipes if more than one command */
    if (n > 1) {
        pipes = malloc((n - 1) * sizeof(int[2]));
        if (!pipes) {
            perror("malloc");
            free(pids);
            return 1;
        }
        for (int i = 0; i < n - 1; ++i) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                free(pipes);
                free(pids);
                return 1;
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            /* ---- CHILD PROCESS ---- */

            /* If not first command, set stdin from previous pipe */
            if (i > 0) {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            /* If not last command, set stdout to next pipe */
            if (i < n - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            /* Close all pipes in child */
            if (pipes) {
                for (int k = 0; k < n - 1; ++k) {
                    close(pipes[k][0]);
                    close(pipes[k][1]);
                }
            }

            /* Handle input redirection */
            if (cmds[i].infile) {
                int fd = open(cmds[i].infile, O_RDONLY);
                if (fd < 0) {
                    perror(cmds[i].infile);
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDIN_FILENO) == -1) {
                    perror("dup2 infile");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }

            /* Handle output redirection */
            if (cmds[i].outfile) {
                int fd = open(cmds[i].outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror(cmds[i].outfile);
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("dup2 outfile");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }

            /* Empty command check */
            if (!cmds[i].args || !cmds[i].args[0])
                exit(EXIT_SUCCESS);

            /* Execute */
            execvp(cmds[i].args[0], cmds[i].args);
            fprintf(stderr, "myshell: %s: %s\n", cmds[i].args[0], strerror(errno));
            exit(EXIT_FAILURE);
        } else {
            /* ---- PARENT ---- */
            pids[i] = pid;

            /* Close previous pipe ends in parent */
            if (i > 0) {
                close(pipes[i - 1][0]);
                close(pipes[i - 1][1]);
            }
        }
    }

    /* Close all pipes after forking */
    if (pipes) {
        for (int k = 0; k < n - 1; ++k) {
            close(pipes[k][0]);
            close(pipes[k][1]);
        }
        free(pipes);
    }

    /* Wait for all children */
    int status;
    for (int i = 0; i < n; ++i) {
        waitpid(pids[i], &status, 0);
    }
    free(pids);
    return 1;
}
