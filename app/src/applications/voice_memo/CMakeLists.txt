# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_VOICE_MEMO)
    target_sources(app PRIVATE
        voice_memo_app.c
        voice_memo_ui.c
        voice_memo_shell.c
    )

    # Relocate UI code to external flash when available
    set(RELOCATE_FILES ${RELOCATE_FILES}
        ${CMAKE_CURRENT_SOURCE_DIR}/voice_memo_ui.c
        PARENT_SCOPE)
endif()
