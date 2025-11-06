#include "shell.h"

int main() {
    char *line;

    init_shell();

    while (1) {
        reap_background_jobs(); // reap zombies
        line = readline("myshell> ");
        if (!line) break;
        trim(line);

        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        /* --- handle exit --- */
        if (strcmp(line, "exit") == 0) {
            free(line);
            break;
        }

        /* --- handle if-then-else block --- */
        if (strncmp(line, "if", 2) == 0 && (line[2] == ' ' || line[2] == '\0')) {
            handle_if_block(line);
            free(line);
            continue;
        }

        /* --- handle normal commands (including pipes, redirection, etc.) --- */
        execute_command(line);
        free(line);
    }

    printf("Exiting shell...\n");
    return 0;
}

