/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project, Leonardo Bispo.
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

typedef enum trivia_button {
    TRUE_BUTTON = 0,
    FALSE_BUTTON,
    PLAY_MORE_BUTTON,
    CLOSE_BUTTON,
} trivia_button_t;

typedef void (*on_button_press_cb_t)(trivia_button_t trivia_button);

void trivia_ui_show(lv_obj_t *root, on_button_press_cb_t on_button_click_cb);

void trivia_ui_remove(void);

void trivia_ui_update_question(uint8_t *buff);

void trivia_ui_guess_feedback(bool correct);

void trivia_ui_close_popup(void);

void trivia_ui_not_supported(void);
