#include "shell.h"

/* forward */
void shell_loop(void);

int main(void) {
    /* initialize GNU readline's history */
    using_history();

    printf("Welcome to MyShell (Feature-5: I/O Redirection & Pipes)\n");
    shell_loop();
    return 0;
}

void shell_loop(void) {
    char *line = NULL;
    command_t *cmds = NULL;
    int ncmds = 0;

    while (1) {
        line = read_line_rl();
        if (line == NULL) { /* EOF */
            printf("\n");
            break;
        }

        /* skip empty / whitespace-only lines */
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') { free(line); continue; }

        /* expand !n or other history-handling if you had it - here we rely on readline history */
        add_history(line);

        /* parse pipeline */
        cmds = parse_pipeline(line, &ncmds);
        if (cmds == NULL) {
            free(line);
            continue;
        }

        /* if single stage and builtin, handle it in the shell process */
        if (ncmds == 1 && cmds[0].args && cmds[0].args[0]) {
            if (handle_builtin(cmds[0].args)) {
                free_commands(cmds, ncmds);
                free(line);
                continue;
            }
        }

        /* execute pipeline (handles forking etc) */
        execute_pipeline(cmds, ncmds);

        /* cleanup */
        free_commands(cmds, ncmds);
        free(line);
    }
}
