#include "shell.h"

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    init_shell();
    shell_loop();
    while (1) {
        reap_background_jobs();  // Clean finished jobs
        printf("myshell> ");
        fflush(stdout);

        nread = getline(&line, &len, stdin);
        if (nread == -1) {
            printf("\n");
            break;
        }

        if (line[nread - 1] == '\n')
            line[nread - 1] = '\0';

        if (strlen(line) == 0)
            continue;

        // Split by semicolon (;)
        char *cmd_str = strtok(line, ";");
        while (cmd_str != NULL) {
            trim(cmd_str);
            if (strlen(cmd_str) > 0)
                execute_command(cmd_str);
            cmd_str = strtok(NULL, ";");
        }
    }

    free(line);
    return 0;
}
