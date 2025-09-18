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

#include <inttypes.h>
#include <lvgl.h>

typedef enum music_control_ui_evt_type {
    MUSIC_CONTROL_UI_CLOSE,
    MUSIC_CONTROL_UI_PLAY,
    MUSIC_CONTROL_UI_PAUSE,
    MUSIC_CONTROL_UI_NEXT_TRACK,
    MUSIC_CONTROL_UI_PREV_TRACK
} music_control_ui_evt_type_t;

typedef void(*on_music_control_ui_event_cb_t)(music_control_ui_evt_type_t evt_type);

void music_control_ui_show(lv_obj_t *root, on_music_control_ui_event_cb_t evt_cb);

void music_control_ui_remove(void);

void music_control_ui_music_info(char *track, char *artist);

void music_control_ui_set_track_progress(int percent_played);

void music_control_ui_set_time(int hour, int min, int second);

void music_control_ui_set_music_state(bool playing, int percent_played, bool shuffle);
