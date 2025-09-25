# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if (CONFIG_RTC_ALARM)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    # Export none critical files for relocation to save flash
    FILE(GLOB relocate_sources *_ui.c)
    set(RELOCATE_FILES ${RELOCATE_FILES} ${relocate_sources} PARENT_SCOPE)
endif()
