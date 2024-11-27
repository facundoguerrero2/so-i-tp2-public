#pragma once
#include "fifos.h"
#include "paths_cfg.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * @brief max json buffer size
 */
#define MAX_BUFFER_SIZE 4096
/**
 * @brief read config from json and storage it in int cfg array.
 */
void read_config_from_json();

/**
 * @brief update json from user input in terminal
 */
void update_config_from_input();

/**
 * @brief Indices for configuration settings in the cfg array.
 *
 * This enum defines the indices for various configuration settings in the cfg array.
 */
typedef enum
{
    CFG_UPDATE_CPU,
    CFG_UPDATE_MEMORY,
    CFG_UPDATE_DISK_READ_TIME,
    CFG_UPDATE_DISK_WRITE_TIME,
    CFG_UPDATE_DISK_IO_TIME,
    CFG_UPDATE_NET_RECEIVE_KBPS,
    CFG_UPDATE_NET_SENT_KBPS,
    CFG_UPDATE_NET_RECEIVED_PACKETS,
    CFG_UPDATE_NET_SENT_PACKETS,
    CFG_UPDATE_PROCESSES,
    CFG_COUNT // Always the last to define the size of the array
} config_indices;

/**
 * @brief Array of configuration keys corresponding to the indices in config_indices.
 *
 * This array maps the indices defined in config_indices to their corresponding
 * configuration keys as strings.
 */
extern const char* config_keys[CFG_COUNT];

/**
 * @brief checks if config.json exists in cfg_file_path
 * if not exists it will create one and initialize it
 */
void setup_json();
