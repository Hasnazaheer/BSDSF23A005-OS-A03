#include "shell.h"

int main(void)
{
    printf("Welcome to MyShell (Feature-4: Readline Integration)\n");

    using_history();  // Initialize readline's internal history mechanism
    shell_loop();     // Enter main loop

    return 0;
}
