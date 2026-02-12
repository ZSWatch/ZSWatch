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

#include <errno.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/reboot.h>

#if defined(CONFIG_SETTINGS_NVS)
#include <zephyr/fs/nvs.h>
#elif defined(CONFIG_SETTINGS_FCB)
#include <zephyr/storage/fcb.h>
#elif defined(CONFIG_SETTINGS_ZMS) || defined(CONFIG_SETTINGS_ZMS_LEGACY)
#include <zephyr/fs/zms.h>
#elif defined(CONFIG_SETTINGS_FILE)
#include <zephyr/fs/fs.h>
#endif

LOG_MODULE_REGISTER(zsw_settings, LOG_LEVEL_INF);

int zsw_settings_erase_all(void)
{
    int rc = 0;
    void *storage = NULL;

    rc = settings_storage_get(&storage);
    if (rc != 0) {
        LOG_ERR("settings_storage_get failed: %d", rc);
        return rc;
    }

#if defined(CONFIG_SETTINGS_NVS)
    rc = nvs_clear((struct nvs_fs *)storage);
#elif defined(CONFIG_SETTINGS_FCB)
    rc = fcb_clear((struct fcb *)storage);
#elif defined(CONFIG_SETTINGS_ZMS) || defined(CONFIG_SETTINGS_ZMS_LEGACY)
    rc = zms_clear((struct zms_fs *)storage);
#elif defined(CONFIG_SETTINGS_FILE)
    rc = fs_unlink((const char *)storage);
#else
    rc = -ENOTSUP;
#endif

    if (rc != 0) {
        LOG_ERR("settings erase failed: %d", rc);
    }

    return rc;
}
