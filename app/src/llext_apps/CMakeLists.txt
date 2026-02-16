# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ZSW_LLEXT_APPS)
    target_sources(app PRIVATE zsw_llext_exports.c)

    # Build the about_ext LLEXT app
    add_llext_target(about_ext
        OUTPUT  ${PROJECT_BINARY_DIR}/llext/about_ext.llext
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/about_ext/about_ext_app.c
    )
    # Include app source dirs so LLEXT can find headers
    target_include_directories(about_ext_llext_lib PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/managers
    )

    # Generate C include file from the built LLEXT binary for embedded testing
    generate_inc_file_for_target(app
        ${PROJECT_BINARY_DIR}/llext/about_ext.llext
        ${ZEPHYR_BINARY_DIR}/include/generated/about_ext.inc
    )

    # Ensure LLEXT apps are built as part of the main firmware build
    add_dependencies(app about_ext)
endif()
