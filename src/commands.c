#include "commands.h"
#include "fifos.h"
#include "json_cfg.h"
#include "monitor.h"
char* command = NULL;
FILE* batch_file = NULL;
FILE* input_file = NULL;
FILE* output_file = NULL;
int job_id = 1;
pid_t foreground_pid = -1;

int tokenize(char* input, char* separator, char* args[])
{

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
    {
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
    stop_monitor();
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
    if (input_file != NULL)
    {
        fclose(input_file);
        input_file = NULL;
        printf("Archivo cerrado.\n");
    }
    if (output_file != NULL)
    {
        fclose(output_file);
        output_file = NULL;

        printf("Archivo cerrado.\n");
    }
}

void send_config()
{
    read_config_from_json();
    int fd_write_config;
    int fd_read_config;
    fd_write_config = open(FIFO_CONFIG, O_WRONLY | O_NONBLOCK);

    printf("Config to send: ");
    for (int i = 0; i < CONFIG_SIZE; i++)
    {
        printf("%d ", cfg[i]);
    }
    printf("\n");

    write(fd_write_config, cfg, sizeof(cfg));
    close(fd_write_config);

    fd_read_config = open(FIFO_CONFIG_ACK, O_RDONLY);
    int cfg_ack[CONFIG_SIZE];
    read(fd_read_config, cfg_ack, sizeof(cfg));
    close(fd_read_config);

    printf("Config acknowledge: ");
    for (int i = 0; i < CONFIG_SIZE; i++)
    {
        printf("%d ", cfg_ack[i]);
    }
    printf("\n");
}

bool excecute_internal_command(char* args[])
{

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
    else if (strcmp(args[0], "monitor") == 0 && args[1] != NULL && strcmp(args[1], "--stop") == 0)
    {
        stop_monitor();
    }
    else if (strcmp(args[0], "monitor") == 0 && args[1] != NULL && strcmp(args[1], "--status") == 0)
    {
        status_monitor();
    }
    else if (strcmp(args[0], "monitor") == 0 && args[1] != NULL && strcmp(args[1], "--restart") == 0)
    {
        stop_monitor();
        start_monitor();
        sleep(1); // sleep for monitor starts and wait cfg
        send_config();
    }
    else if (strcmp(args[0], "monitor") == 0 && args[1] != NULL && strcmp(args[1], "--changecfg") == 0)
    {
        update_config_from_input();
    }
    else if (strcmp(args[0], "monitor") == 0 && args[1] != NULL && strcmp(args[1], "--start") == 0)
    {
        start_monitor();
        sleep(1); // sleep for monitor starts and wait cfg
        send_config();
    }
    else
    {
        return 0;
    }
    return 1;
}
void execute_external_command(char* args[], int tokens_cant)
{
    int background = 0;
    if (strcmp(args[tokens_cant - 1], "&") == 0)
    { // if in the token has & is background process
        background = 1;
        args[tokens_cant - 1] = NULL;
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
    int pipefd[2];   // pipefd[0]: Extremo de lectura.
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

            char* args[ARG_BUFFER_SIZE];
            tokenize(cmds[i], " \n", args);

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
    char* args[ARG_BUFFER_SIZE];
    int tokens_cant = tokenize(input, "|", args); // dividimos en tokens separados por |
    // si hay mas de un token entonces hay dos comandos entonces ejecutamos pipeline
    if (tokens_cant > 1) // si da mayor a uno significa que hay pipes que hay mas de un comando
    {
        execute_pipeline(args, tokens_cant);
    }
    else // si da 1 entonces ejecutamos normal (un solo comando)
    {
        // si hay un solo comando entonces lo dividimos para comando + argumento
        tokens_cant = tokenize(input, " \n", args);
        if (tokens_cant == 0)
        {
            return;
        }
        // save original descriptors
        int original_stdin = dup(STDIN_FILENO);
        int original_stdout = dup(STDOUT_FILENO);

        redirect(args, tokens_cant); // Llamar a redirect antes de ejecutar el comando
        if (!excecute_internal_command(args))
        { // si no es interno es externo
            execute_external_command(args, tokens_cant);
        }
        // recover original descriptors
        dup2(original_stdin, STDIN_FILENO);
        dup2(original_stdout, STDOUT_FILENO);
        close(original_stdin);
        close(original_stdout);
    }
}

void redirect(char* args[], int tokens_cant)
{
    for (int i = 0; i < tokens_cant; i++)
    {
        if (strcmp(args[i], "<") == 0 && i + 1 < tokens_cant)
        {
            int in_fd = open(args[i + 1], O_RDONLY);
            if (in_fd == -1)
            {
                perror("No se puede abrir el archivo de entrada");
                return;
            }
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">") == 0 && i + 1 < tokens_cant)
        {
            int out_fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, FILE_PERMISSIONS);
            if (out_fd == -1)
            {
                perror("No se puede abrir el archivo de salida");
                return;
            }
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
            args[i] = NULL;
        }
    }
}
