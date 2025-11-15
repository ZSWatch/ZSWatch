# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(display)
add_subdirectory(input)
add_subdirectory(audio)

add_subdirectory_ifdef(CONFIG_SENSOR sensor)
