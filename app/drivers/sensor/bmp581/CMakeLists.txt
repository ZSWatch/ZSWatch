# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(${PROJECT_SOURCE_DIR}/src/ext_drivers/BMP5-Sensor-API)
zephyr_sources(${PROJECT_SOURCE_DIR}/src/ext_drivers/BMP5-Sensor-API/bmp5.c)

zephyr_sources(bosch_zsw_bmp581.c)
