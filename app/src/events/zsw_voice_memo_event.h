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

enum zsw_voice_memo_recording_state {
    ZSW_VOICE_MEMO_RECORDING_STARTED,
    ZSW_VOICE_MEMO_RECORDING_STOPPED,
    ZSW_VOICE_MEMO_RECORDING_ABORTED,
};

struct zsw_voice_memo_recording_event {
    enum zsw_voice_memo_recording_state state;
    char filename[32];
    uint32_t duration_ms;
    uint32_t size_bytes;
    uint32_t timestamp;
};

struct zsw_voice_memo_result_event {
    char title[128];
    char filename[32];
    char action_type[20];   /* "calendar_event", "task", "reminder", or "" */
    char datetime[32];      /* ISO datetime string or "" */
};
