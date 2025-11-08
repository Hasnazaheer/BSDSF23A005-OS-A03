#include "shell.h"

// Simple initialization
void init_shell() {
    printf("Welcome to myshell (v7: if-then-else)\n");
}

// Stub for reaping background jobs
void reap_background_jobs() {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0);
}

// Trim whitespace
void trim(char *str) {
    if (!str) return;
    // trim leading
    while(*str && (*str == ' ' || *str == '\t')) memmove(str, str+1, strlen(str));
    // trim trailing
    int len = strlen(str);
    while(len > 0 && (str[len-1] == ' ' || str[len-1] == '\t')) str[--len] = '\0';
}

// Handle built-in commands
int handle_builtin(char **args) {
    if (!args[0]) return 0;

    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    if (strcmp(args[0], "cd") == 0) {
        if (!args[1]) {
            fprintf(stderr, "myshell: expected argument to \"cd\"\n");
        } else if (chdir(args[1]) != 0) {
            perror("myshell");
        }
        return 1;
    }

    if (strcmp(args[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("  cd <dir>\n  help\n  exit\n  jobs (stub)\n  history (stub)\n");
        return 1;
    }

    if (strcmp(args[0], "jobs") == 0 || strcmp(args[0], "history") == 0) {
        printf("myshell: %s: built-in not yet implemented.\n", args[0]);
        return 1;
    }

    return 0; // not a built-in
}

// Stub for if-then-else
int handle_if_block(const char *first_line) {
    printf("If-then-else block detected (stub, not fully implemented)\n");
    return 1;
}
