#include "commands.h"

char* command = NULL;
FILE* batch_file = NULL;
int job_id = 1;
pid_t foreground_pid = -1;

int tokenize(char *input, char* separator, char* args[]){
    
    char* token = strtok(input, separator);
    int t = 0;
    while (token != NULL)
    {
        args[t++] = token;
        token = strtok(NULL, separator);
    }
    args[t] = NULL;
    return t;
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

bool excecute_internal_command(char *args[]){

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
    }else{
        return 0;
    }
    return 1;
}

void execute_external_command(char* args[], int tokens_cant)
{   
    int background = 0;
    if (strcmp(args[tokens_cant-1], "&") == 0)
    { // if in the token has & is background process
        background = 1;
        args[tokens_cant-1] = NULL;
    }
    
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


void execute_pipeline(char* cmds[], int n)
{
    int pipefd[2]; // pipefd[0]: Extremo de lectura.
                   // pipefd[1]: Extremo de escritura.
    int prev_fd = 0; // Para guardar el extremo de lectura del pipe anterior

    for (int i = 0; i < n; i++)
    {                 // for para hacer esto por cada n (comandos) unidos por |
        pipe(pipefd); // Crear un nuevo pipe

        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork failed");
            return;
        }

        if (pid == 0)
        {
            // Proceso hijo
            dup2(prev_fd, STDIN_FILENO); // Redirigir entrada estándar para leer en el pipe anterior
            if (i < n - 1)
            {
                dup2(pipefd[1], STDOUT_FILENO); // Redirigir salida estándar para leer en el pipe siguiente
            }
            close(pipefd[0]);
            
            char* args[128];
            tokenize(cmds[i]," \n",args);

            if (execvp(args[0], args) == -1)
            {
                perror("execvp error");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // Proceso padre
            wait(NULL);
            close(pipefd[1]);
            prev_fd = pipefd[0];
        }
    }
}



void command_select(char* input)
{
    // Array para almacenar tokens
    char* args[64];
    int tokens_cant = tokenize(input, "|", args); //dividimos en tokens separados por |
    // si hay mas de un token entonces hay dos comandos entonces ejecutamos pipeline
    if (tokens_cant > 1) //si da mayor a uno significa que hay pipes que hay mas de un comando
    {
        execute_pipeline(args, tokens_cant);
    }
    else //si da 1 entonces ejecutamos normal (un solo comando)
    {
        // si hay un solo comando entonces lo dividimos para comando + argumento
        tokens_cant = tokenize(input, " \n", args);
        if(tokens_cant == 0){
            return;
        }

        if(!excecute_internal_command(args)){ //si no es interno es externo
            execute_external_command(args,tokens_cant);
        }
    }
}
