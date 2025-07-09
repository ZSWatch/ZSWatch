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
#include <zephyr/sys/util.h>
#include <filesystem/zsw_filesystem.h>
#include <lvgl.h>
#include "lv_conf.h"

#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>
#include <errno.h>
#include <zephyr/fs/fs.h>
#include <zephyr/fs/fs_sys.h>
#include <zephyr/sys/__assert.h>

LOG_MODULE_REGISTER(zsw_fs, LOG_LEVEL_INF);

#define ZSW_FS_MOUNT_POINT "/S"

//#include LV_MEM_CUSTOM_INCLUDE

#define TABLE_HEADER_MAGIC 0x0A0A0A0A

#define SPI_FLASH_SECTOR_SIZE        4096

#define FLASH_PARTITION_NAME            lvgl_raw_partition

#define FLASH_PARTITION_ID      FIXED_PARTITION_ID(FLASH_PARTITION_NAME)
#define FLASH_PARTITION_DEVICE  FIXED_PARTITION_DEVICE(FLASH_PARTITION_NAME)
#define FLASH_PARTITION_OFFSET  FIXED_PARTITION_OFFSET(FLASH_PARTITION_NAME)

#define FILE_TABLE_MAX_LEN  32000
#define MAX_FILE_NAME_LEN   32
#define MAX_OPENED_FILES    64

#define IS_SPECIAL_FULL_FS_FILE_PATH(name) \
    (strncmp(name, FULL_FS_SPECIAL_FILE_NAME, sizeof(FULL_FS_SPECIAL_FILE_NAME) - 1) == 0)
#define IS_SPECIAL_FULL_FS_FILE(ptr) \
    (ptr == &full_fs_file)

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
    bool            is_cached;
    uint32_t        cache_start;
    uint32_t        cache_end;
} opened_file_t;

#define FULL_FS_SPECIAL_FILE_NAME "full_fs"

typedef struct fullFsFile_t {
    uint32_t        index;
    uint32_t        len;
    bool            opened;
} fullFsFile_t;

static file_table_t file_table;
static opened_file_t opened_files[MAX_OPENED_FILES];

static uint8_t file_cache_buffer[SPI_FLASH_SECTOR_SIZE];
static opened_file_t *current_cached_file;

static const struct flash_area *flash_area;

static lv_fs_drv_t fs_drv;

static fullFsFile_t full_fs_file;

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

static int zsw_fs_open(struct fs_file_t *zfp, const char *file_name, fs_mode_t mode)
{
    // TODO: Set max opened files to 1 to avoid issues when the full fs is open.
    // OR handle it another way, like error if opening full fs if other file is open.

    // Zephyr FS always passes the full path, so we need to strip the mount point.
    char *file_name_ptr = (char *)file_name + strlen(ZSW_FS_MOUNT_POINT) + 1;

    LOG_INF("Opening fs file %s, mode: %d", file_name_ptr, (int)mode);

    if (mode & ~(FS_O_RDWR | FS_O_CREATE)) {
        LOG_ERR("Unsupported mode flags set: %d", mode);
        return -EINVAL;
    }

    if (IS_SPECIAL_FULL_FS_FILE_PATH(file_name_ptr)) {
        if (full_fs_file.opened) {
            return -EALREADY;
        }
        if (mode & FS_O_WRITE) {
            // TODO: Erase whole image?
            /*
            LOG_INF("Erasing full fs image");
            int rc = flash_area_erase(flash_area, 0, flash_area->fa_size);
            LOG_INF("Flash area erased with rc: %d", rc);
            if (rc != 0) {
                LOG_ERR("Failed to erase flash area: %d", rc);
                return -EIO;
            }*/
        }
        full_fs_file.opened = true;
        full_fs_file.index = 0;
        zfp->filep = &full_fs_file;
    } else {
        if (mode & FS_O_WRITE) {
            LOG_ERR("Write mode not supported for this file");
            return -EACCES;
        }
        zfp->filep = lvgl_fs_open(NULL, file_name_ptr, 0);

        if (zfp->filep == NULL) {
            return -EEXIST;
        }
    }

    return 0;
}

