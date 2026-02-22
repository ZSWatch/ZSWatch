# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ZSW_LLEXT_APPS)
    add_zsw_llext_app(weather_ext
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/weather_app.c
            ${CMAKE_CURRENT_SOURCE_DIR}/weather_ui.c
        EXTRA_INCLUDES
            ${CMAKE_SOURCE_DIR}/src/applications
            ${CMAKE_SOURCE_DIR}/src/images
            ${CMAKE_SOURCE_DIR}/src/ble
            ${CMAKE_SOURCE_DIR}/src/ui
    )
elseif(CONFIG_STORE_IMAGES_EXTERNAL_FLASH OR CONFIG_ARCH_POSIX)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/weather_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/weather_ui.c
    )
    set(RELOCATE_FILES ${RELOCATE_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/weather_ui.c
        PARENT_SCOPE)
endif()
