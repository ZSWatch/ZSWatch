# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_2048)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    set(LV_USE_100ASK_2048 "1")
    target_include_directories(app PRIVATE 2048_lib/src/lv_100ask_2048)
    target_sources(app PRIVATE 2048_lib/src/lv_100ask_2048/lv_100ask_2048.c)
    # File contains some build warnings, ignore those.
    set_source_files_properties(2048_lib/src/lv_100ask_2048/lv_100ask_2048.c TARGET_DIRECTORY app PROPERTIES COMPILE_OPTIONS -w)
endif()
