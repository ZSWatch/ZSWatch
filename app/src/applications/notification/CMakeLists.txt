# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

FILE(GLOB app_sources
    *.c
    ui_export/screens/*
    ui_export/images/*
)

target_sources(app PRIVATE ${app_sources})

# Export none critical files for relocation to save flash
set(RELOCATE_FILES ${RELOCATE_FILES} ${app_sources} PARENT_SCOPE)
