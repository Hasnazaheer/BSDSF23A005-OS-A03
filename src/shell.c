#define _GNU_SOURCE
#include "shell.h"
#include <fcntl.h>
#include <errno.h>

/* ----------------- readline wrapper ----------------- */
char *read_line_rl(void) {
    char *line = readline("myshell> ");
    return line; /* readline returns malloc'd string or NULL on EOF */
}

/* ----------------- tokenizer -----------------
   Break a single command into tokens, preserving token order.
   Returns a malloc'd argv (NULL terminated). Caller must not free tokens themselves
   (they are pointers into the line buffer), but free the argv array.
*/
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

/* ----------------- parsing pipeline -----------------
   Input line is a full command like:
     cmd1 arg1 < in | cmd2 arg2 > out
   We parse by splitting on '|' first (but keeping original substrings),
   then for each part, parse tokens and detect '<' and '>' and the following filename.
*/
command_t *parse_pipeline(char *line, int *out_count) {
    if (!line) { *out_count = 0; return NULL; }

    /* Duplicate line because strtok/routines will modify substrings */
    char *line_dup = strdup(line);
    if (!line_dup) { *out_count = 0; return NULL; }

    /* First, count number of pipeline stages (split on '|') */
    int max_stages = 16;
    char **stages = malloc(max_stages * sizeof(char*));
    int stage_count = 0;

    char *saveptr = NULL;
    char *stage = strtok_r(line_dup, "|", &saveptr);
    while (stage != NULL) {
        /* trim leading/trailing spaces of stage */
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

    /* Allocate command_t array */
    command_t *cmds = calloc(stage_count, sizeof(command_t));
    if (!cmds) {
        for (int i = 0; i < stage_count; ++i) free(stages[i]);
        free(stages);
        *out_count = 0;
        return NULL;
    }

    /* For each stage parse tokens and detect < and > */
    for (int i = 0; i < stage_count; ++i) {
        cmds[i].infile = NULL;
        cmds[i].outfile = NULL;
        cmds[i].args = NULL;

        /* Create a working copy because tokenize modifies string */
        char *work = strdup(stages[i]);
        if (!work) continue;

        /* manual token scan to handle < and > */
        char *tok;
        int argv_cap = 16, argv_n = 0;
        char **argv = malloc(argv_cap * sizeof(char*));
        if (!argv) { free(work); continue; }

        tok = strtok(work, TOKEN_DELIM);
        while (tok != NULL) {
            if (strcmp(tok, "<") == 0) {
                /* next token should be infile */
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
            } else {
                /* normal argument */
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

/* free command_t array (frees strings and arrays) */
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

/* ----------------- builtins (reuse earlier features) ----------------- */
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
        printf("Job control not yet implemented.\n");
        return 1;
    }

    if (strcmp(args[0], "history") == 0) {
        HIST_ENTRY **hist_list = history_list();
        if (!hist_list) { printf("No history.\n"); return 1; }
        int idx = history_length; /* history_length is a variable, not a function */

        for (int i = 0; i < idx; ++i) {
            printf("%4d  %s\n", i+1, hist_list[i]->line);
        }
        return 1;
    }

    return 0; /* not handled */
}
