#include "fifos.h"
int cfg[CONFIG_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}; // initialize with no metrics and 1 second sleep

void create_fifos()
{
    mkfifo(FIFO_CONFIG, FIFO_FILE_PERMISSIONS);
    mkfifo(FIFO_CONFIG_ACK, FIFO_FILE_PERMISSIONS);
}