static lv_fs_res_t lvgl_fs_close(struct _lv_fs_drv_t *drv, void *file)
{
    opened_file_t *open_file = (opened_file_t *)file;
    open_file->header = NULL;
    open_file->index = 0;
    open_file->is_cached = false;
    return errno_to_lv_fs_res(0);
}

static int zsw_fs_close(struct fs_file_t *zfp)
{
    if (IS_SPECIAL_FULL_FS_FILE(zfp->filep)) {
        full_fs_file.opened = false;
        full_fs_file.index = 0;
        return 0;
    } else {
        lvgl_fs_close(NULL, zfp->filep);
    }
    return 0;
}

static lv_fs_res_t lvgl_fs_read(struct _lv_fs_drv_t *drv, void *file, void *buf, uint32_t btr,
                                uint32_t *br)
{
    int rc;
    uint32_t extra_bytes_start;
    uint32_t extra_bytes_end;
    uint32_t extra_cache_bytes;
    uint32_t orig_read_address, read_address;
    opened_file_t *open_file = (opened_file_t *)file;

    btr = MIN(btr, open_file->header->len - open_file->index);
    if (btr == 0) {
        *br = 0;
        return LV_FS_RES_OK;
    }

    orig_read_address = open_file->header->offset + open_file->index + file_table.header_length;

    if (open_file->is_cached && (orig_read_address >= current_cached_file->cache_start) &&
        ((orig_read_address + btr) < current_cached_file->cache_end)) {
        __ASSERT(current_cached_file == open_file, "Cached file mismatch");
        memcpy(buf, file_cache_buffer + (orig_read_address - current_cached_file->cache_start), btr);
        *br = btr;
        open_file->index += btr;
        return errno_to_lv_fs_res(0);
    } else if (current_cached_file) {
        // Cache miss
        current_cached_file->is_cached = false;
        current_cached_file = NULL;
    }

    read_address = ROUND_DOWN(orig_read_address, 4);

    if ((read_address != orig_read_address) && btr < sizeof(file_cache_buffer)) {
        extra_bytes_start = orig_read_address - read_address;
    } else {
        extra_bytes_start = 0;
        read_address = orig_read_address;
    }

    if (!IS_ALIGNED(btr + extra_bytes_start, 4) && (ROUND_UP(btr + extra_bytes_start, 4) < sizeof(file_cache_buffer))) {
        extra_bytes_end = ROUND_UP(btr + extra_bytes_start, 4) - (btr + extra_bytes_start);
    } else {
        extra_bytes_end = 0;
    }

    uint32_t cache_max_len = sizeof(file_cache_buffer) - extra_bytes_start - btr - extra_bytes_end;
    cache_max_len = MIN(cache_max_len, open_file->header->len - open_file->index);

    if (!current_cached_file && btr != 4) {
        current_cached_file = open_file;
        current_cached_file->cache_start = read_address;
        current_cached_file->cache_end = read_address + cache_max_len + btr + extra_bytes_start + extra_bytes_end;
        current_cached_file->is_cached = true;
        extra_cache_bytes = cache_max_len;
    } else {
        extra_cache_bytes = 0;
    }

    rc = flash_area_read(flash_area, read_address, (extra_bytes_start || extra_bytes_end ||
                                                    current_cached_file) ? file_cache_buffer : buf,
                         btr + extra_bytes_start + extra_bytes_end + extra_cache_bytes);
    if (rc != 0) {
        printk("Flash read failed! %d\n", rc);
        *br = 0;
        return errno_to_lv_fs_res(rc);
    }

    if (extra_bytes_start || current_cached_file) {
        memcpy(buf, file_cache_buffer + extra_bytes_start, btr);
    }

    *br = btr;
    open_file->index += btr;
    return errno_to_lv_fs_res(0);
}

