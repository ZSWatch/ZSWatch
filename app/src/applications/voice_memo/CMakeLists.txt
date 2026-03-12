# Copyright (c) 2025 ZSWatch Project
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_APPLICATIONS_USE_VOICE_MEMO)
    FILE(GLOB app_sources *.c)
    target_sources(app PRIVATE ${app_sources})

    # NOTE: Voice memo is intentionally NOT added to RELOCATE_FILES (XIP).
    # During recording, voice_memo_store writes to LittleFS on the same QSPI
    # flash that serves XIP. QSPI writes block XIP reads, so any voice_memo
    # function executing from XIP (e.g. mic_data_callback called by audio_mic
    # thread) would cause a BUS FAULT. Keeping voice_memo in internal flash
    # avoids this XIP contention.
endif()
