#include "shell.h"

#define MAX_VARS 100

typedef struct {
    char name[64];
    char value[256];
} Variable;

static Variable vars[MAX_VARS];
static int var_count = 0;

// ---------- Initialization ----------
void init_shell() {
    printf("\nWelcome to My Custom Shell (Feature 8: Variables)\n");
    printf("Type 'set' to view variables or 'exit' to quit.\n\n");
}

// ---------- Utility ----------
void trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

// ---------- Variable Handling ----------
void set_variable(const char *name, const char *value) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            strncpy(vars[i].value, value, sizeof(vars[i].value));
            return;
        }
    }
    if (var_count < MAX_VARS) {
        strncpy(vars[var_count].name, name, sizeof(vars[var_count].name));
        strncpy(vars[var_count].value, value, sizeof(vars[var_count].value));
        var_count++;
    } else {
        fprintf(stderr, "Variable limit reached.\n");
    }
}

const char *get_variable(const char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            return vars[i].value;
        }
    }
    return "";
}

void print_all_variables() {
    printf("\n--- Stored Variables ---\n");
    for (int i = 0; i < var_count; i++) {
        printf("%s = %s\n", vars[i].name, vars[i].value);
    }
    if (var_count == 0) printf("(No variables set)\n");
    printf("------------------------\n");
}

char *expand_vars_in_string(const char *input) {
    static char expanded[1024];
    expanded[0] = '\0';

    const char *p = input;
    while (*p) {
        if (*p == '$') {
            p++;
            char varname[64] = {0};
            int i = 0;
            while (*p && (isalnum(*p) || *p == '_') && i < 63) {
                varname[i++] = *p++;
            }
            const char *val = get_variable(varname);
            strcat(expanded, val);
        } else {
            int len = strlen(expanded);
            expanded[len] = *p;
            expanded[len + 1] = '\0';
            p++;
        }
    }
    return strdup(expanded);
}

void reap_background_jobs() {
    // Optional future feature
}

int handle_if_block(char *line) {
    // Optional for next features
    return 0;
}
