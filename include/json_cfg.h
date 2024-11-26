#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "fifos.h"
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
