# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ZSW_LLEXT_APPS)
    add_zsw_llext_app(calculator_ext
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/calculator_app.c
            ${CMAKE_CURRENT_SOURCE_DIR}/calculator_ui.c
            ${CMAKE_CURRENT_SOURCE_DIR}/smf_calculator_thread.c
        EXTRA_INCLUDES
            ${CMAKE_SOURCE_DIR}/src/applications
            ${CMAKE_SOURCE_DIR}/src/ui
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    # Export none critical files for relocation to save flash
    set(RELOCATE_FILES ${RELOCATE_FILES} ${app_sources} PARENT_SCOPE)
endif()
