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

/* Internal header — only include from zsw_recording_manager.c */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define VOICE_MEMO_DIR            "/user/recordings"
#define VOICE_MEMO_MAX_FILENAME   32
#define VOICE_MEMO_MAGIC          "ZSWO"
#define VOICE_MEMO_HEADER_VERSION 1
#define VOICE_MEMO_HEADER_SIZE    32

typedef struct __attribute__((packed)) {
    uint8_t  magic[4];
    uint16_t version;
    uint16_t sample_rate;
    uint16_t frame_size;
    uint16_t reserved1;
    uint32_t bitrate;
    uint32_t timestamp;
    uint32_t total_frames;
    uint32_t duration_ms;
    uint32_t reserved2;
} zsw_recording_manager_store_header_t;

_Static_assert(sizeof(zsw_recording_manager_store_header_t) == VOICE_MEMO_HEADER_SIZE,
               "zsw_recording_manager_store_header_t must be exactly 32 bytes");

typedef struct {
    char     filename[VOICE_MEMO_MAX_FILENAME];
    uint32_t timestamp;
    uint32_t duration_ms;
    uint32_t size_bytes;
} zsw_recording_entry_t;

int zsw_recording_manager_store_init(void);
int zsw_recording_manager_store_start_recording(void);
int zsw_recording_manager_store_write_frame(const uint8_t *opus_data, size_t len);
int zsw_recording_manager_store_flush(void);
int zsw_recording_manager_store_stop_recording(uint32_t *out_duration_ms, uint32_t *out_size_bytes);
int zsw_recording_manager_store_abort_recording(void);
int zsw_recording_manager_store_list(zsw_recording_entry_t *entries, size_t max_entries);
int zsw_recording_manager_store_delete(const char *filename);
int zsw_recording_manager_store_get_free_space(uint32_t *free_bytes);
int zsw_recording_manager_store_get_count(void);
const char *zsw_recording_manager_store_get_current_filename(void);
bool zsw_recording_manager_store_is_recording(void);
uint32_t zsw_recording_manager_store_get_unix_timestamp(void);
