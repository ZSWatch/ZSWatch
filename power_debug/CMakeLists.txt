cmake_minimum_required(VERSION 3.20.0)

# Reuse the board definitions from the main application
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../app)

# Add custom device tree bindings from the main app
list(APPEND DTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../app)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(PowerDebug)

# Build the custom drivers used by the WatchDK board
add_subdirectory(../app/drivers ${CMAKE_CURRENT_BINARY_DIR}/app_drivers)

target_sources(app PRIVATE
    src/main.c
)

target_include_directories(app PRIVATE
    ../app/src
)
