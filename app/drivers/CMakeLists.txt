# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(audio)

add_subdirectory_ifdef(CONFIG_DISPLAY display)
add_subdirectory_ifdef(CONFIG_INPUT input)
add_subdirectory_ifdef(CONFIG_SENSOR sensor)
