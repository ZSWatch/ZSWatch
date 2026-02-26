# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0
#
# Example CMakeLists.txt for building this app with the ZSWatch EDK.
# Copy this directory as a starting point for your own app.
#
# Build:
#   cmake -B build -DLLEXT_EDK_INSTALL_DIR=/path/to/zswatch-edk
#   make -C build
#
# The .llext file will be in build/my_app.llext

cmake_minimum_required(VERSION 3.20.0)

# Point to the EDK's standalone build file
if(NOT DEFINED LLEXT_EDK_INSTALL_DIR)
    set(LLEXT_EDK_INSTALL_DIR $ENV{LLEXT_EDK_INSTALL_DIR})
endif()

set(CMAKE_TOOLCHAIN_FILE ${LLEXT_EDK_INSTALL_DIR}/toolchain.cmake)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

project(my_app)

# Load EDK flags
include(${LLEXT_EDK_INSTALL_DIR}/cmake.cflags)
if(EXISTS "${LLEXT_EDK_INSTALL_DIR}/zswatch_extra.cmake")
    include(${LLEXT_EDK_INSTALL_DIR}/zswatch_extra.cmake)
endif()

# Collect sources
file(GLOB APP_SOURCES ${CMAKE_SOURCE_DIR}/*.c)

# Build flags
separate_arguments(CFLAGS_LIST UNIX_COMMAND "${LLEXT_CFLAGS}")
list(APPEND CFLAGS_LIST "-c")
list(APPEND CFLAGS_LIST "-I${CMAKE_SOURCE_DIR}")

# Compile
set(OBJ_FILES "")
foreach(src ${APP_SOURCES})
    get_filename_component(src_name ${src} NAME_WE)
    set(obj_file ${PROJECT_BINARY_DIR}/${src_name}.o)
    list(APPEND OBJ_FILES ${obj_file})
    add_custom_command(
        OUTPUT ${obj_file}
        COMMAND ${CMAKE_C_COMPILER} ${CFLAGS_LIST} -o ${obj_file} ${src}
        DEPENDS ${src}
        COMMENT "Compiling ${src_name}.c"
    )
endforeach()

# Link
if(DEFINED LLEXT_EXTRA_LINK_FLAGS)
    separate_arguments(LINK_FLAGS_LIST UNIX_COMMAND "${LLEXT_EXTRA_LINK_FLAGS}")
else()
    set(LINK_FLAGS_LIST "")
endif()

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.llext
    COMMAND ${CMAKE_C_COMPILER}
        -shared -nostdlib -nodefaultlibs -fPIC
        ${LINK_FLAGS_LIST}
        -o ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.llext
        ${OBJ_FILES}
    DEPENDS ${OBJ_FILES}
    COMMENT "Linking ${PROJECT_NAME}.llext"
)

add_custom_target(${PROJECT_NAME} ALL
    DEPENDS ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.llext
)
