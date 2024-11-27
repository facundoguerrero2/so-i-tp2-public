#include "paths_cfg.h"

void set_path()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd() error");
        return;
    }

    // Definir la variable de entorno BINARY_PATH con el directorio actual
    if (setenv("BINARY_PATH", cwd, 1) != 0)
    {
        perror("setenv() error");
    }
    else
    {
        printf("BINARY_PATH set to %s\n", cwd);
    }
}
