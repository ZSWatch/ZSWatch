# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_MCUMGR OR CONFIG_ARCH_POSIX)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    # Only the UI goes to XIP. update_app.c holds MCUmgr callbacks that run
    # on any SMP traffic, also while XIP is off.
    set(RELOCATE_FILES ${RELOCATE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/update_ui.c PARENT_SCOPE)
endif()
