# Copyright (c) 2023, Daniel Kampert
#
# SPDX-License-Identifier: Apache-2.0
#

# TODO: Doesn´t work with CMakeList.txt of the drivers directory -> Check it
#zephyr_library()

zephyr_include_directories(${PROJECT_SOURCE_DIR}/src/ext_drivers/BMP5-Sensor-API)
zephyr_sources(${PROJECT_SOURCE_DIR}/src/ext_drivers/BMP5-Sensor-API/bmp5.c)

zephyr_sources(bosch_bmp581.c)