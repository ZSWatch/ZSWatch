# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(${PROJECT_SOURCE_DIR}/../app/src/ext_drivers/BMI270-Sensor-API)
zephyr_sources(${PROJECT_SOURCE_DIR}/../app/src/ext_drivers/BMI270-Sensor-API/bmi2.c)
zephyr_sources(${PROJECT_SOURCE_DIR}/../app/src/ext_drivers/BMI270-Sensor-API/bmi270.c)

if(CONFIG_BMI270_PLUS_TRIGGER)
    zephyr_sources(trigger/bosch_bmi270_interrupt.c)
endif()

zephyr_sources(bosch_bmi270.c private/bosch_bmi270_config.c)
