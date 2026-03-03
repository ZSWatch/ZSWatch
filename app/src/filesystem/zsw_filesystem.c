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

#include <stdio.h>
#include <stdbool.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/logging/log.h>

#include <filesystem/zsw_filesystem.h>

LOG_MODULE_REGISTER(zsw_filesystem, LOG_LEVEL_DBG);

#define PARTITION_NODE DT_NODELABEL(lvgl_lfs)
FS_FSTAB_DECLARE_ENTRY(PARTITION_NODE);

struct fs_mount_t *mountpoint = &FS_FSTAB_ENTRY(PARTITION_NODE);

/* Second LittleFS partition for user data (settings, recordings, etc.)
 * Mounted manually to work around nRF Partition Manager limitation
 * which hardcodes all FSTAB entries to the same flash area.
 */
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(user_lfs_data);

static struct fs_mount_t user_mnt = {
    .type = FS_LITTLEFS,
    .fs_data = &user_lfs_data,
    .storage_dev = (void *)FIXED_PARTITION_ID(user_storage),
    .mnt_point = ZSW_USER_LFS_MOUNT_POINT,
};

static int lsdir(const char *path)
{
    int res;
    struct fs_dir_t dirp;
    static struct fs_dirent entry;

    fs_dir_t_init(&dirp);

    /* Verify fs_opendir() */
    res = fs_opendir(&dirp, path);
    if (res) {
        LOG_ERR("Error opening dir %s [%d]\n", path, res);
        return res;
    }

    LOG_WRN("Listing dir %s ...", path);
    for (;;) {
        /* Verify fs_readdir() */
        res = fs_readdir(&dirp, &entry);

        /* entry.name[0] == 0 means end-of-dir */
        if (res || entry.name[0] == 0) {
            if (res < 0) {
                LOG_ERR("Error reading dir [%d]\n", res);
            }
            break;
        }

        if (entry.type == FS_DIR_ENTRY_DIR) {
            LOG_PRINTK("[DIR ] %s\n", entry.name);
        } else {
            LOG_PRINTK("[FILE] %s (size = %zu)\n",
                       entry.name, entry.size);
        }
    }

    /* Verify fs_closedir() */
    fs_closedir(&dirp);

    return res;
}

static int log_fs_stats(const char *mount_point, bool full_scan)
{
    struct fs_statvfs sbuf;
    int rc;

    rc = fs_statvfs(mount_point, &sbuf);
    if (rc < 0) {
        LOG_ERR("FAIL: statvfs %s: %d", mount_point, rc);
        return rc;
    }

    LOG_WRN("%s: bsize = %lu ; frsize = %lu ;"
            " blocks = %lu ; bfree = %lu",
            mount_point,
            sbuf.f_bsize, sbuf.f_frsize,
            sbuf.f_blocks, sbuf.f_bfree);

    if (!full_scan) {
        return 0;
    }

    rc = lsdir(mount_point);
    if (rc < 0) {
        LOG_ERR("FAIL: lsdir %s: %d", mount_point, rc);
        return rc;
    }

    return 0;
}

int zsw_filesystem_ls(void)
{
    return log_fs_stats(mountpoint->mnt_point, false);
}

static int zsw_user_lfs_init(void)
{
    int rc;

    rc = fs_mount(&user_mnt);
    if (rc < 0) {
        LOG_ERR("Failed to mount %s: %d", ZSW_USER_LFS_MOUNT_POINT, rc);
        return rc;
    }

    LOG_INF("User LFS mounted at %s", ZSW_USER_LFS_MOUNT_POINT);
    return log_fs_stats(ZSW_USER_LFS_MOUNT_POINT, false);
}

SYS_INIT(zsw_filesystem_ls, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
SYS_INIT(zsw_user_lfs_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
