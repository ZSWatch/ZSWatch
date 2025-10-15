# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_ACCELEROMETER)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    # Relocate only UI to save flash as app runs in background
    FILE(GLOB xip_sources *_ui.c)
    set(RELOCATE_FILES ${RELOCATE_FILES} ${xip_sources} PARENT_SCOPE)
endif()
