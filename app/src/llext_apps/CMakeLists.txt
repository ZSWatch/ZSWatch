# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ZSW_LLEXT_APPS)
    target_sources(app PRIVATE zsw_llext_exports.c)

    # Destination for built .llext files — picked up by `west upload_fs`
    set(LFS_APPS_DIR ${CMAKE_SOURCE_DIR}/src/images/binaries/lvgl_lfs/apps)

    # --------------------------------------------------------------------------
    # Helper: register an LLEXT app target with standard options and post-build
    # copy.  Call as:
    #   add_zsw_llext_app(<name>
    #       SOURCES <source_file1> [source_file2 ...]
    #       [EXTRA_INCLUDES <dir1> <dir2> ...]
    #   )
    #
    # Multiple source files are supported (linked into a single shared library).
    # --------------------------------------------------------------------------
    function(add_zsw_llext_app name)
        cmake_parse_arguments(ARG "" "" "SOURCES;EXTRA_INCLUDES" ${ARGN})

        # 1) Create the LLEXT ELF target
        add_llext_target(${name}
            OUTPUT  ${PROJECT_BINARY_DIR}/llext/${name}.llext
            SOURCES ${ARG_SOURCES}
        )

        # 2) Common include directories (access to zsw headers, managers, etc.)
        target_include_directories(${name}_llext_lib PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/src/managers
        )

        # 3) App-specific extra includes
        if(ARG_EXTRA_INCLUDES)
            target_include_directories(${name}_llext_lib PRIVATE ${ARG_EXTRA_INCLUDES})
        endif()

        # 4) -mlong-calls: XIP .text lives far from firmware .text on nRF5340
        target_compile_options(${name}_llext_lib PRIVATE -mlong-calls)

        # 5) Copy built .llext to LFS source tree so west upload_fs picks it up
        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${LFS_APPS_DIR}/${name}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${PROJECT_BINARY_DIR}/llext/${name}.llext
                ${LFS_APPS_DIR}/${name}/app.llext
            COMMENT "Copying ${name}.llext → LFS source"
        )

        # 6) Ensure it builds as part of the main firmware
        add_dependencies(app ${name})
    endfunction()

    # --------------------------------------------------------------------------
    # LLEXT apps
    # --------------------------------------------------------------------------

    add_zsw_llext_app(about_ext
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/about_ext/about_ext_app.c
    )

    add_zsw_llext_app(battery_real_ext
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/battery_real_ext/battery_real_ext_app.c
            ${CMAKE_CURRENT_SOURCE_DIR}/battery_real_ext/battery_ui.c
        EXTRA_INCLUDES
            ${CMAKE_SOURCE_DIR}/src/applications
            ${CMAKE_SOURCE_DIR}/src/ui
            ${CMAKE_CURRENT_SOURCE_DIR}/battery_real_ext
    )

endif()
