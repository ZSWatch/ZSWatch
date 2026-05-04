/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>

#define ZSW_CHAT_MAX_TRANSCRIPT_LEN     200
#define ZSW_CHAT_MAX_PATH_LEN           64
#define ZSW_CHAT_MAX_CODEC_LEN          16
#define ZSW_CHAT_MAX_ERROR_LEN          64

/** Chat state as communicated from the companion app */
enum zsw_chat_state {
    ZSW_CHAT_STATE_IDLE,
    ZSW_CHAT_STATE_UPLOADING,
    ZSW_CHAT_STATE_TRANSCRIBING,
    ZSW_CHAT_STATE_THINKING,
    ZSW_CHAT_STATE_GENERATING_TTS,
    ZSW_CHAT_STATE_UPLOADING_REPLY,
    ZSW_CHAT_STATE_REPLY_READY,
    ZSW_CHAT_STATE_ERROR,
};

/** Event published when the companion app sends a chat state update */
struct zsw_chat_state_event {
    enum zsw_chat_state state;
    uint32_t session_id;
};

/** Event published when the companion app sends back the recognized transcript */
struct zsw_chat_transcript_event {
    uint32_t session_id;
    char transcript[ZSW_CHAT_MAX_TRANSCRIPT_LEN];
};

/** Event published when the reply audio file is ready for playback */
struct zsw_chat_reply_ready_event {
    uint32_t session_id;
    char path[ZSW_CHAT_MAX_PATH_LEN];
    char codec[ZSW_CHAT_MAX_CODEC_LEN];
    uint32_t sample_rate;
    uint32_t duration_ms;
};

/** Event published when a chat error occurs */
struct zsw_chat_error_event {
    uint32_t session_id;
    char message[ZSW_CHAT_MAX_ERROR_LEN];
};
