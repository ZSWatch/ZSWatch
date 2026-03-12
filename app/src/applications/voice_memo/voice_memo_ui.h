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

#include <lvgl.h>
#include "managers/zsw_recording_manager_store.h"

typedef void (*voice_memo_ui_start_cb_t)(void);
typedef void (*voice_memo_ui_stop_cb_t)(void);
typedef void (*voice_memo_ui_delete_cb_t)(const char *filename);
typedef void (*voice_memo_ui_back_confirm_cb_t)(bool save);

typedef struct {
    voice_memo_ui_start_cb_t       on_start_recording;
    voice_memo_ui_stop_cb_t        on_stop_recording;
    voice_memo_ui_delete_cb_t      on_delete;
    voice_memo_ui_back_confirm_cb_t on_back_during_recording;
} voice_memo_ui_callbacks_t;

/** Show the voice memo main screen (recording list). */
void voice_memo_ui_show(lv_obj_t *root, const voice_memo_ui_callbacks_t *cbs);

/** Remove all UI objects. */
void voice_memo_ui_remove(void);

/** Switch to recording screen. */
void voice_memo_ui_show_recording(void);

/** Switch back to list screen. */
void voice_memo_ui_show_list(void);

/** Update the recording elapsed time display. remaining_s is seconds left until max duration. */
void voice_memo_ui_update_time(uint32_t elapsed_ms, uint32_t remaining_s);

/** Update audio level indicator (0-100, peak amplitude percentage). */
void voice_memo_ui_update_level(uint8_t level);

/** Update the recording list with current entries. kb_per_sec is storage rate for remaining calc. */
void voice_memo_ui_update_list(const zsw_recording_entry_t *entries, int count,
                               uint32_t free_space_kb, uint32_t kb_per_sec);

/** Show confirmation dialog when back is pressed during recording. */
void voice_memo_ui_show_back_confirm(void);
