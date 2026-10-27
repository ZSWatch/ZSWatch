# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_TILES)
    target_sources(app PRIVATE tiles_app.c tiles_ui.c)

    # Export none critical files for relocation to save flash
    set(RELOCATE_FILES ${RELOCATE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/tiles_ui.c PARENT_SCOPE)
endif()
