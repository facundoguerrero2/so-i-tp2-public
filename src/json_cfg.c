#include "json_cfg.h"

const char* config_keys[CFG_COUNT] = {[CFG_UPDATE_CPU] = "update_cpu",
                                      [CFG_UPDATE_MEMORY] = "update_memory",
                                      [CFG_UPDATE_DISK_READ_TIME] = "update_disk_read_time",
                                      [CFG_UPDATE_DISK_WRITE_TIME] = "update_disk_write_time",
                                      [CFG_UPDATE_DISK_IO_TIME] = "update_disk_io_time",
                                      [CFG_UPDATE_NET_RECEIVE_KBPS] = "update_net_receive_kbps",
                                      [CFG_UPDATE_NET_SENT_KBPS] = "update_net_sent_kbps",
                                      [CFG_UPDATE_NET_RECEIVED_PACKETS] = "update_net_received_packets",
                                      [CFG_UPDATE_NET_SENT_PACKETS] = "update_net_sent_packets",
                                      [CFG_UPDATE_PROCESSES] = "update_processes",
                                      [CFG_UPDATE_FRAGMENTATION] = "update_fragmentation",
                                      [CFG_UPDATE_FIT_COUNTERS] = "update_fit_counters"};

void read_config_from_json()
{
    // Obtener la variable de entorno BINARY_PATH
    const char* binary_path = getenv("BINARY_PATH");
    if (!binary_path)
    {
        fprintf(stderr, "Error: BINARY_PATH no está definido\n");
        return;
    }

    // Construir la ruta completa al archivo config.json
    char config_path[PATH_MAX];
    snprintf(config_path, sizeof(config_path), "%s/config.json", binary_path);

    FILE* file = fopen(config_path, "r");
    if (!file)
    {
        perror("Error al abrir el archivo de configuración");
        return;
    }

    // Leer el contenido en un buffer fijo
    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_read = fread(buffer, 1, MAX_BUFFER_SIZE - 1, file);
    fclose(file);
    buffer[bytes_read] = '\0'; // Asegurar la terminación nula

    // Parsear el JSON
    cJSON* json = cJSON_Parse(buffer);
    if (!json)
    {
        fprintf(stderr, "Error al parsear el JSON: %s\n", cJSON_GetErrorPtr());
        return;
    }

    // Leer "metrics"
    cJSON* metrics = cJSON_GetObjectItemCaseSensitive(json, "metrics");
    if (!cJSON_IsObject(metrics))
    {
        fprintf(stderr, "Error: no se encontró el objeto 'metrics'\n");
        cJSON_Delete(json);
        return;
    }

    for (int i = 0; i < CFG_COUNT; i++)
    {
        cJSON* item = cJSON_GetObjectItemCaseSensitive(metrics, config_keys[i]);
        if (cJSON_IsNumber(item))
        {
            cfg[i] = item->valueint;
        }
        else
        {
            cfg[i] = 0; // Default value if the JSON key is missing or invalid
        }
    }

    // Leer "intervals"
    cJSON* intervals = cJSON_GetObjectItemCaseSensitive(json, "intervals");
    if (!cJSON_IsObject(intervals))
    {
        fprintf(stderr, "Error: no se encontró el objeto 'intervals'\n");
        cJSON_Delete(json);
        return;
    }

    cfg[CONFIG_SIZE-1] = cJSON_GetObjectItemCaseSensitive(intervals, "sampling_interval")->valueint;

    // Limpiar el objeto JSON
    cJSON_Delete(json);
}

