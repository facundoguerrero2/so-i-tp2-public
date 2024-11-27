#pragma once

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
/**
 * @brief Ruta fifo configs
 */
#define FIFO_CONFIG "/tmp/fifo_config"
/**
 * @brief ruta fifo config ack
 */
#define FIFO_CONFIG_ACK "/tmp/fifo_config_ack"
/**
 * @brief CFG ARRAY SIZE
 */
#define CONFIG_SIZE 11
/**
 * @brief Default file permissions for FIFO files.
 *
 * This macro defines the default file permissions for FIFO files as 0666, which means:
 * - Owner: read and write
 * - Group: read and write
 * - Others: read and write
 */
#define FIFO_FILE_PERMISSIONS 0666
/**
 * @brief CFG array
 */
extern int cfg[CONFIG_SIZE];
/**
 * @brief create the fifos
 */
void create_fifos();
