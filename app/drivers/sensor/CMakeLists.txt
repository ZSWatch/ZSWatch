# Copyright (c) 2023, Daniel Kampert
#
# SPDX-License-Identifier: Apache-2.0
#

# TODO:
# Weird compilation issue when not using
# zephyr_library()
# zephyr_library_sources(apds9306.c)
#
# -> ..add_library cannot create target "..__app__drivers__sensor" because another
# -> target with the same name already exists.  The existing target is a static...
#
add_subdirectory_ifdef(CONFIG_APDS9306 apds9306)
add_subdirectory_ifdef(CONFIG_BME68X_EXT_IAQ bme68x_iaq)
add_subdirectory_ifdef(CONFIG_BMP581 bmp581)
add_subdirectory_ifdef(CONFIG_BMI270_PLUS bmi270)