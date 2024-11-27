#pragma once

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief PID of the monitoring process.
 */
extern pid_t monitor_pid;

/**
 * @brief Starts the monitoring process.
 *
 * This function starts the monitoring process by forking a new process and
 * executing the monitoring program.
 */
void start_monitor();

/**
 * @brief Stops the monitoring process.
 *
 * This function stops the monitoring process by sending a termination signal
 * to the process with the PID stored in `monitor_pid`.
 */
void stop_monitor();

/**
 * @brief Displays the status of the monitoring process.
 *
 * This function displays the current status of the monitoring process,
 * including whether it is running and its PID.
 */
void status_monitor();
