# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_IAQ)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
    add_subdirectory(ui_export)
endif()
