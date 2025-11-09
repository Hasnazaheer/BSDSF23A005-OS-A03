#include "shell.h"
#include <fcntl.h>
#include <errno.h>

/* ----------------- readline wrapper ----------------- */
char *read_line_rl(void) {
    char *line = readline("myshell> ");
    return line; /* readline returns malloc'd string or NULL on EOF */
}

/* ----------------- tokenizer ----------------- */
char **tokenize(char *line) {
    int bufsize = MAX_TOKENS, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "myshell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOKEN_DELIM);
    while (token != NULL) {
        tokens[position++] = token;
        if (position >= bufsize) {
            bufsize *= 2;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "myshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TOKEN_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

/* ----------------- parsing pipeline ----------------- */
command_t *parse_pipeline(char *line, int *out_count) {
    if (!line) { *out_count = 0; return NULL; }

    char *line_dup = strdup(line);
    if (!line_dup) { *out_count = 0; return NULL; }

    int max_stages = 16;
    char **stages = malloc(max_stages * sizeof(char*));
    int stage_count = 0;

    char *saveptr = NULL;
    char *stage = strtok_r(line_dup, "|", &saveptr);
    while (stage != NULL) {
        while (*stage == ' ' || *stage == '\t') stage++;
        char *end = stage + strlen(stage) - 1;
        while (end > stage && (*end == ' ' || *end == '\t')) { *end = '\0'; end--; }

        stages[stage_count++] = strdup(stage);
        if (stage_count >= max_stages) {
            max_stages *= 2;
            stages = realloc(stages, max_stages * sizeof(char*));
        }
        stage = strtok_r(NULL, "|", &saveptr);
    }
    free(line_dup);

    if (stage_count == 0) {
        free(stages);
        *out_count = 0;
        return NULL;
    }

    command_t *cmds = calloc(stage_count, sizeof(command_t));
    if (!cmds) {
        for (int i = 0; i < stage_count; ++i) free(stages[i]);
        free(stages);
        *out_count = 0;
        return NULL;
    }

    for (int i = 0; i < stage_count; ++i) {
        cmds[i].infile = NULL;
        cmds[i].outfile = NULL;
        cmds[i].args = NULL;
        cmds[i].background = 0;

        char *work = strdup(stages[i]);
        if (!work) continue;

        char *tok;
        int argv_cap = 16, argv_n = 0;
        char **argv = malloc(argv_cap * sizeof(char*));
        if (!argv) { free(work); continue; }

        tok = strtok(work, TOKEN_DELIM);
        while (tok != NULL) {
            if (strcmp(tok, "<") == 0) {
                tok = strtok(NULL, TOKEN_DELIM);
                if (tok != NULL) {
                    cmds[i].infile = strdup(tok);
                } else {
                    fprintf(stderr, "myshell: syntax error near unexpected token `newline' after '<'\n");
                }
            } else if (strcmp(tok, ">") == 0) {
                tok = strtok(NULL, TOKEN_DELIM);
                if (tok != NULL) {
                    cmds[i].outfile = strdup(tok);
                } else {
                    fprintf(stderr, "myshell: syntax error near unexpected token `newline' after '>'\n");
                }
            } else if (strcmp(tok, "&") == 0) {
                cmds[i].background = 1; // mark background
            } else {
                if (argv_n + 1 >= argv_cap) {
                    argv_cap *= 2;
                    argv = realloc(argv, argv_cap * sizeof(char*));
                }
                argv[argv_n++] = strdup(tok);
            }
            tok = strtok(NULL, TOKEN_DELIM);
        }
        argv[argv_n] = NULL;
        cmds[i].args = argv;
        free(work);
    }

    for (int i = 0; i < stage_count; ++i) free(stages[i]);
    free(stages);

    *out_count = stage_count;
    return cmds;
}

/* free command_t array */
void free_commands(command_t *cmds, int count) {
    if (!cmds) return;
    for (int i = 0; i < count; ++i) {
        if (cmds[i].args) {
            for (int j = 0; cmds[i].args[j] != NULL; ++j) free(cmds[i].args[j]);
            free(cmds[i].args);
        }
        if (cmds[i].infile) free(cmds[i].infile);
        if (cmds[i].outfile) free(cmds[i].outfile);
    }
    free(cmds);
}

/* ----------------- builtins ----------------- */
int handle_builtin(char **args) {
    if (args == NULL || args[0] == NULL) return 1;

    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting myshell...\n");
        exit(0);
    }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "myshell: expected argument to \"cd\"\n");
        } else {
            if (chdir(args[1]) != 0) perror("myshell");
        }
        return 1;
    }

    if (strcmp(args[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("  cd <dir>\n  help\n  exit\n  jobs\n  history\n");
        return 1;
    }

    if (strcmp(args[0], "jobs") == 0) {
        HIST_ENTRY **hist_list = history_list();
        if (!hist_list) { printf("No history.\n"); return 1; }
        int idx = history_length;
        for (int i = 0; i < idx; ++i) {
            printf("%4d  %s\n", i+1, hist_list[i]->line);
        }
        return 1;
    }

    if (strcmp(args[0], "history") == 0) {
        HIST_ENTRY **hist_list = history_list();
        if (!hist_list) { printf("No history.\n"); return 1; }
        int idx = history_length;
        for (int i = 0; i < idx; ++i) {
            printf("%4d  %s\n", i+1, hist_list[i]->line);
        }
        return 1;
    }

    return 0;
}
int handle_if_structure(char **lines, int n_lines) {
    if (n_lines <= 0) return 1;

    int status = 0;

    // Execute the "if" line first
    if (lines[0] != NULL) {
        char *cmd_line = lines[0] + 3; // skip "if "
        command_t *cmds = parse_pipeline(cmd_line, &status);
        if (cmds) {
            execute_pipeline(cmds, status);
            free_commands(cmds, status);
            status = WEXITSTATUS(status);
        }
    }

    // Determine whether to execute then/else
    int start = 1, end = n_lines;
    int then_start = -1, else_start = -1, fi_index = -1;
    for (int i = 1; i < n_lines; i++) {
        if (strncmp(lines[i], "then", 4) == 0) then_start = i+1;
        if (strncmp(lines[i], "else", 4) == 0) else_start = i+1;
        if (strncmp(lines[i], "fi", 2) == 0) { fi_index = i; break; }
    }

    if (status == 0 && then_start != -1) {
        for (int i = then_start; i < (else_start != -1 ? else_start-1 : fi_index); i++) {
            command_t *cmds; int ncmds;
            cmds = parse_pipeline(lines[i], &ncmds);
            if (cmds) { execute_pipeline(cmds, ncmds); free_commands(cmds, ncmds); }
        }
    } else if (status != 0 && else_start != -1) {
        for (int i = else_start; i < fi_index; i++) {
            command_t *cmds; int ncmds;
            cmds = parse_pipeline(lines[i], &ncmds);
            if (cmds) { execute_pipeline(cmds, ncmds); free_commands(cmds, ncmds); }
        }
    }

    return status;
}
