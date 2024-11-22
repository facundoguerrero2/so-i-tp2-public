#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <cjson/cJSON.h>
#include <limits.h>



void display_prompt() {
    char cwd[PATH_MAX];
    char *user = getenv("USER");
    char hostname[HOST_NAME_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return;
    }

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname() error");
        return;
    }

    printf("%s@%s:%s$ ", user, hostname, cwd);
    fflush(stdout);
}

int main() {
    while (1) {
        display_prompt();

        char command[1024];
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break; // EOF o error
        }

        // Aquí manejarás los comandos
        printf("Comando ingresado: %s", command);
    }
    return 0;
}
