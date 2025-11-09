#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* forward declaration */
void shell_loop(void);

int main(void) {
    /* initialize GNU readline's history */
    using_history();

    printf("Welcome to MyShell (Features: I/O Redirection, Pipes, Multitasking, If-Then-Else)\n");
    shell_loop();
    return 0;
}

void shell_loop(void) {
    char *line = NULL;
    command_t *cmds = NULL;
    int ncmds = 0;

    while (1) {
        /* check and reap background jobs */
        check_bg_jobs();

        /* read input */
        line = read_line_rl();
        if (line == NULL) { /* EOF */
            printf("\n");
            break;
        }

        /* skip empty / whitespace-only lines */
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') { free(line); continue; }

        /* add line to history */
        add_history(line);

        /* ------------------ IF-THEN-ELSE-FI ------------------ */
        if (strncmp(p, "if", 2) == 0 && (p[2] == ' ' || p[2] == '\0')) {
            char *lines[64];  // max 64 lines per if-block
            int n_lines = 0;
            lines[n_lines++] = line; // first line "if ..."

            while (1) {
                char *next_line = read_line_rl();
                if (!next_line) break;

                lines[n_lines++] = next_line;

                // Stop when we reach "fi"
                char *trim = next_line;
                while (*trim == ' ' || *trim == '\t') trim++;
                if (strncmp(trim, "fi", 2) == 0 && (trim[2] == '\0' || trim[2] == ' ')) {
                    break;
                }

                if (n_lines >= 64) break; // prevent overflow
            }

            /* call if-block handler */
            handle_if_structure(lines, n_lines);

            /* free collected lines */
            for (int i = 0; i < n_lines; i++) free(lines[i]);
            continue;
        }

        /* ------------------ COMMAND CHAINING (;) ------------------ */
        char *cmd_str = strtok(line, ";");
        while (cmd_str != NULL) {
            /* parse pipeline */
            cmds = parse_pipeline(cmd_str, &ncmds);
            if (cmds == NULL) { 
                cmd_str = strtok(NULL, ";");
                continue; 
            }

            /* if single stage and builtin, handle it in the shell process */
            if (ncmds == 1 && cmds[0].args && cmds[0].args[0]) {
                if (handle_builtin(cmds[0].args)) {
                    free_commands(cmds, ncmds);
                    cmd_str = strtok(NULL, ";");
                    continue;
                }
            }

            /* execute pipeline (handles background & foreground) */
            execute_pipeline(cmds, ncmds);

            free_commands(cmds, ncmds);
            cmd_str = strtok(NULL, ";");
        }

        free(line);
    }
}
