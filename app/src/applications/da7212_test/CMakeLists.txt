# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_DT_HAS_DLG_DA7212_ENABLED)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})
endif()
