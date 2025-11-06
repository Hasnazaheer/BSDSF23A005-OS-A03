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
