#include "shell.h"

int main() {
    char *line;
    init_shell();

    while (1) {
        line = readline("myshell> ");
        if (!line)
            break;

        trim(line);

        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        add_history(line);

        // Exit command
        if (strcmp(line, "exit") == 0) {
            free(line);
            break;
        }

        // Handle variable assignment: VAR=value
        char *eq = strchr(line, '=');
        if (eq && eq != line) {
            *eq = '\0';
            char *var = line;
            char *val = eq + 1;
            trim(var);
            trim(val);
            set_variable(var, val);
            free(line);
            continue;
        }

        // Handle "set" command
        if (strcmp(line, "set") == 0) {
            print_all_variables();
            free(line);
            continue;
        }

        // Expand variables before execution
        char *expanded = expand_vars_in_string(line);
        execute_command(expanded);
        free(expanded);
        free(line);
    }

    return 0;
}
