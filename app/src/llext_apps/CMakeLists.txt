# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

# LLEXT app sources live alongside their built-in counterparts in
# src/applications/<app>/. Each app's CMakeLists.txt decides whether
# to build as LLEXT or normal based on CONFIG_ZSW_LLEXT_APPS.
#
# This file only compiles the shared LLEXT export table.

if(CONFIG_ZSW_LLEXT_APPS)
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/zsw_llext_exports.c
        ${CMAKE_CURRENT_SOURCE_DIR}/zsw_llext_log.c
    )
endif()
