# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

# --------------------------------------------------------------------------
# LLEXT app helper function – defined here (before the subdirectory loop)
# so that individual app CMakeLists.txt files can call add_zsw_llext_app().
# --------------------------------------------------------------------------
if(CONFIG_ZSW_LLEXT_APPS)
    # Destination for built .llext files — picked up by `west upload_fs`
    set(LFS_APPS_DIR ${CMAKE_SOURCE_DIR}/src/images/binaries/lvgl_lfs/apps)

    function(add_zsw_llext_app name)
        cmake_parse_arguments(ARG "" "" "SOURCES;EXTRA_INCLUDES" ${ARGN})

        # 1) Create the LLEXT ELF target
        add_llext_target(${name}
            OUTPUT  ${PROJECT_BINARY_DIR}/llext/${name}.llext
            SOURCES ${ARG_SOURCES}
        )

        # 2) Common include directories
        target_include_directories(${name}_llext_lib PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/src/managers
            ${CMAKE_SOURCE_DIR}/src/llext
        )

        # 3) App-specific extra includes
        if(ARG_EXTRA_INCLUDES)
            target_include_directories(${name}_llext_lib PRIVATE ${ARG_EXTRA_INCLUDES})
        endif()

        # 4) -mlong-calls: XIP .text lives far from firmware .text on nRF5340
        target_compile_options(${name}_llext_lib PRIVATE -mlong-calls)

        # 5) Keep .text.iflash as a separate section
        target_link_options(${name}_llext_lib PRIVATE -Wl,--unique=.text.iflash)

        # 6) Copy built .llext to LFS source tree so west upload_fs picks it up
        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${LFS_APPS_DIR}/${name}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${PROJECT_BINARY_DIR}/llext/${name}.llext
                ${LFS_APPS_DIR}/${name}/app.llext
            COMMENT "Copying ${name}.llext → LFS source"
        )

        # 7) Ensure it builds as part of the main firmware
        add_dependencies(app ${name})
    endfunction()
endif()

macro(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        SET(dirlist ${dirlist} ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
endmacro()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Add all folders (applications) in this folder.
message("Applications")
FOREACH(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
    include_directories(${subdir})
    message(STATUS ${subdir})
ENDFOREACH()

# Collect RELOCATE_FILES from all subdirectories and relocate them
if(RELOCATE_FILES)
  zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION EXTFLASH_TEXT NOCOPY)
  zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION EXTFLASH_RODATA NOCOPY)
  zephyr_code_relocate(FILES ${RELOCATE_FILES} LOCATION RAM_DATA)
endif()

set(RELOCATE_FILES ${RELOCATE_FILES} ${app_sources} PARENT_SCOPE)

target_sources(app PRIVATE ${app_sources})
