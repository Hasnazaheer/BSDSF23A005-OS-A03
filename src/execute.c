#include "shell.h"

int execute(char **args)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
            perror("myshell");
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("myshell");
    }
    else
    {
        waitpid(pid, &status, 0);
    }
    return 1;
}
