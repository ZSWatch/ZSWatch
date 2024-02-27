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

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <filesystem/zsw_filesystem.h>
#include <lvgl.h>
#include "lv_conf.h"
#include LV_MEM_CUSTOM_INCLUDE

#define TABLE_HEADER_MAGIC 0x0A0A0A0A

#define SPI_FLASH_SECTOR_SIZE        4096

#define FLASH_PARTITION_NAME            lvgl_raw_partition

#define FLASH_PARTITION_ID      FIXED_PARTITION_ID(FLASH_PARTITION_NAME)
#define FLASH_PARTITION_DEVICE  FIXED_PARTITION_DEVICE(FLASH_PARTITION_NAME)
#define FLASH_PARTITION_OFFSET  FIXED_PARTITION_OFFSET(FLASH_PARTITION_NAME)

#define FILE_TABLE_MAX_LEN  4096
#define MAX_FILE_NAME_LEN   32
#define MAX_OPENED_FILES    64

typedef struct file_header_t {
    uint8_t         filename[MAX_FILE_NAME_LEN];
    uint32_t        offset;
    uint32_t        len;
} file_header_t;

typedef struct file_table_t {
    uint32_t        magic;
    uint32_t        header_length; // Image offset counted from after this.
    uint32_t        total_length;
    uint32_t        num_files;
    file_header_t   file_headers[FILE_TABLE_MAX_LEN / sizeof(file_header_t)];
} file_table_t;

typedef struct opened_file_t {
    file_header_t  *header;
    uint32_t        index;
} opened_file_t;

static file_table_t file_table;
static opened_file_t opened_files[MAX_OPENED_FILES];

static const struct flash_area *flash_area;

static lv_fs_drv_t fs_drv;

static file_header_t *find_file(const char *name)
{
    for (int i = 0; i < file_table.num_files; i++) {
        if (strncmp(name, file_table.file_headers[i].filename, MAX_FILE_NAME_LEN) == 0) {
            return &file_table.file_headers[i];
        }
    }
    return NULL;
}

static opened_file_t *find_free_opened_file(void)
{
    for (int i = 0; i < MAX_OPENED_FILES; i++) {
        if (opened_files[i].header == NULL) {
            return &opened_files[i];
        }
    }
    return NULL;
}

static bool lvgl_fs_ready(struct _lv_fs_drv_t *drv)
{
    return true;
}

static lv_fs_res_t errno_to_lv_fs_res(int err)
{
    switch (err) {
        case 0:
            return LV_FS_RES_OK;
        case -EIO:
            /*Low level hardware error*/
            return LV_FS_RES_HW_ERR;
        case -EBADF:
            /*Error in the file system structure */
            return LV_FS_RES_FS_ERR;
        case -ENOENT:
            /*Driver, file or directory is not exists*/
            return LV_FS_RES_NOT_EX;
        case -EFBIG:
            /*Disk full*/
            return LV_FS_RES_FULL;
        case -EACCES:
            /*Access denied. Check 'fs_open' modes and write protect*/
            return LV_FS_RES_DENIED;
        case -EBUSY:
            /*The file system now can't handle it, try later*/
            return LV_FS_RES_BUSY;
        case -ENOMEM:
            /*Not enough memory for an internal operation*/
            return LV_FS_RES_OUT_OF_MEM;
        case -EINVAL:
            /*Invalid parameter among arguments*/
            return LV_FS_RES_INV_PARAM;
        case -ENOTSUP:
            /*Not supported by the filesystem*/
            return LV_FS_RES_NOT_IMP;
        default:
            return LV_FS_RES_UNKNOWN;
    }
}

static void *lvgl_fs_open(struct _lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    file_header_t *file;

    if (file_table.magic != TABLE_HEADER_MAGIC) {
        return NULL;
    }

    file = find_file(path);
    opened_file_t *open_file;

    if (!file) {
        return NULL;
    }

    open_file = find_free_opened_file();
    if (!file) {
        return NULL;
    }

    open_file->header = file;
    open_file->index = 0;

    return open_file;
}

