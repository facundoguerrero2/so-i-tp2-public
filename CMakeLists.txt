#Verify cmake version
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Create project
project(
    "ShellProject"
    VERSION 1.0.0
    LANGUAGES C
)

# Enable testing
include(CTest)
# Include headers
include_directories(include)
add_subdirectory(so-i-24-facundoguerrero2-public)
# Flags for compiling
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g3 -Wall -pedantic -Werror -Wextra -Wconversion")


find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

add_executable(${PROJECT_NAME} src/main.c src/commands.c src/signals_handling.c src/monitor.c src/fifos.c src/json_cfg.c)

target_link_libraries(${PROJECT_NAME} PRIVATE cjson::cjson)

if(RUN_COVERAGE EQUAL 1)
    message("Run with coverage")
    add_subdirectory(tests)
endif()
