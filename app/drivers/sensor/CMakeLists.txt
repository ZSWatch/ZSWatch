# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_BME68X_EXT_IAQ bme68x_iaq)
add_subdirectory_ifdef(CONFIG_ZSW_BMP581 bmp581)
add_subdirectory_ifdef(CONFIG_BMI270_PLUS bmi270)
add_subdirectory_ifdef(CONFIG_APDS9306 apds9306)
