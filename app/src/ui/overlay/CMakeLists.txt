# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_VOICE_MEMO)
    target_sources(app PRIVATE zsw_recording_overlay.c)
    target_sources(app PRIVATE zsw_voice_memo_popup.c)

    if(CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_QUICK_RECORD_BUTTON GREATER 0)
        target_sources(app PRIVATE zsw_quick_record.c)
    endif()
endif()
