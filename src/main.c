#include "shell.h"

int main() {
    char *line;

    init_shell();

    while (1) {
        reap_background_jobs(); // reap zombie processes

        line = readline("myshell> ");
        if (!line) break;

        trim(line);
        if (strlen(line) == 0) { free(line); continue; }

        // handle exit command directly
        if (strcmp(line, "exit") == 0) { free(line); break; }

        // handle if-then-else
        if (strncmp(line, "if", 2) == 0 && (line[2] == ' ' || line[2] == '\0')) {
            handle_if_block(line);
            free(line);
            continue;
        }

        // handle normal commands
        execute_command(line);
        free(line);
    }

    printf("Exiting shell...\n");
    return 0;
}
