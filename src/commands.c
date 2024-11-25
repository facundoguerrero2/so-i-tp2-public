#include "commands.h"

char* command = NULL;
FILE* batch_file = NULL;
int job_id = 1;
pid_t foreground_pid = -1;

void execute_pipeline(char *cmds[], int n) {
    int pipefd[2];//pipefd[0]: Extremo de lectura.
                  //pipefd[1]: Extremo de escritura.
    int prev_fd = 0; // Para guardar el extremo de lectura del pipe anterior

    for (int i = 0; i < n; i++) {
        pipe(pipefd); // Crear un nuevo pipe

        pid_t pid = fork();

        if (pid == -1) {
            perror("fork failed");
            return;
        }

        if (pid == 0) {
            // Proceso hijo
            dup2(prev_fd, STDIN_FILENO); // Redirigir entrada estándar
            if (i < n - 1) {
                dup2(pipefd[1], STDOUT_FILENO); // Redirigir salida estándar
            }
            close(pipefd[0]);
            char *args[128];
            char *token = strtok(cmds[i], " \n");
            int arg_count = 0;

            while (token != NULL) {
                args[arg_count++] = token;
                token = strtok(NULL, " \n");
            }
            args[arg_count] = NULL;

            if (execvp(args[0], args) == -1) {
                perror("execvp error");
                exit(EXIT_FAILURE);
            }
        } else {
            // Proceso padre
            wait(NULL);
            close(pipefd[1]);
            prev_fd = pipefd[0];
        }
    }
}

void cd_command(char* path)
{
    char cwd[PATH_MAX];
    // if user put only cd in the terminal
    if (path == NULL || strcmp(path, "") == 0)
    {
        path = getenv("HOME");
        // if user put cd - in the terminal
    }
    else if (strcmp(path, "-") == 0)
    { // REVISAR ESTE
        path = getenv("OLDPWD");
    }

    if (chdir(path) != 0)
    { // change path
        perror("cd error");
    }
    else
    {
        // change environmental variables
        setenv("OLDPWD", getenv("PWD"), 1);
        setenv("PWD", getcwd(cwd, sizeof(cwd)), 1);
    }
}

void echo_command(char* msj)
{
    // if user wants to print a environmental variable
    if (msj[0] == '$')
    {
        const char* value = getenv(msj + 1);
        if (value != NULL)
        {
            printf("%s\n", value); // printing variable
        }
        else
        {
            printf("La variable de entorno %s no está definida.\n", msj + 1);
        }
    }
    else
    {
        printf("%s ", msj); // printing message
        printf("\n");
    }
}

void quit_command()
{

    if (command != NULL)
    {
        free(command);
        command = NULL;
        printf("Memoria dinámica liberada.\n");
    }
    if (batch_file != NULL)
    {
        fclose(batch_file);
        batch_file = NULL;
        printf("Archivo cerrado.\n");
    }
}

void execute_external_command(char* args[], int background)
{
    // create a new process
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return;
    }

    if (pid == 0)
    {
        // child process
        if (execvp(args[0], args) == -1)
        {
            perror("execvp error");
        }
        exit(EXIT_FAILURE); // Salir si execvp falla
    }
    else
    {
        // father process
        if (background)
        {
            // if child is in background, print ids and dont wait it
            printf("[%d] %d\n", job_id++, pid);
        }
        else
        {
            // if child is in foreground
            foreground_pid = pid; // change foreground_id (for signal sending)

            int status;
            waitpid(pid, &status, 0); // wait child to finish

            foreground_pid = -1; // When child finish there are not process in foreground
                                 // With that the shell can ignore the signlas to himself
        }
    }
}

void command_select(char* input)
{
    // Array para almacenar comandos separados por pipes
    char* args[128];
    char* token = strtok(input, "|");
    int cmd_count = 0;

    // Divide el input en comandos individuales
    while (token != NULL) {
        args[cmd_count++] = token;
        token = strtok(NULL, "|");
    }

    // Ejecuta el pipeline si hay más de un comando
    if (cmd_count > 1) {
        execute_pipeline(args, cmd_count);
    }
    else{
        
        token = strtok(input, " \n"); // saves tokens of input (separates with spaces)
        int arg_count = 0;
        int background = 0;

        while (token != NULL)
        {
            if (strcmp(token, "&") == 0)
            { // if in the token has & is background process
                background = 1;
                break;
            }
            // saves the token in args array
            args[arg_count++] = token;
            token = strtok(NULL, " \n"); // continue taken tokens from input string
        }
        // finally we have in args array all of tokens (parts) of user input
        // example if input was "delay 10 &"
        // args = delay, 10, &

        args[arg_count] = NULL;

        if (arg_count == 0)
            return;

        if (strcmp(args[0], "cd") == 0)
        {
            cd_command(args[1]);
        }
        else if (strcmp(args[0], "clr") == 0)
        {
            system("clear");
        }
        else if (strcmp(args[0], "echo") == 0)
        {
            echo_command(args[1]);
        }
        else if (strcmp(args[0], "quit") == 0)
        {
            quit_command();
            exit(0);
        }
        else
        {
            execute_external_command(args, background);
        }
    }
}
