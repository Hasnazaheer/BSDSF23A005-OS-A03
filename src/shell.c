#include "shell.h"

static Job jobs[MAX_JOBS];
static int job_count = 0;

void init_shell() {
    using_history();
    printf("Welcome to myshell (v7: if-then-else)\n");
}

void add_job(pid_t pid, const char *cmd) {
    if (job_count < MAX_JOBS) {
        jobs[job_count].pid = pid;
        strcpy(jobs[job_count].cmdline, cmd);
        jobs[job_count].running = 1;
        job_count++;
    }
}

void reap_background_jobs() {
    int status;
    for (int i = 0; i < job_count; ++i) {
        if (jobs[i].running) {
            pid_t result = waitpid(jobs[i].pid, &status, WNOHANG);
            if (result > 0) {
                jobs[i].running = 0;
                printf("[Done] %s (PID=%d)\n", jobs[i].cmdline, jobs[i].pid);
            }
        }
    }
}

void list_jobs() {
    for (int i = 0; i < job_count; ++i) {
        if (jobs[i].running)
            printf("[%d] Running: %s\n", jobs[i].pid, jobs[i].cmdline);
    }
}

void trim(char *str) {
    char *end;
    while (*str == ' ' || *str == '\t') str++;
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t')) *end-- = '\0';
}

/* ---------- execute_command() -------------- */
int execute_command(char *line) {
    char *commands[MAX_CMDS];
    int cmd_count = 0;
    char *token = strtok(line, ";");

    while (token && cmd_count < MAX_CMDS) {
        commands[cmd_count++] = token;
        token = strtok(NULL, ";");
    }

    for (int i = 0; i < cmd_count; i++) {
        char *cmd = commands[i];
        trim(cmd);

        if (strlen(cmd) == 0) continue;

        /* background execution */
        int background = 0;
        if (cmd[strlen(cmd)-1] == '&') {
            background = 1;
            cmd[strlen(cmd)-1] = '\0';
            trim(cmd);
        }

        pid_t pid = fork();
        if (pid == 0) {
            /* Child */
            execl("/bin/sh", "sh", "-c", cmd, NULL);
            perror("exec");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            if (background) {
                add_job(pid, cmd);
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
        } else {
            perror("fork");
        }
    }
    return 1;
}

/* ---------- handle_if_block() -------------- */
int handle_if_block(const char *first_line) {
    char *if_cmd = strdup(first_line + 2);
    trim(if_cmd);

    char *then_cmds[100], *else_cmds[100];
    int then_count = 0, else_count = 0;
    int in_then = 0, in_else = 0;

    char *line;
    while ((line = readline("> ")) != NULL) {
        trim(line);

        if (strcmp(line, "then") == 0) {
            in_then = 1;
        } else if (strcmp(line, "else") == 0) {
            in_then = 0;
            in_else = 1;
        } else if (strcmp(line, "fi") == 0) {
            free(line);
            break;
        } else if (in_then) {
            then_cmds[then_count++] = strdup(line);
        } else if (in_else) {
            else_cmds[else_count++] = strdup(line);
        }

        free(line);
    }

    /* Run the if command */
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", if_cmd, NULL);
        perror("exec if");
        exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    int exit_code = WEXITSTATUS(status);

    if (exit_code == 0) {
        for (int i = 0; i < then_count; ++i) {
            execute_command(then_cmds[i]);
            free(then_cmds[i]);
        }
    } else {
        for (int i = 0; i < else_count; ++i) {
            execute_command(else_cmds[i]);
            free(else_cmds[i]);
        }
    }

    free(if_cmd);
    return 1;
}