static lv_fs_res_t lvgl_fs_close(struct _lv_fs_drv_t *drv, void *file)
{
    opened_file_t *open_file = (opened_file_t *)file;
    open_file->header = NULL;
    open_file->index = 0;
    return errno_to_lv_fs_res(0);
}

static lv_fs_res_t lvgl_fs_read(struct _lv_fs_drv_t *drv, void *file, void *buf, uint32_t btr,
                                uint32_t *br)
{
    int rc;
    opened_file_t *open_file = (opened_file_t *)file;

    rc = flash_area_read(flash_area, open_file->header->offset + open_file->index + file_table.header_length, buf, btr);
    if (rc != 0) {
        printk("Flash read failed! %d\n", rc);
        *br = 0;
        return errno_to_lv_fs_res(rc);
    }
    *br = btr;
    open_file->index += btr;
    return errno_to_lv_fs_res(0);
}

static lv_fs_res_t lvgl_fs_write(struct _lv_fs_drv_t *drv, void *file, const void *buf,
                                 uint32_t btw, uint32_t *bw)
{
    return LV_FS_RES_NOT_IMP;
}

static lv_fs_res_t lvgl_fs_seek(struct _lv_fs_drv_t *drv, void *file, uint32_t pos,
                                lv_fs_whence_t whence)
{
    opened_file_t *open_file = (opened_file_t *)file;

    switch (whence) {
        case LV_FS_SEEK_END:
            open_file->index = open_file->header->len;
            break;
        case LV_FS_SEEK_CUR:
            open_file->index += pos;
            break;
        case LV_FS_SEEK_SET:
        default:
            open_file->index = pos;
            break;
    }

    return errno_to_lv_fs_res(0);
}

static lv_fs_res_t lvgl_fs_tell(struct _lv_fs_drv_t *drv, void *file, uint32_t *pos_p)
{
    opened_file_t *open_file = (opened_file_t *)file;
    *pos_p = open_file->index;
    return LV_FS_RES_OK;
}

static void *lvgl_fs_dir_open(struct _lv_fs_drv_t *drv, const char *path)
{
    return NULL;
}

static lv_fs_res_t lvgl_fs_dir_read(struct _lv_fs_drv_t *drv, void *dir, char *fn)
{
    return LV_FS_RES_NOT_IMP;
}

static lv_fs_res_t lvgl_fs_dir_close(struct _lv_fs_drv_t *drv, void *dir)
{
    int err;
    err = 0;
    return errno_to_lv_fs_res(err);
}

int zsw_filesytem_get_num_rawfs_files(void)
{
    return file_table.num_files;
}

static int zsw_decoder_init(void)
{
    int rc;
    lv_fs_drv_init(&fs_drv);

    /* LVGL uses letter based mount points, just pass the root slash as a
     * letter. Note that LVGL will remove the drive letter, or in this case
     * the root slash, from the path passed via the FS callbacks.
     * Zephyr FS API assumes this slash is present so we will need to add
     * it back.
     */
    fs_drv.letter = 'S';
    fs_drv.ready_cb = lvgl_fs_ready;

    fs_drv.open_cb = lvgl_fs_open;
    fs_drv.close_cb = lvgl_fs_close;
    fs_drv.read_cb = lvgl_fs_read;
    fs_drv.write_cb = lvgl_fs_write;
    fs_drv.seek_cb = lvgl_fs_seek;
    fs_drv.tell_cb = lvgl_fs_tell;

    fs_drv.dir_open_cb = lvgl_fs_dir_open;
    fs_drv.dir_read_cb = lvgl_fs_dir_read;
    fs_drv.dir_close_cb = lvgl_fs_dir_close;

    lv_fs_drv_register(&fs_drv);

    memset(opened_files, 0, sizeof(opened_files));

    rc = flash_area_open(FLASH_PARTITION_ID, &flash_area);

    if (rc != 0) {
        printk("FAIL: unable to find flash area %u: %d\n", FLASH_PARTITION_ID, rc);
        return 0;
    }

    rc = flash_area_read(flash_area, 0, &file_table, FILE_TABLE_MAX_LEN);
    if (rc != 0) {
        printk("Flash read failed! %d\n", rc);
        return rc;
    }
    return 0;
}

SYS_INIT(zsw_decoder_init, APPLICATION, 99);
