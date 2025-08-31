# Create project
project(
    "TEST_MemProject"
    VERSION 1.0.0
    LANGUAGES C
)

set(CMAKE_C_FLAGS "--coverage")

enable_testing()
add_executable(${PROJECT_NAME} test.c ../src/commands.c ../src/signals_handling.c ../src/monitor.c ../src/fifos.c ../src/json_cfg.c)

target_link_libraries(${PROJECT_NAME} PRIVATE unity::unity cjson::cjson)
add_test(PRUEBA ${PROJECT_NAME})
