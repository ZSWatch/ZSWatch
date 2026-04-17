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

#pragma once

#include <lvgl.h>

typedef enum {
	DA7212_TEST_UI_STATE_IDLE,
	DA7212_TEST_UI_STATE_PLAYING_MELODY,
	DA7212_TEST_UI_STATE_RECORDING_VOICE,
	DA7212_TEST_UI_STATE_PLAYING_VOICE,
} da7212_test_ui_state_t;

typedef void (*da7212_test_ui_evt_cb_t)(void);

void da7212_test_ui_show(lv_obj_t *root,
						 da7212_test_ui_evt_cb_t melody_cb,
						 da7212_test_ui_evt_cb_t voice_demo_cb,
						 da7212_test_ui_evt_cb_t stop_cb);
void da7212_test_ui_remove(void);
void da7212_test_ui_set_status(const char *text);
void da7212_test_ui_set_state(da7212_test_ui_state_t state);
