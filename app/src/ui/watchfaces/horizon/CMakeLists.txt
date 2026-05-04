# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_WATCHFACE_HORIZON)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    FILE(GLOB font_sources ui_font_*.c)
    zephyr_code_relocate(FILES ${font_sources} LOCATION EXTFLASH_RODATA NOCOPY)
    zephyr_code_relocate(FILES ${font_sources} LOCATION RAM_DATA)
endif()
