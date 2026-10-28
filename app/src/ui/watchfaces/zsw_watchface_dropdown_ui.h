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

/**
 * @brief Add the dropdown to the watchface root page.
 *
 * @param shown_cb Optional, called with true when the dropdown opens and false when it closes.
 */
void zsw_watchface_dropdown_ui_add(lv_obj_t *root_page, watchface_app_evt_listener evt_cb, int brightness,
                                   void (*shown_cb)(bool shown));
void zsw_watchface_dropdown_ui_set_music_info(char *track_name, char *artist);
void zsw_watchface_dropdown_ui_set_battery_info(uint8_t battery_percent, bool is_charging, int tte, int ttf);
void zsw_watchface_dropdown_ui_remove(void);

/**
 * @brief Close the dropdown if it is open.
 * @return true if it was open.
 */
bool zsw_watchface_dropdown_ui_close(void);
