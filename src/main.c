#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

int main() {
    char *line;
    char **args;
    int status = 1;

    printf("Welcome to MyShell (Feature-3: History + Built-ins)\n");

    while (status) {
        printf("myshell> ");
        line = read_line();
        if (!line) continue;
        line[strcspn(line, "\n")] = 0; // remove trailing newline

        // ---------------- Handle !n ----------------
        if (line[0] == '!' && isdigit(line[1])) {
            int cmd_num = atoi(&line[1]);
            char *hist_cmd = get_history_command(cmd_num);
            if (hist_cmd) {
                free(line);
                line = strdup(hist_cmd);
                printf("%s\n", line); // show command being executed
            } else {
                printf("myshell: !%d: event not found\n", cmd_num);
                free(line);
                continue;
            }
        }

        // Add to history
        add_to_history(line);

        // Parse and execute
        args = parse_line(line);
        if (args[0] == NULL) {
            free(line);
            free(args);
            continue;
        }

        if (!handle_builtin(args))
            execute(args);

        free(line);
        free(args);
    }

    return 0;
}
