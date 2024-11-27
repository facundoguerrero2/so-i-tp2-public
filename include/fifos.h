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
 * @brief CFG array
 */
extern int cfg[CONFIG_SIZE];
/**
 * @brief create the fifos
 */
void create_fifos();
