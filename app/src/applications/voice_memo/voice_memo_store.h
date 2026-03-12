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
#include <stdbool.h>
#include <stddef.h>

#define VOICE_MEMO_DIR            "/user/recordings"
#define VOICE_MEMO_MAX_FILENAME   32
#define VOICE_MEMO_MAGIC          "ZSWO"
#define VOICE_MEMO_HEADER_VERSION 1
#define VOICE_MEMO_HEADER_SIZE    32

/** On-disk file header for .zsw_opus recordings. */
typedef struct __attribute__((packed)) {
    uint8_t  magic[4];        /* "ZSWO" */
    uint16_t version;         /* 1 */
    uint16_t sample_rate;     /* 16000 */
    uint16_t frame_size;      /* 160 (samples per Opus frame) */
    uint16_t reserved1;
    uint32_t bitrate;         /* 32000 */
    uint32_t timestamp;       /* Unix epoch seconds */
    uint32_t total_frames;    /* 0xFFFFFFFF until clean stop */
    uint32_t duration_ms;     /* 0xFFFFFFFF until clean stop */
    uint32_t reserved2;       /* Padding to 32 bytes */
} voice_memo_header_t;

_Static_assert(sizeof(voice_memo_header_t) == VOICE_MEMO_HEADER_SIZE,
               "voice_memo_header_t must be exactly 32 bytes");

/** Entry returned by voice_memo_store_list(). */
typedef struct {
    char     filename[VOICE_MEMO_MAX_FILENAME];
    uint32_t timestamp;
    uint32_t duration_ms;
    uint32_t size_bytes;
} voice_memo_entry_t;

/** Initialize recording storage (create directory, repair dirty files). */
int voice_memo_store_init(void);

/** Start a new recording. Returns 0 on success. */
int voice_memo_store_start_recording(void);

/** Write encoded Opus frame data (2-byte length prefix + data). */
int voice_memo_store_write_frame(const uint8_t *opus_data, size_t len);

/** Flush any buffered data to flash. */
int voice_memo_store_flush(void);

/** Finalize and close the current recording. Returns 0 on success. */
int voice_memo_store_stop_recording(uint32_t *out_duration_ms, uint32_t *out_size_bytes);

/** Abort the current recording: close and delete the file without saving. */
int voice_memo_store_abort_recording(void);

/** List recordings. Fills entries[], returns count (or negative error). */
int voice_memo_store_list(voice_memo_entry_t *entries, size_t max_entries);

/** Delete a recording by filename (without path/extension). */
int voice_memo_store_delete(const char *filename);

/** Get free space in bytes. */
int voice_memo_store_get_free_space(uint32_t *free_bytes);

/** Get total recording count. */
int voice_memo_store_get_count(void);

/** Get the full path of the current recording file (NULL if not recording). */
const char *voice_memo_store_get_current_filename(void);

/** Check if currently recording. */
bool voice_memo_store_is_recording(void);

/** Get the current Unix timestamp (seconds since epoch) from the RTC. */
uint32_t voice_memo_store_get_unix_timestamp(void);
