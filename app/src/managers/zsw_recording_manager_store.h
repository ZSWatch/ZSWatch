/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2026 ZSWatch Project.
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

/**
 * @file zsw_recording_manager_store.h
 * @brief Low-level storage for voice recordings in .zsw_opus format on LittleFS.
 *
 * Internal header — only include from zsw_recording_manager.c.
 * Handles file creation, buffered writes, crash recovery, and directory listing.
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

/** Maximum number of stored recordings. */
#define ZSW_RECORDING_MAX_FILES         50
/** Minimum free space required to start a recording (KB). */
#define ZSW_RECORDING_MIN_FREE_SPACE_KB 500

typedef struct __attribute__((packed))
{
    uint8_t  magic[4];
    uint16_t version;
    uint16_t sample_rate;
    uint16_t frame_size;
    uint16_t reserved1;
    uint32_t bitrate;
    uint32_t timestamp;
    uint32_t total_frames;   /**< 0xFFFFFFFF means the file was not finalized (dirty). */
    uint32_t duration_ms;    /**< 0xFFFFFFFF means the file was not finalized (dirty). */
    uint32_t reserved2;
}
zsw_recording_manager_store_header_t;

_Static_assert(sizeof(zsw_recording_manager_store_header_t) == VOICE_MEMO_HEADER_SIZE,
               "zsw_recording_manager_store_header_t must be exactly 32 bytes");

/** @brief A single recording entry as returned by the list function. */
typedef struct {
    char     filename[VOICE_MEMO_MAX_FILENAME];
    uint32_t timestamp;
    uint32_t duration_ms;
    uint32_t size_bytes;
} zsw_recording_entry_t;

/** @brief Initialize storage. Scans for and repairs dirty (incomplete) files. */
int zsw_recording_manager_store_init(void);

/** @brief Create a new recording file with a dirty header. */
int zsw_recording_manager_store_start_recording(void);

/** @brief Append an encoded Opus frame (buffered, flushed when buffer is full). */
int zsw_recording_manager_store_write_frame(const uint8_t *opus_data, size_t len);

/** @brief Force-flush the write buffer to flash. */
int zsw_recording_manager_store_flush(void);

/** @brief Finalize the recording: update header with frame count and duration. */
int zsw_recording_manager_store_stop_recording(uint32_t *out_duration_ms, uint32_t *out_size_bytes);

/** @brief Discard the current recording and delete the file. */
int zsw_recording_manager_store_abort_recording(void);

/** @brief List all stored recordings, sorted by timestamp. */
int zsw_recording_manager_store_list(zsw_recording_entry_t *entries, size_t max_entries);

/** @brief Delete a recording by filename (without extension). */
int zsw_recording_manager_store_delete(const char *filename);

/** @brief Get free space on the recording partition. */
int zsw_recording_manager_store_get_free_space(uint32_t *free_bytes);

/** @brief Get the number of stored recordings. */
int zsw_recording_manager_store_get_count(void);

/** @brief Get the filename of the recording currently being written. */
const char *zsw_recording_manager_store_get_current_filename(void);

/** @brief Check if a recording is currently being written. */
bool zsw_recording_manager_store_is_recording(void);

/** @brief Get current UNIX timestamp from the RTC. */
uint32_t zsw_recording_manager_store_get_unix_timestamp(void);
