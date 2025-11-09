#include "shell.h"

/* Forward declaration */
void shell_loop(void);

int main(void) {
    using_history();
    printf("Welcome to MyShell (Feature-6: Command Chaining & Background Execution)\n");
    shell_loop();
    return 0;
}

void shell_loop(void) {
    char *line = NULL;
    command_t *cmds = NULL;
    int ncmds = 0;

    while (1) {
        // Reap finished background jobs
        int status;
        pid_t pid;
        for (int i = 0; i < bg_job_count; ) {
            pid = waitpid(bg_jobs[i].pid, &status, WNOHANG);
            if (pid > 0) {
                printf("[myshell] Background job %d finished\n", bg_jobs[i].pid);
                free(bg_jobs[i].cmdline);
                for (int j = i; j < bg_job_count - 1; j++) bg_jobs[j] = bg_jobs[j+1];
                bg_job_count--;
            } else i++;
        }

        line = read_line_rl();
        if (!line) { printf("\n"); break; }

        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') { free(line); continue; }

        add_history(line);

        int ncmds_chain = 0;
        char **cmd_chain = split_commands(line, &ncmds_chain);

        for (int i = 0; i < ncmds_chain; ++i) {
            int background = is_background(cmd_chain[i]);

            cmds = parse_pipeline(cmd_chain[i], &ncmds);
            if (!cmds) continue;

            if (ncmds == 1 && cmds[0].args && cmds[0].args[0]) {
                if (handle_builtin(cmds[0].args)) {
                    free_commands(cmds, ncmds);
                    continue;
                }
            }

            if (background) {
                pid_t bg_pid = fork();
                if (bg_pid == 0) {
                    execute_pipeline(cmds, ncmds);
                    exit(0);
                } else if (bg_pid > 0) {
                    printf("[myshell] Background job %d started\n", bg_pid);
                    if (bg_job_count < MAX_BG_JOBS) {
                        bg_jobs[bg_job_count].pid = bg_pid;
                        bg_jobs[bg_job_count].cmdline = strdup(cmd_chain[i]);
                        bg_job_count++;
                    }
                } else perror("fork");
            } else {
                execute_pipeline(cmds, ncmds);
            }

            free_commands(cmds, ncmds);
        }

        for (int i = 0; i < ncmds_chain; i++) free(cmd_chain[i]);
        free(cmd_chain);
        free(line);
    }
}
