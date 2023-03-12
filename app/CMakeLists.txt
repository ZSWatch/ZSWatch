cmake_minimum_required(VERSION 3.20.0)

set(BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lvgl)

add_subdirectory(drivers)
add_subdirectory(src/applications)
add_subdirectory(src/ext_drivers/bmi270)

include_directories(src/)
include_directories(src/ui)
include_directories(src/applications)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

FILE(GLOB img_sources src/images/*.c)
target_sources(app PRIVATE ${img_sources})

FILE(GLOB ui_sources src/ui/*.c)
target_sources(app PRIVATE ${ui_sources})

FILE(GLOB events_sources src/events/*.c)
target_sources(app PRIVATE ${events_sources})

add_compile_definitions(LV_LVGL_H_INCLUDE_SIMPLE)