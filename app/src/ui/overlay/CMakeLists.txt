# Copyright (c) 2026 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_VOICE_MEMO)
    target_sources(app PRIVATE zsw_recording_overlay.c)
    target_sources(app PRIVATE zsw_voice_memo_popup.c)
    target_sources(app PRIVATE zsw_quick_record.c)
endif()
