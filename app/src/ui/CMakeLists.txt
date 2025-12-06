# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

FILE(GLOB ui_sources *.c)
target_sources(app PRIVATE ${ui_sources})

add_subdirectory(watchfaces)

include_directories(watchfaces)
