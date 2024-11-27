#include "commands.h"
#include "fifos.h"
#include "json_cfg.h"
#include "signals_handling.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Definir setUp y tearDown
void setUp(void)
{
    // Inicialización antes de cada prueba
}

void tearDown(void)
{
    // Limpieza después de cada prueba
}

// Mock data for testing
char* mock_json = "{\"intervals\":{\"sampling_interval\":4},\"metrics\":{\"update_cpu\":1,\"update_memory\":1,\"update_"
                  "disk_read_time\":1,\"update_disk_write_time\":1,\"update_disk_io_time\":1}}";
char buffer[MAX_BUFFER_SIZE];

// Mock function to replace reading from a file
void mock_read_config_from_json()
{
    strcpy(buffer, mock_json);
    cJSON* json = cJSON_Parse(buffer);
    if (!json)
    {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        return;
    }

    cJSON* metrics = cJSON_GetObjectItemCaseSensitive(json, "metrics");
    if (cJSON_IsObject(metrics))
    {
        cfg[0] = cJSON_GetObjectItemCaseSensitive(metrics, "update_cpu")->valueint;
        cfg[1] = cJSON_GetObjectItemCaseSensitive(metrics, "update_memory")->valueint;
        cfg[2] = cJSON_GetObjectItemCaseSensitive(metrics, "update_disk_read_time")->valueint;
        cfg[3] = cJSON_GetObjectItemCaseSensitive(metrics, "update_disk_write_time")->valueint;
        cfg[4] = cJSON_GetObjectItemCaseSensitive(metrics, "update_disk_io_time")->valueint;
    }

    cJSON* intervals = cJSON_GetObjectItemCaseSensitive(json, "intervals");
    if (cJSON_IsObject(intervals))
    {
        cfg[10] = cJSON_GetObjectItemCaseSensitive(intervals, "sampling_interval")->valueint;
    }

    cJSON_Delete(json);
}

// Test function for read_config_from_json
void test_read_config_from_json()
{
    mock_read_config_from_json();
    TEST_ASSERT_EQUAL_INT(1, cfg[0]);
    TEST_ASSERT_EQUAL_INT(1, cfg[1]);
    TEST_ASSERT_EQUAL_INT(1, cfg[2]);
    TEST_ASSERT_EQUAL_INT(1, cfg[3]);
    TEST_ASSERT_EQUAL_INT(1, cfg[4]);
    TEST_ASSERT_EQUAL_INT(4, cfg[10]);
}

// Test function for tokenize
void test_tokenize()
{
    char input[] = "echo hello world";
    char* args[4];
    int tokens = tokenize(input, " ", args);
    TEST_ASSERT_EQUAL_INT(3, tokens);
    TEST_ASSERT_EQUAL_STRING("echo", args[0]);
    TEST_ASSERT_EQUAL_STRING("hello", args[1]);
    TEST_ASSERT_EQUAL_STRING("world", args[2]);
}

// Test function for signal_handler
void test_signal_handler()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process
        while (1)
            ; // Infinite loop to keep the process running
    }
    else
    {
        // Parent process
        foreground_pid = pid;
        signal_handler(SIGTERM);
        int status;
        waitpid(pid, &status, 0);
        TEST_ASSERT_TRUE(WIFSIGNALED(status));
        TEST_ASSERT_EQUAL_INT(SIGTERM, WTERMSIG(status));
    }
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_read_config_from_json);
    RUN_TEST(test_tokenize);
    // RUN_TEST(test_command_select);
    RUN_TEST(test_signal_handler);

    return UNITY_END();
}
