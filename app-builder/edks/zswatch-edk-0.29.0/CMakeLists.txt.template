# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
#
# Standalone CMakeLists.txt for building ZSWatch LLEXT apps out-of-tree.
#
# Usage:
#   cmake -B build -DLLEXT_EDK_INSTALL_DIR=/path/to/zswatch-edk
#   make -C build
#
# Produces: build/<project_name>.llext

cmake_minimum_required(VERSION 3.20.0)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/toolchain.cmake)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# The project name becomes the .llext output name
project(my_app)

# --- Locate EDK ---
if(NOT DEFINED LLEXT_EDK_INSTALL_DIR)
    set(LLEXT_EDK_INSTALL_DIR $ENV{LLEXT_EDK_INSTALL_DIR})
endif()

if(NOT LLEXT_EDK_INSTALL_DIR OR NOT EXISTS "${LLEXT_EDK_INSTALL_DIR}/cmake.cflags")
    message(FATAL_ERROR
        "LLEXT_EDK_INSTALL_DIR not set or invalid.\n"
        "Set it to the extracted ZSWatch EDK directory:\n"
        "  cmake -B build -DLLEXT_EDK_INSTALL_DIR=/path/to/zswatch-edk-<version>"
    )
endif()

# --- Load Zephyr EDK compile flags (sets LLEXT_CFLAGS as a CMake list) ---
include(${LLEXT_EDK_INSTALL_DIR}/cmake.cflags)

# --- Load ZSWatch-specific extras (appends to LLEXT_CFLAGS) ---
if(EXISTS "${LLEXT_EDK_INSTALL_DIR}/zswatch_extra.cmake")
    include(${LLEXT_EDK_INSTALL_DIR}/zswatch_extra.cmake)
endif()

# --- Collect source files ---
# By default, compile all .c files in src/. Override with -DAPP_SOURCES="file1.c;file2.c"
if(NOT DEFINED APP_SOURCES)
    file(GLOB APP_SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)
    if(NOT APP_SOURCES)
        # Fall back to .c files in project root
        file(GLOB APP_SOURCES ${CMAKE_SOURCE_DIR}/*.c)
    endif()
endif()

if(NOT APP_SOURCES)
    message(FATAL_ERROR "No source files found. Place .c files in src/ or set APP_SOURCES.")
endif()

# --- Register compile flags ---
# LLEXT_CFLAGS is already a proper CMake list (semicolon-separated).
# Use add_compile_options() which handles CMake lists correctly.
add_compile_options(${LLEXT_CFLAGS})

# Include the source directory itself (for local headers / image .c files)
add_compile_options(-I${CMAKE_SOURCE_DIR}/src -I${CMAKE_SOURCE_DIR})

# Additional include directories
if(DEFINED APP_EXTRA_INCLUDES)
    foreach(inc ${APP_EXTRA_INCLUDES})
        add_compile_options(-I${inc})
    endforeach()
endif()

# Retrieve the final flat list of compile options
get_property(ALL_CFLAGS DIRECTORY PROPERTY COMPILE_OPTIONS)

# --- Compile each source to .o ---
set(OBJ_FILES "")
foreach(src ${APP_SOURCES})
    get_filename_component(src_name ${src} NAME_WE)
    set(obj_file ${PROJECT_BINARY_DIR}/${src_name}.o)
    list(APPEND OBJ_FILES ${obj_file})

    add_custom_command(
        OUTPUT ${obj_file}
        COMMAND ${CMAKE_C_COMPILER} ${ALL_CFLAGS} -c
            -o ${obj_file}
            ${src}
        DEPENDS ${src}
        COMMENT "Compiling ${src_name}.c"
    )
endforeach()

# --- Link into shared library (.llext) ---
set(LINK_FLAGS -shared -nostdlib -nodefaultlibs -fPIC)

# ZSWatch-specific link flags (e.g. --unique for .text.iflash)
if(DEFINED LLEXT_EXTRA_LINK_FLAGS)
    # LLEXT_EXTRA_LINK_FLAGS may be a plain string — split it
    separate_arguments(EXTRA_LINK_LIST UNIX_COMMAND "${LLEXT_EXTRA_LINK_FLAGS}")
    list(APPEND LINK_FLAGS ${EXTRA_LINK_LIST})
endif()

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.llext
    COMMAND ${CMAKE_C_COMPILER}
        ${LINK_FLAGS}
        -o ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.llext
        ${OBJ_FILES}
    DEPENDS ${OBJ_FILES}
    COMMENT "Linking ${PROJECT_NAME}.llext"
)

add_custom_target(${PROJECT_NAME} ALL
    DEPENDS ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.llext
)

message(STATUS "ZSWatch LLEXT app: ${PROJECT_NAME}")
message(STATUS "Sources: ${APP_SOURCES}")
message(STATUS "EDK: ${LLEXT_EDK_INSTALL_DIR}")