void update_config_from_input()
{
    // Crear un objeto JSON vacío
    cJSON* root = cJSON_CreateObject();
    if (!root)
    {
        fprintf(stderr, "Error al crear el objeto JSON\n");
        return;
    }

    // Crear objetos para "intervals" y "metrics"
    cJSON* intervals = cJSON_CreateObject();
    cJSON* metrics = cJSON_CreateObject();
    if (!intervals || !metrics)
    {
        fprintf(stderr, "Error al crear los objetos 'intervals' o 'metrics'\n");
        cJSON_Delete(root);
        return;
    }

    // Agregar los objetos al root
    cJSON_AddItemToObject(root, "intervals", intervals);
    cJSON_AddItemToObject(root, "metrics", metrics);

    // Solicitar al usuario los valores y agregarlos al JSON
    int sampling_interval;
    printf("sampling_interval value: ");
    scanf("%d", &sampling_interval);
    cJSON_AddNumberToObject(intervals, "sampling_interval", sampling_interval);

    int values[CFG_COUNT];
    const char* metric_names[] = {"update_cpu",
                                  "update_memory",
                                  "update_disk_read_time",
                                  "update_disk_write_time",
                                  "update_disk_io_time",
                                  "update_net_receive_kbps",
                                  "update_net_sent_kbps",
                                  "update_net_received_packets",
                                  "update_net_sent_packets",
                                  "update_processes",
                                  "update_fragmentation",
                                  "update_fit_counters"
                                 };

    for (int i = 0; i < CFG_COUNT; i++)
    {
        printf("Enable %s metric (0 o 1): ", metric_names[i]);
        scanf("%d", &values[i]);
        cJSON_AddNumberToObject(metrics, metric_names[i], values[i]);
    }

    // Serializar el JSON a una cadena
    char* json_string = cJSON_Print(root);
    if (!json_string)
    {
        fprintf(stderr, "Error al serializar el JSON\n");
        cJSON_Delete(root);
        return;
    }

    // Obtener la variable de entorno BINARY_PATH
    const char* binary_path = getenv("BINARY_PATH");
    if (!binary_path)
    {
        fprintf(stderr, "Error: BINARY_PATH no está definido\n");
        return;
    }

    // Construir la ruta completa al archivo config.json
    char config_path[PATH_MAX];
    snprintf(config_path, sizeof(config_path), "%s/config.json", binary_path);

    FILE* file = fopen(config_path, "w");
    if (!file)
    {
        perror("Error al abrir el archivo para escribir");
        free(json_string);
        cJSON_Delete(root);
        return;
    }

    fprintf(file, "%s", json_string);
    fclose(file);

    // Limpiar memoria
    free(json_string);
    cJSON_Delete(root);

    getchar();

    printf("Configuración actualizada exitosamente en %s\n", "config.json");
}

void setup_json()
{
    // Obtener la variable de entorno BINARY_PATH
    const char* binary_path = getenv("BINARY_PATH");
    if (!binary_path)
    {
        fprintf(stderr, "Error: BINARY_PATH no está definido\n");
        return;
    }

    // Construir la ruta completa al archivo config.json
    char config_path[PATH_MAX];
    snprintf(config_path, sizeof(config_path), "%s/config.json", binary_path);
    struct stat buffer;
    if (stat(config_path, &buffer) != 0)
    {
        // El archivo no existe, crear e inicializar
        FILE* file = fopen(config_path, "w");
        if (file)
        {
            cJSON* root = cJSON_CreateObject();
            if (!root)
            {
                fprintf(stderr, "Error al crear el objeto JSON\n");
                return;
            }

            // Crear objetos para "intervals" y "metrics"
            cJSON* intervals = cJSON_CreateObject();
            cJSON* metrics = cJSON_CreateObject();
            if (!intervals || !metrics)
            {
                fprintf(stderr, "Error al crear los objetos 'intervals' o 'metrics'\n");
                cJSON_Delete(root);
                return;
            }

            // Agregar los objetos al root
            cJSON_AddItemToObject(root, "intervals", intervals);
            cJSON_AddItemToObject(root, "metrics", metrics);

            // Solicitar al usuario los valores y agregarlos al JSON
            cJSON_AddNumberToObject(intervals, "sampling_interval", 1);

            const char* metric_names[] = {"update_cpu",
                                          "update_memory",
                                          "update_disk_read_time",
                                          "update_disk_write_time",
                                          "update_disk_io_time",
                                          "update_net_receive_kbps",
                                          "update_net_sent_kbps",
                                          "update_net_received_packets",
                                          "update_net_sent_packets",
                                          "update_processes",
                                          "update_fragmentation",
                                          "update_fit_counters"
                                        };

            for (int i = 0; i < CFG_COUNT; i++)
            {
                cJSON_AddNumberToObject(metrics, metric_names[i], 1);
            }

            // Serializar el JSON a una cadena
            char* json_string = cJSON_Print(root);
            if (!json_string)
            {
                fprintf(stderr, "Error al serializar el JSON\n");
                cJSON_Delete(root);
                return;
            }

            fprintf(file, "%s", json_string);
            fclose(file);

            // Limpiar memoria
            free(json_string);
            cJSON_Delete(root);

            printf("Archivo de configuración creado e inicializado en %s\n", config_path);
        }
        else
        {
            perror("Error al crear el archivo de configuración");
        }
    }
    else
    {
        printf("Archivo de configuración existente en %s\n", config_path);
    }
}
