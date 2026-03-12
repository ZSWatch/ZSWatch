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
#include "zsw_recording_manager_store.h"

/** Maximum recording duration in seconds before auto-stop. */
#define ZSW_RECORDING_MAX_DURATION_S    300

/** @brief Result of a completed recording, used for BLE notification. */
typedef struct {
    char filename[32];
    uint32_t duration_ms;
    uint32_t size_bytes;
    uint32_t timestamp;
} zsw_recording_result_t;

/** @brief Initialize recording manager and storage. Call once at startup. */
int zsw_recording_manager_init(void);

/** @brief Start a new recording. Returns 0 on success, negative on error. */
int zsw_recording_manager_start(void);

/** @brief Stop recording, finalize file, and publish zbus event. */
int zsw_recording_manager_stop(void);

/** @brief Abort recording and discard the file. */
int zsw_recording_manager_abort(void);

/** @brief Check if a recording is currently in progress. */
bool zsw_recording_manager_is_recording(void);

/** @brief Get current audio input level (0-100, peak amplitude percentage). */
uint8_t zsw_recording_manager_get_audio_level(void);

/** @brief Get elapsed recording time in milliseconds. Returns 0 if not recording. */
uint32_t zsw_recording_manager_get_elapsed_ms(void);

/** @brief List stored recordings. Returns count on success, negative on error. */
int zsw_recording_manager_list(zsw_recording_entry_t *entries, size_t max_entries);

/** @brief Delete a recording by filename. */
int zsw_recording_manager_delete(const char *filename);

/** @brief Get free storage space in bytes. */
int zsw_recording_manager_get_free_space(uint32_t *free_bytes);

/** @brief Get number of stored recordings. */
int zsw_recording_manager_get_count(void);
