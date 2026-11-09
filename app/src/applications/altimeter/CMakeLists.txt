# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

FILE(GLOB app_sources *.c)
target_sources(app PRIVATE ${app_sources})

# Export none critical files for relocation to save flash
FILE(GLOB xip_sources *_ui.c)
set(RELOCATE_FILES ${RELOCATE_FILES} ${xip_sources} PARENT_SCOPE)
