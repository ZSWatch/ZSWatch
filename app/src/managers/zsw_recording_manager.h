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

typedef struct {
    char filename[32];
    uint32_t duration_ms;
    uint32_t size_bytes;
    uint32_t timestamp;
} zsw_recording_result_t;

int zsw_recording_manager_init(void);
int zsw_recording_manager_start(void);
int zsw_recording_manager_stop(void);
int zsw_recording_manager_abort(void);
bool zsw_recording_manager_is_recording(void);
uint8_t zsw_recording_manager_get_audio_level(void);
uint32_t zsw_recording_manager_get_elapsed_ms(void);

/* Delegated store operations */
int zsw_recording_manager_list(zsw_recording_entry_t *entries, size_t max_entries);
int zsw_recording_manager_delete(const char *filename);
int zsw_recording_manager_get_free_space(uint32_t *free_bytes);
int zsw_recording_manager_get_count(void);
