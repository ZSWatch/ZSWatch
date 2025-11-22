# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(${PROJECT_SOURCE_DIR}/../app/src/ext_drivers/BMP5-Sensor-API)
zephyr_sources(${PROJECT_SOURCE_DIR}/../app/src/ext_drivers/BMP5-Sensor-API/bmp5.c)

zephyr_sources_ifdef(CONFIG_ZSW_BMP581 zsw_bosch_bmp581.c)
zephyr_sources_ifdef(CONFIG_ZSW_BMP581_EMUL zsw_bosch_bmp581_emul.c)
