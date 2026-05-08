# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_LEA_ASSISTANT)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    set(RELOCATE_FILES ${RELOCATE_FILES} ${app_sources} PARENT_SCOPE)

    # Internal BT headers needed for PAST support
    target_include_directories(app PRIVATE ${ZEPHYR_BASE}/subsys/bluetooth)
endif()
