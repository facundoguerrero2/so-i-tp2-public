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
                                      [CFG_UPDATE_PROCESSES] = "update_processes"};

void read_config_from_json()
{
    // Leer el archivo JSON
    FILE* file = fopen(PROJECT_PATH "/config.json", "r");
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

    cfg[10] = cJSON_GetObjectItemCaseSensitive(intervals, "sampling_interval")->valueint;

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

    int values[10];
    const char* metric_names[] = {"update_cpu",
                                  "update_memory",
                                  "update_disk_read_time",
                                  "update_disk_write_time",
                                  "update_disk_io_time",
                                  "update_net_receive_kbps",
                                  "update_net_sent_kbps",
                                  "update_net_received_packets",
                                  "update_net_sent_packets",
                                  "update_processes"};

    for (int i = 0; i < 10; i++)
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

    // Escribir la cadena en el archivo
    FILE* file = fopen(PROJECT_PATH "/config.json", "w");
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
