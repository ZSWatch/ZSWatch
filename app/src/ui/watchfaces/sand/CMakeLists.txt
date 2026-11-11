# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_WATCHFACE_SAND)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
    # The simulation runs every frame while the sand moves, it is worth optimizing for speed
    set_source_files_properties(sand_sim.c TARGET_DIRECTORY app PROPERTIES COMPILE_OPTIONS "-O2")
endif()
