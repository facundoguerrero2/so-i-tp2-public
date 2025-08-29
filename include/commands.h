#pragma once
#include <cjson/cJSON.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Number of seconds for sleep interval.
 */
#define SEC 1

/**
 * @def ARG_BUFFER_SIZE
 * @brief Buffer size for command arguments.
 */
#define ARG_BUFFER_SIZE 128

/**
 * @brief Default file permissions for created files.
 *
 * This macro defines the default file permissions as 0644, which means:
 * - Owner: read and write
 * - Group: read
 * - Others: read
 */
#define FILE_PERMISSIONS 0644
/**
 * @brief Current command entered by the user.
 */
extern char* command;

/**
 * @brief Batch file for executing commands from a file.
 */
extern FILE* batch_file;

/**
 * @brief Current job identifier.
 */
extern int job_id;

/**
 * @brief PID of the foreground process.
 */
extern pid_t foreground_pid;

/**
 * @brief Tokenizes a string based on a given separator.
 *
 * This function takes an input string and a separator, and splits the input string
 * into tokens based on the separator. The tokens are stored in the provided array.
 *
 * @param input The input string to be tokenized.
 * @param separator The characters used as delimiters for tokenization.
 * @param args The array where the tokens will be stored.
 * @return The number of tokens found in the input string.
 */
int tokenize(char* input, char* separator, char* args[]);
/**
 * @brief Changes the current working directory.
 *
 * @param path Path of the directory to change to.
 */
void cd_command(char* path);

/**
 * @brief Prints a message to the console.
 *
 * @param msj Message to print.
 */
void echo_command(char* msj);

/**
 * @brief Frees resources and exits the shell.
 */
void quit_command(void);
/**
 * @brief Executes an internal command if it matches known commands.
 *
 * This function checks if the given command matches any known internal commands
 * (e.g., `cd`, `clr`, `echo`, `quit`). If it matches, the corresponding internal
 * command function is executed.
 *
 * @param args Array of command arguments.
 * @return `true` if the command is an internal command and was executed, `false` otherwise.
 */
bool excecute_internal_command(char* args[]);
/**
 * @brief Executes an external command.
 *
 * @param args Array of command arguments.
 * @param background Indicates if the command should run in the background.
 */
void execute_external_command(char* args[], int background);
/**
 * @brief Executes a pipeline of commands.
 *
 * This function takes an array of commands and executes them in a pipeline,
 * where the output of each command is passed as input to the next command.
 *
 * @param cmds Array of commands to be executed in the pipeline.
 * @param n Number of commands in the pipeline.
 */
void execute_pipeline(char* cmds[], int n);

/**
 * @brief Selects and executes the entered command.
 *
 * @param input Command entered by the user.
 */
void command_select(char* input);

/**
 * @brief Handles input and output redirection for a command.
 *
 * This function checks the arguments for input (`<`) and output (`>`) redirection
 * operators and sets up the appropriate file descriptors for `stdin` and `stdout`.
 *
 * @param args Array of command arguments.
 * @param tokens_cant Number of tokens in the `args` array.
 */
void redirect(char* args[], int tokens_cant);

/**
 * @brief Sends config array to monitor
 */
void send_config();

/**
 * @brief Checks if a filename has a specific suffix.
 *
 * This function checks if the given filename ends with the specified suffix.
 *
 * @param name The filename to check.
 * @param suf The suffix to look for.
 * @return true if the filename ends with the suffix, false otherwise.
 */
int has_suffix(const char* name, const char* suf);

/**
 * @brief Recursively scans a directory for JSON files and prints their content.
 *
 * This function traverses the specified directory and its subdirectories,
 * looking for files with a `.json` suffix. For each JSON file found, it reads
 * and prints its content in a flattened key-value format.
 *
 * @param dirpath The path of the directory to scan.
 */
void scan_json_recursive(const char* dirpath);

/**
 * @brief Flattens a cJSON object and prints its key-value pairs.
 *
 * This function recursively traverses a cJSON object or array, flattening its structure
 * and printing each key-value pair in a dot-separated format. For arrays, indices are used
 * to represent the position of elements.
 *
 * @param node The cJSON object or array to flatten.
 * @param prefix The prefix to use for keys (can be NULL).
 */
void json_flatten_kv(cJSON* node, const char* prefix);

/**
 * @brief Prints a key-value pair from a cJSON node.
 *
 * This function prints the key and value of a cJSON node in a formatted manner.
 * It handles different types of cJSON nodes, including strings, numbers, booleans, and nulls.
 *
 * @param key The key associated with the cJSON node.
 * @param v The cJSON node containing the value to print.
 */
void print_kv_line(const char* key, cJSON* v);

/**
 * @brief Prints the content of a file.
 *
 * This function reads and prints the content of the specified file to the standard output.
 * If the file cannot be opened or read, an error message is printed.
 *
 * @param path The path to the file to be read.
 */
void print_file_content(const char* path);