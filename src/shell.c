#include "shell.h"

Job jobs[MAX_JOBS];
int job_count = 0;

void init_shell() {
    job_count = 0;
    signal(SIGCHLD, SIG_IGN); // Prevent zombies
}

void trim(char *str) {
    char *end;
    while (*str == ' ' || *str == '\t') str++;
    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t')) *end-- = '\0';
}

void reap_background_jobs() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < job_count; i++) {
            if (jobs[i].pid == pid) {
                printf("[Done] %s (PID=%d)\n", jobs[i].cmd, pid);
                for (int j = i; j < job_count - 1; j++)
                    jobs[j] = jobs[j + 1];
                job_count--;
                break;
            }
        }
    }
}
void shell_loop() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break; // Ctrl+D exits shell

        // Remove trailing newline
        input[strcspn(input, "\n")] = 0;

        // Split by ';' for command chaining
        char *command = strtok(input, ";");
        while (command != NULL) {
            trim(command);
            if (strlen(command) > 0)
                execute_command(command);
            command = strtok(NULL, ";");
        }

        // Clean up finished background jobs
        reap_background_jobs();
    }
}
