#include <stdio.h>
#include <stdlib.h>
#include "shell.h"

int main() {
    char *line;
    char **args;
    int status = 1;

    printf("Welcome to MyShell (Feature-2: Built-in Commands)\n");

    while (status) {
        printf("myshell> ");
        line = read_line();          // Read user input
        args = parse_line(line);     // Tokenize into arguments

        if (args[0] == NULL) {
            free(line);
            free(args);
            continue; // Empty input, skip
        }

        // 🔹 Check if command is a built-in first
        if (!handle_builtin(args)) {
            execute(args); // External command if not built-in
        }

        free(line);
        free(args);
    }

    return 0;
}
