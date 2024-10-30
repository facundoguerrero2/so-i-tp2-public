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

# Flags for compiling
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS_DEBUG "-g3 -Wall -pedantic -Werror -Wextra -Wconversion")

# Include headers
include_directories(include)

find_package(cJSON REQUIRED)
find_package(unity REQUIRED)

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE cjson::cjson)