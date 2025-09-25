# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

FILE(GLOB app_sources
    *.c
    ui_export/screens/*
    ui_export/images/*
)

target_sources(app PRIVATE ${app_sources})


# Relocate only UI to save flash as app runs in background
FILE(GLOB relocate_sources
    ui_export/screens/*
    ui_export/images/*
)

set(RELOCATE_FILES ${RELOCATE_FILES} ${relocate_sources} PARENT_SCOPE)
