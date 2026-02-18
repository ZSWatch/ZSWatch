# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

# battery_ui.c/h live in the LLEXT app directory (shared with battery_real_ext).
set(BATTERY_UI_DIR ${CMAKE_SOURCE_DIR}/src/llext_apps/battery_real_ext)

FILE(GLOB app_sources *.c)
target_sources(app PRIVATE ${app_sources} ${BATTERY_UI_DIR}/battery_ui.c)
target_include_directories(app PRIVATE ${BATTERY_UI_DIR})

# Export UI files for relocation to external flash (save internal flash)
set(RELOCATE_FILES ${RELOCATE_FILES} ${BATTERY_UI_DIR}/battery_ui.c PARENT_SCOPE)
