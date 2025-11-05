#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"

#define MAX_INPUT_SIZE 1024
#define MAX_TOKENS 64
#define TOKEN_DELIM " \t\r\n\a"

// Read a line of input from stdin
char *read_line(void) {
    char *line = NULL;
    size_t bufsize = 0; // getline allocates buffer automatically
    getline(&line, &bufsize, stdin);
    return line;
}

// Split line into tokens (arguments)
char **parse_line(char *line) {
    int bufsize = MAX_TOKENS, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKEN_DELIM);
    while (token != NULL) {
        tokens[position++] = token;
        token = strtok(NULL, TOKEN_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

// Handle built-in commands
int handle_builtin(char **args) {
    if (args[0] == NULL) return 1; // No command entered

    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting myshell...\n");
        exit(0);
    }

    else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "myshell: expected argument to \"cd\"\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("myshell");
            }
        }
        return 1;
    }

    else if (strcmp(args[0], "help") == 0) {
        printf("MyShell Built-in Commands:\n");
        printf("  cd <dir>   - Change current directory\n");
        printf("  help       - Display this help message\n");
        printf("  exit       - Exit the shell\n");
        printf("  jobs       - Placeholder (not yet implemented)\n");
        return 1;
    }

    else if (strcmp(args[0], "jobs") == 0) {
        printf("Job control not yet implemented.\n");
        return 1;
    }

    return 0; // Not a built-in
}
