# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

FILE(GLOB sensor_sources *.c)
list(REMOVE_ITEM sensor_sources ${CMAKE_CURRENT_SOURCE_DIR}/zsw_imu_step_debug.c)
target_sources(app PRIVATE ${sensor_sources})
target_sources_ifdef(CONFIG_ZSW_BMI270_STEP_DEBUG app PRIVATE zsw_imu_step_debug.c)
