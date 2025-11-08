#ifndef SHELL_H
#define SHELL_H

#define MAX_ARGS 100
#define HISTORY_SIZE 20

// Function prototypes
char *read_line(void);
char **parse_line(char *line);
int execute(char **args);
int handle_builtin(char **args);
void add_to_history(const char *line);
void print_history();
char *get_history_command(int index);

#endif
