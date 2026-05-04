# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_CHAT)
    target_sources(app PRIVATE
        chat_app.c
        chat_ui.c
    )

    # Relocate UI code to external flash when available
    set(RELOCATE_FILES ${RELOCATE_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/chat_ui.c
        PARENT_SCOPE)
endif()
