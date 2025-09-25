# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_INFO)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    # Export none critical files for relocation to save flash
    # App runs in background can only XIP ui.
    FILE(GLOB xip_sources *_ui.c)
    set(RELOCATE_FILES ${RELOCATE_FILES} ${xip_sources} PARENT_SCOPE)
endif()