static ssize_t zsw_fs_read(struct fs_file_t *zfp, void *ptr, size_t size)
{
    LOG_DBG("Reading %d bytes from file", size);
    if (IS_SPECIAL_FULL_FS_FILE(zfp->filep)) {
        int rc;
        size = MIN(size, full_fs_file.len - full_fs_file.index);
        rc = flash_area_read(flash_area, full_fs_file.index, ptr, size);
        if (rc != 0) {
            return -EIO;
        } else {
            full_fs_file.index += size;
            return size;
        }
    } else {
        uint32_t bytes_read;

        lv_fs_res_t res = lvgl_fs_read(NULL, zfp->filep, ptr, size, &bytes_read);
        LOG_DBG("Res: %d, bytes read: %d", res, bytes_read);
        if (res != LV_FS_RES_OK) {
            return -EIO;
        }

        return bytes_read;
    }
}

static lv_fs_res_t lvgl_fs_write(struct _lv_fs_drv_t *drv, void *file, const void *buf,
                                 uint32_t btw, uint32_t *bw)
{
    return LV_FS_RES_NOT_IMP;
}

static ssize_t zsw_fs_write(struct fs_file_t *zfp, const void *ptr, size_t size)
{
    if (IS_SPECIAL_FULL_FS_FILE(zfp->filep)) {
        int rc;
        rc = flash_area_write(flash_area, full_fs_file.index, ptr, size);
        LOG_DBG("W: %d => %d (%d)", full_fs_file.index, size, rc);
        if (rc != 0) {
            int new_rc = flash_area_write(flash_area, full_fs_file.index, ptr, size);
            LOG_ERR("Flash write failed! %d\n", new_rc);
            if (new_rc != 0) {
                return -EIO;
            }
            return -EIO;
        } else {
            full_fs_file.index += size;
            return size;
        }
    } else {
        return -ENOTSUP;
    }
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

static int zsw_fs_seek(struct fs_file_t *zfp, off_t offset, int whence)
{
    int rc;
    LOG_DBG("Seeking %d bytes in file, whence: %d", (int)offset, whence);

    if (IS_SPECIAL_FULL_FS_FILE(zfp->filep)) {
        switch (whence) {
            case FS_SEEK_SET:
                full_fs_file.index = offset;
                break;
            case FS_SEEK_CUR:
                full_fs_file.index += offset;
                break;
            case FS_SEEK_END:
                full_fs_file.index = full_fs_file.len + offset;
                break;
            default:
                return -EINVAL;
        }
    } else {
        switch (whence) {
            case FS_SEEK_SET:
                rc = lvgl_fs_seek(NULL, zfp->filep, offset, LV_FS_SEEK_SET);
                break;
            case FS_SEEK_CUR:
                rc = lvgl_fs_seek(NULL, zfp->filep, offset, LV_FS_SEEK_CUR);
                break;
            case FS_SEEK_END:
                rc = lvgl_fs_seek(NULL, zfp->filep, offset, LV_FS_SEEK_END);
                break;
            default:
                return -EINVAL;
        }

        if (rc != LV_FS_RES_OK) {
            return -EIO;
        }
    }

    return 0;
}

static lv_fs_res_t lvgl_fs_tell(struct _lv_fs_drv_t *drv, void *file, uint32_t *pos_p)
{
    opened_file_t *open_file = (opened_file_t *)file;
    *pos_p = open_file->index;
    LOG_DBG("Telling file, pos: %d", *pos_p);
    return LV_FS_RES_OK;
}

static off_t zsw_fs_tell(struct fs_file_t *zfp)
{
    if (IS_SPECIAL_FULL_FS_FILE(zfp->filep)) {
        return full_fs_file.index;
    } else {
        uint32_t pos;
        lvgl_fs_tell(NULL, zfp->filep, &pos);
        return pos;
    }
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
    return LV_FS_RES_NOT_IMP;
}

static int zsw_fs_mount(struct fs_mount_t *mountp)
{
    mountp->mnt_point = ZSW_FS_MOUNT_POINT;
    return 0;
}

static int zsw_fs_unmount(struct fs_mount_t *mountp)
{
    return 0;
}

static int zsw_fs_stat(struct fs_mount_t *mountp,
                       const char *path, struct fs_dirent *entry)
{
    char *file_name_ptr = (char *)path + strlen(ZSW_FS_MOUNT_POINT) + 1;

    LOG_DBG("stat file %s", file_name_ptr);

    if (IS_SPECIAL_FULL_FS_FILE_PATH(file_name_ptr)) {
        entry->type = FS_DIR_ENTRY_FILE;
        entry->size = full_fs_file.len;
        strncpy(entry->name, FULL_FS_SPECIAL_FILE_NAME, MAX_FILE_NAME_LEN);
        return 0;
    } else {
        file_header_t *file = find_file(file_name_ptr);
        if (file) {
            entry->type = FS_DIR_ENTRY_FILE;
            entry->size = file->len;
            strncpy(entry->name, file->filename, MAX_FILE_NAME_LEN);
            return 0;
        } else {
            return -ENOENT;
        }
    }
}

static int zsw_fs_unlink(struct fs_mount_t *mountp, const char *path)
{
    char *file_name_ptr = (char *)path + strlen(ZSW_FS_MOUNT_POINT) + 1;

    LOG_DBG("unlink file %s", file_name_ptr);

    if (IS_SPECIAL_FULL_FS_FILE_PATH(file_name_ptr)) {
        return 0;
    } else {
        return -ENOTSUP;
    }
}

int zsw_filesytem_get_num_rawfs_files(void)
{
    return file_table.num_files;
}

int zsw_filesytem_get_total_size(void)
{
    return file_table.total_length;
}

int zsw_filesytem_erase(void)
{
    memset(opened_files, 0, sizeof(opened_files));
    memset(&file_table, 0, sizeof(file_table));
    current_cached_file = NULL;
    full_fs_file.len = 0;
    full_fs_file.index = 0;

    int num_opened_files = 0;
    for (int i = 0; i < MAX_OPENED_FILES; i++) {
        if (opened_files[i].header != NULL) {
            num_opened_files++;
        }
    }
    LOG_WRN("Number of opened files: %d", num_opened_files);

    LOG_INF("Erasing full fs image");
    int rc = flash_area_erase(flash_area, 0, flash_area->fa_size);
    LOG_INF("Flash area erased with rc: %d", rc);
    if (rc != 0) {
        LOG_ERR("Failed to erase flash area: %d", rc);
        return -EIO;
    }

    return 0;
}

/* Zephyr File system interface */
static const struct fs_file_system_t zsw_fs = {
    .open = zsw_fs_open,
    .close = zsw_fs_close,
    .read = zsw_fs_read,
    .write = zsw_fs_write,
    .lseek = zsw_fs_seek,
    .tell = zsw_fs_tell,
    .mount = zsw_fs_mount,
    .unmount = zsw_fs_unmount,
    .unlink = zsw_fs_unlink,
    .stat = zsw_fs_stat,
};

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

    // Fill in the special file that corresponds to the full image.
    full_fs_file.opened = false;
    full_fs_file.index = 0;
    full_fs_file.len = file_table.total_length;
    if (file_table.magic != TABLE_HEADER_MAGIC) {
        LOG_ERR("Invalid file table magic: %x", file_table.magic);
        full_fs_file.len = 0;
    }

    rc = fs_register(FS_TYPE_EXTERNAL_BASE, &zsw_fs);

    if (rc == 0) {
        static struct fs_mount_t zsw_fs_mount = {
            .mnt_point = ZSW_FS_MOUNT_POINT,
            .type = FS_TYPE_EXTERNAL_BASE,

        };
        rc = fs_mount(&zsw_fs_mount);
    }

    return rc;
}

SYS_INIT(zsw_decoder_init, APPLICATION, 99);
