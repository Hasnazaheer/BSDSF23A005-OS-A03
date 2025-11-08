#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "shell.h"

// ---------------- History Storage ----------------
static char *history[HISTORY_SIZE];
static int history_count = 0;

void add_to_history(const char *line) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(line);
    } else {
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++)
            history[i - 1] = history[i];
        history[HISTORY_SIZE - 1] = strdup(line);
    }
}

void print_history() {
    for (int i = 0; i < history_count; i++)
        printf("%d %s\n", i + 1, history[i]);
}

char *get_history_command(int index) {
    if (index < 1 || index > history_count)
        return NULL;
    return history[index - 1];
}

// ---------------- Input & Parsing ----------------
char *read_line(void) {
    char *line = NULL;
    size_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

char **parse_line(char *line) {
    int bufsize = MAX_ARGS, position = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " \t\r\n\a");
    while (token != NULL) {
        tokens[position++] = token;
        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}

// ---------------- Built-in Commands ----------------
int handle_builtin(char **args) {
    if (args[0] == NULL) return 1;

    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting myshell...\n");
        exit(0);
    } 
    else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL)
            fprintf(stderr, "myshell: expected argument to \"cd\"\n");
        else if (chdir(args[1]) != 0)
            perror("myshell");
        return 1;
    } 
    else if (strcmp(args[0], "help") == 0) {
        printf("MyShell Built-in Commands:\n");
        printf("  cd <dir>   - Change directory\n");
        printf("  help       - Display this help\n");
        printf("  exit       - Exit shell\n");
        printf("  history    - Show command history\n");
        printf("  jobs       - Not implemented\n");
        return 1;
    } 
    else if (strcmp(args[0], "jobs") == 0) {
        printf("Job control not yet implemented.\n");
        return 1;
    } 
    else if (strcmp(args[0], "history") == 0) {
        print_history();
        return 1;
    }

    return 0; // Not a built-in
}
