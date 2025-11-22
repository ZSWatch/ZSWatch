# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(${PROJECT_SOURCE_DIR}/../app/src/ext_drivers/BMI270-Sensor-API)
zephyr_sources(${PROJECT_SOURCE_DIR}/../app/src/ext_drivers/BMI270-Sensor-API/bmi2.c)
zephyr_sources(${PROJECT_SOURCE_DIR}/../app/src/ext_drivers/BMI270-Sensor-API/bmi270.c)

zephyr_sources_ifdef(CONFIG_ZSW_BMI270_TRIGGER trigger/zsw_bosch_bmi270_interrupt.c)
zephyr_sources_ifdef(CONFIG_ZSW_BMI270 zsw_bosch_bmi270.c private/zsw_bosch_bmi270_config.c)