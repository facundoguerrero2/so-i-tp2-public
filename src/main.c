#include <cjson/cJSON.h>

#include "commands.h"
#include "signals_handling.h"
#include "fifos.h"

void display_prompt()
{
    char cwd[PATH_MAX];
    char* user = getenv("USER");
    char hostname[HOST_NAME_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd() error");
        return;
    }

    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        perror("gethostname() error");
        return;
    }

    printf("%s@%s:%s$ ", user, hostname, cwd);
    fflush(stdout);
}





int main(int argc, char* argv[])
{
    create_fifos();  

    size_t len = 0;
    setup_signal_handlers();
    if (argc > 1)
    {
        // Modo batch
        batch_file = fopen(argv[1], "r");
        if (batch_file == NULL)
        {
            perror("Error abriendo el archivo batch");
            return 1;
        }

        while (getline(&command, &len, batch_file) != -1)
        {
            command_select(command);
        }
    }
    else
    {
        // Modo
        while (1)
        {
            display_prompt();                               // Mostrar el prompt
            ssize_t nread = getline(&command, &len, stdin); // Leer el comando

            if (nread == -1)
            {
                break; // Salir en caso de EOF o error
            }

            command_select(command);
        }
    }

    quit_command();

    return 0;
}
