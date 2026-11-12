# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_WATCHFACE_DIORAMA)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
    # Every pixel of the face comes from here, it is worth optimizing for speed
    set_source_files_properties(diorama_scene.c TARGET_DIRECTORY app PROPERTIES COMPILE_OPTIONS "-O2")
endif()
