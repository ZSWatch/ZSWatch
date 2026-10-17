# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_HR)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()

# No UI files to relocate - UI is handled by LVGL Editor generated code
