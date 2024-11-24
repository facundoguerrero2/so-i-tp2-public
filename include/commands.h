#pragma once
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>

/**
 * @brief Current command entered by the user.
 */
extern char *command;

/**
 * @brief Batch file for executing commands from a file.
 */
extern FILE *batch_file;

/**
 * @brief Current job identifier.
 */
extern int job_id;

/**
 * @brief PID of the foreground process.
 */
extern pid_t foreground_pid;

/**
 * @brief Selects and executes the entered command.
 * 
 * @param input Command entered by the user.
 */
void command_select(char *input);

/**
 * @brief Changes the current working directory.
 * 
 * @param path Path of the directory to change to.
 */
void cd_command(char *path);

/**
 * @brief Prints a message to the console.
 * 
 * @param msj Message to print.
 */
void echo_command(char *msj);

/**
 * @brief Frees resources and exits the shell.
 */
void quit_command(void);

/**
 * @brief Executes an external command.
 * 
 * @param args Array of command arguments.
 * @param background Indicates if the command should run in the background.
 */
void execute_external_command(char *args[], int background);
