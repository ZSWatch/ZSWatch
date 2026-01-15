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

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zsw_filesystem, LOG_LEVEL_INF);

#define PARTITION_NODE DT_NODELABEL(lvgl_lfs)
FS_FSTAB_DECLARE_ENTRY(PARTITION_NODE);

struct fs_mount_t *mountpoint = &FS_FSTAB_ENTRY(PARTITION_NODE);

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

int zsw_filesystem_ls(void)
{
    struct fs_statvfs sbuf;
    int rc;

    rc = fs_statvfs(mountpoint->mnt_point, &sbuf);
    if (rc < 0) {
        LOG_PRINTK("FAIL: statvfs: %d\n", rc);
        return -1;
    }

    LOG_WRN("%s: bsize = %lu ; frsize = %lu ;"
            " blocks = %lu ; bfree = %lu",
            mountpoint->mnt_point,
            sbuf.f_bsize, sbuf.f_frsize,
            sbuf.f_blocks, sbuf.f_bfree);

    rc = lsdir(mountpoint->mnt_point);
    if (rc < 0) {
        LOG_PRINTK("FAIL: lsdir %s: %d\n", mountpoint->mnt_point, rc);
        return -1;
    }

    return 0;
}

SYS_INIT(zsw_filesystem_ls, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
