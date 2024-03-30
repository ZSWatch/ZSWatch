/*
 * This file is part of ZSWatch project <https://github.com/jakkra/ZSWatch/>.
 * Copyright (c) 2023 Jakob Krantz.
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

#include "utils/zsw_ui_utils.h"
#include "popup/zsw_popup_window.h"
#include "notification/zsw_popup_notifcation.h"

static inline lv_color_t zsw_color_red(void)
{
    return lv_color_make(0xff, 0xba, 0xaf);
}

static inline lv_color_t zsw_color_blue(void)
{
    return lv_color_make(0x9e, 0xc8, 0xf6);
}

static inline lv_color_t zsw_color_gray(void)
{
    return lv_color_make(0x49, 0x50, 0x60);
}

static inline lv_color_t zsw_color_dark_gray(void)
{
    return lv_color_make(0x30, 0x34, 0x3f);
}

static inline lv_color_t zsw_color_bg(void)
{
    return zsw_color_dark_gray();
}
