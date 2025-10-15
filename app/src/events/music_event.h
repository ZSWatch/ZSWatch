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

typedef enum {
    MUSIC_CONTROL_UI_CLOSE,
    MUSIC_CONTROL_UI_PLAY,
    MUSIC_CONTROL_UI_PAUSE,
    MUSIC_CONTROL_UI_NEXT_TRACK,
    MUSIC_CONTROL_UI_PREV_TRACK
} music_control_ui_evt_type_t;

struct music_event {
    music_control_ui_evt_type_t control_type;
};
