# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ZSW_LLEXT_APPS)
    add_zsw_llext_app(compass_ext
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/compass_app.c
            ${CMAKE_CURRENT_SOURCE_DIR}/compass_ui.c
        EXTRA_INCLUDES
            ${CMAKE_SOURCE_DIR}/src/applications
            ${CMAKE_SOURCE_DIR}/src/images
            ${CMAKE_SOURCE_DIR}/src/sensors
            ${CMAKE_SOURCE_DIR}/src/sensor_fusion
            ${CMAKE_SOURCE_DIR}/src/ui
            ${CMAKE_SOURCE_DIR}/src/ui/popup
    )
elseif(CONFIG_APPLICATIONS_USE_COMPASS)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/compass_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compass_ui.c
    )
    set(RELOCATE_FILES ${RELOCATE_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/compass_app.c
        ${CMAKE_CURRENT_SOURCE_DIR}/compass_ui.c
        PARENT_SCOPE)
endif()
