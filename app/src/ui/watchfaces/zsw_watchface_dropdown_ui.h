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

#include "applications/watchface/watchface_app.h"

void zsw_watchface_dropdown_ui_add(lv_obj_t *root_page, watchface_app_evt_listener evt_cb, int brightness);
void zsw_watchface_dropdown_ui_set_music_info(char *track_name, char *artist);
void zsw_watchface_dropdown_ui_set_battery_info(uint8_t battery_percent, bool is_charging, int tte, int ttf);
void zsw_watchface_dropdown_ui_remove(void);
