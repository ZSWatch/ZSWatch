# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

FILE(GLOB sensor_sources *.c)
target_sources(app PRIVATE ${sensor_sources})

add_subdirectory(watchfaces)

include_directories(watchfaces)
