#Verify cmake version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Create project
project(
    "TEST"
    VERSION 1.0.0
    LANGUAGES C
)

# Enable testing
include(CTest)
# Include headers
include_directories(../include)

# Flags for compiling
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-g -fprofile-arcs -ftest-coverage --coverage")
set(CMAKE_EXE_LINKER_FLAGS "-lgcov --coverage")

find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

enable_testing()
add_executable(${PROJECT_NAME} test.c ../src/commands.c ../src/signals_handling.c ../src/monitor.c ../src/fifos.c ../src/json_cfg.c)

target_link_libraries(${PROJECT_NAME} PRIVATE unity::unity cjson::cjson)
add_test(PRUEBA ${PROJECT_NAME})
