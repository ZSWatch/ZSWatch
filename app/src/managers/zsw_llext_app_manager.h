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

/**
 * @brief Initialize the LLEXT app manager.
 *
 * Scans /lfs/apps/ for installed LLEXT apps, loads them,
 * and registers them with the app manager.
 *
 * @return 0 on success, negative errno on failure.
 */
int zsw_llext_app_manager_init(void);

/**
 * @brief Create the app directory for an LLEXT app.
 *
 * Creates /lvgl_lfs/apps/<app_id>/ so that MCUmgr can subsequently
 * upload the .llext file into it. Silently ignores -EEXIST.
 *
 * @param app_id  Filesystem-safe app identifier (directory name).
 * @return 0 on success, negative errno on failure.
 */
int zsw_llext_app_manager_prepare_app_dir(const char *app_id);

/**
 * @brief Remove an installed LLEXT app from the filesystem.
 *
 * Unlinks /lvgl_lfs/apps/<app_id>/app.llext and then removes the
 * app directory. Both operations tolerate -ENOENT gracefully.
 *
 * @param app_id  Filesystem-safe app identifier (directory name).
 * @return 0 on success, negative errno on failure.
 */
int zsw_llext_app_manager_remove_app(const char *app_id);

/**
 * @brief Load an LLEXT app at runtime (hot-load).
 *
 * Loads and registers an app from /lvgl_lfs/apps/<app_id>/app.llext
 * without requiring a reboot. On success, shows a popup notification
 * indicating the app is installed and ready.
 *
 * @param app_id  Filesystem-safe app identifier (directory name).
 * @return 0 on success, negative errno on failure.
 */
int zsw_llext_app_manager_load_app(const char *app_id);
