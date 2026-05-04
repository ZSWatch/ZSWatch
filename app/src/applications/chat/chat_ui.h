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

typedef enum {
    CHAT_UI_STATE_IDLE,
    CHAT_UI_STATE_LISTENING,
    CHAT_UI_STATE_UPLOADING,
    CHAT_UI_STATE_THINKING,
    CHAT_UI_STATE_SPEAKING,
    CHAT_UI_STATE_ERROR,
} chat_ui_state_t;

typedef struct {
    void (*on_record_start)(void);
    void (*on_record_stop)(void);
    void (*on_cancel)(void);
    void (*on_retry)(void);
} chat_ui_callbacks_t;

/**
 * @brief Create the chat UI on the given root object.
 */
void chat_ui_show(lv_obj_t *root, const chat_ui_callbacks_t *callbacks);

/**
 * @brief Remove and clean up all chat UI objects.
 */
void chat_ui_remove(void);

/**
 * @brief Set the current UI state (changes visible elements).
 */
void chat_ui_set_state(chat_ui_state_t state);

/**
 * @brief Update the transcript preview text.
 */
void chat_ui_set_transcript(const char *text);

/**
 * @brief Update the error message text.
 */
void chat_ui_set_error(const char *text);

/**
 * @brief Update the recording level indicator (0-100).
 */
void chat_ui_update_level(int level);

/**
 * @brief Update the recording elapsed time in milliseconds.
 */
void chat_ui_update_recording_time(uint32_t elapsed_ms);
