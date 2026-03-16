# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_VOICE_MEMO)
    target_sources(app PRIVATE
        voice_memo_app.c
        voice_memo_ui.c
    )
endif()
