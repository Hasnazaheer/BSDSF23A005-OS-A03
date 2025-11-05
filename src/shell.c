#include "shell.h"

static char *history[HISTORY_SIZE];
static int history_count = 0;

void add_to_history(const char *cmd)
{
    if (cmd == NULL || strlen(cmd) == 0) return;

    add_history(cmd);  // readline built-in
    if (history_count < HISTORY_SIZE)
    {
        history[history_count++] = strdup(cmd);
    }
    else
    {
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++)
            history[i - 1] = history[i];
        history[HISTORY_SIZE - 1] = strdup(cmd);
    }
}

const char *get_history_entry(int index)
{
    if (index < 1 || index > history_count)
        return NULL;
    return history[index - 1];
}

char **parse_line(char *line)
{
    int bufsize = MAX_ARGS, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, " \t\r\n");
    while (token != NULL)
    {
        tokens[position++] = token;
        token = strtok(NULL, " \t\r\n");
    }
    tokens[position] = NULL;
    return tokens;
}

int handle_builtin(char **args)
{
    if (args[0] == NULL)
        return 1;

    if (strcmp(args[0], "exit") == 0)
    {
        printf("Exiting MyShell...\n");
        exit(0);
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
            fprintf(stderr, "cd: expected argument\n");
        else if (chdir(args[1]) != 0)
            perror("cd");
        return 1;
    }
    else if (strcmp(args[0], "help") == 0)
    {
        printf("MyShell built-in commands:\n");
        printf("  cd <dir>     Change directory\n");
        printf("  help         Show this help\n");
        printf("  exit         Exit shell\n");
        printf("  jobs         Show job control message\n");
        printf("  history      Show recent commands\n");
        printf("  !n           Re-run nth command from history\n");
        return 1;
    }
    else if (strcmp(args[0], "jobs") == 0)
    {
        printf("Job control not yet implemented.\n");
        return 1;
    }
    else if (strcmp(args[0], "history") == 0)
    {
        for (int i = 0; i < history_count; i++)
            printf("%d %s\n", i + 1, history[i]);
        return 1;
    }
    return 0;
}

void shell_loop(void)
{
    char *line;
    char **args;

    while (1)
    {
        line = readline("myshell> ");
        if (!line) break;

        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        if (line[0] == '!')
        {
            int n = atoi(line + 1);
            const char *cmd = get_history_entry(n);
            if (cmd)
            {
                printf("%s\n", cmd);
                free(line);
                line = strdup(cmd);
            }
            else
            {
                printf("No such command in history.\n");
                free(line);
                continue;
            }
        }

        add_to_history(line);
        args = parse_line(line);

        if (!handle_builtin(args))
            execute(args);

        free(args);
        free(line);
    }
}
