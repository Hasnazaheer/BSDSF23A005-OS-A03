#ifndef SHELL_H
#define SHELL_H

// Function prototypes
char *read_line(void);
char **parse_line(char *line);
int execute(char **args);
int handle_builtin(char **args);

#endif
