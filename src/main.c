#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <cjson/cJSON.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

char *command = NULL;
FILE *archive = NULL;
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

void cd_command(char *path) {
    if (path == NULL || strcmp(path, "") == 0) {
        path = getenv("HOME");

    }else if (strcmp(path, "-") == 0) {
        path = getenv("OLDPWD");
    }

    if (chdir(path) != 0) {
        perror("cd error");
    } else {
        setenv("OLDPWD", getenv("PWD"), 1);
        setenv("PWD", path, 1);
    }
}

void echo_command(char *msj) {
    if(msj[0] == '$'){
         
        const char *value = getenv(msj+1);
        if (value != NULL) {
            printf("%s\n", value);
        } else {
            printf("La variable de entorno %s no está definida.\n", msj+1);
        }
    }else{
       
        printf("%s ", msj);
        printf("\n");
    }
}

void quit_command(){

    if (command != NULL) {
        free(command);
        command = NULL;
        printf("Memoria dinámica liberada.\n");
    }
    if (archive != NULL) {
        fclose(archive);
        archive = NULL;
        printf("Archivo cerrado.\n");
    }
}

void execute_external_command(char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        // Proceso hijo
        if (execvp(args[0], args) == -1) {
            perror("execvp error");
        }
        exit(EXIT_FAILURE); // Salir si execvp falla
    } else {
        // Proceso padre
        int status;
        waitpid(pid, &status, 0); // Esperar a que el hijo termine
    }
}


void command_select(char *input) {

    char *args[128];
    char *token = strtok(input, " \n");
    int arg_count = 0;

    while (token != NULL) {
        args[arg_count++] = token;
        token = strtok(NULL, " \n");
    }

    args[arg_count] = NULL;

    if (arg_count == 0) return;

    if (strcmp(args[0], "cd") == 0) {
        cd_command(args[1]);
    } else if (strcmp(args[0], "clr") == 0) {
        system("clear");
    } else if (strcmp(args[0], "echo") == 0) {
        echo_command(args[1]);
    } else if (strcmp(args[0], "quit") == 0) {
        quit_command();
        exit(0);
    } else {
        execute_external_command(args);
    }
}

int main() {
    
    size_t len = 0;
    while (1) {
        
        display_prompt();  // Mostrar el prompt

        ssize_t nread = getline(&command, &len, stdin);  // Leer el comando

        if (nread == -1) {
            break;  // Salir en caso de EOF o error
        }

        command_select(command);
    }

    free(command);  // Liberar la memoria asignada para 'command'
    return 0;
}


