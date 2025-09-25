# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_AUDIO_DMIC)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()
