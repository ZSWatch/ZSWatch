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

#include <lvgl.h>

typedef void(*on_watchface_selected_cb_t)(int watchface_index);

void watchface_picker_ui_show(lv_obj_t *root, on_watchface_selected_cb_t select_cb);
void watchface_picker_ui_add_watchface(const lv_img_dsc_t *src, const char *name, int index);
void watchface_picker_ui_set_selected(int index);
void watchface_picker_ui_remove(void);
