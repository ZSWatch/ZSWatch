# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ZSW_LLEXT_APPS)
    add_zsw_llext_app(trivia_ext
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/trivia_app.c
            ${CMAKE_CURRENT_SOURCE_DIR}/trivia_ui.c
        EXTRA_INCLUDES
            ${CMAKE_SOURCE_DIR}/src/images
            ${CMAKE_SOURCE_DIR}/src/ble
    )
elseif(CONFIG_APPLICATIONS_USE_TRIVIA)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/trivia_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/trivia_ui.c
    )
    set(RELOCATE_FILES ${RELOCATE_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/trivia_ui.c
        PARENT_SCOPE)
endif()
