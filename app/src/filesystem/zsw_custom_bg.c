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

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/sys/crc.h>
#include "filesystem/zsw_custom_bg.h"
#include "managers/zsw_xip_manager.h"

LOG_MODULE_REGISTER(zsw_custom_bg, LOG_LEVEL_INF);

#define BG_PARTITION custom_bg_partition

#if defined(CONFIG_ZSW_XIP) && FIXED_PARTITION_EXISTS(BG_PARTITION)

#define BG_PARTITION_ID     FIXED_PARTITION_ID(BG_PARTITION)
#define BG_XIP_BASE         (DT_REG_ADDR_BY_IDX(DT_NODELABEL(qspi), 1) + FIXED_PARTITION_OFFSET(BG_PARTITION))
#define BG_MAGIC            0x31475A42 // "BZG1"
// The image follows the header, at an offset that keeps the pixels word aligned.
#define BG_DATA_OFFSET      256
#define BG_ERASE_SIZE       4096

typedef struct {
    uint32_t magic;
    uint32_t len;
    uint32_t crc;
} bg_header_t;

static uint8_t __aligned(4) copy_buf[512];
// Length of the copy, 0 until zsw_custom_bg_store() has checked it.
static uint32_t stored_len;

static int file_crc(struct fs_file_t *file, uint32_t *len, uint32_t *crc)
{
    ssize_t n;

    *len = 0;
    *crc = 0;
    while ((n = fs_read(file, copy_buf, sizeof(copy_buf))) > 0) {
        *crc = crc32_ieee_update(*crc, copy_buf, n);
        *len += n;
    }

    return n < 0 ? (int)n : 0;
}

static int partition_crc(const struct flash_area *fa, uint32_t len, uint32_t *crc)
{
    *crc = 0;
    for (uint32_t off = 0; off < len; off += sizeof(copy_buf)) {
        size_t n = MIN(sizeof(copy_buf), len - off);
        int rc = flash_area_read(fa, BG_DATA_OFFSET + off, copy_buf, n);
        if (rc) {
            return rc;
        }
        *crc = crc32_ieee_update(*crc, copy_buf, n);
    }

    return 0;
}

static int copy_file(const struct flash_area *fa, struct fs_file_t *file, uint32_t len, uint32_t crc)
{
    bg_header_t header = { .magic = BG_MAGIC, .len = len, .crc = crc };
    uint32_t off = BG_DATA_OFFSET;
    uint32_t copy_crc;
    ssize_t n;
    int rc;

    // The header is written last, so an interrupted copy is never taken as valid.
    rc = flash_area_erase(fa, 0, ROUND_UP(BG_DATA_OFFSET + len, BG_ERASE_SIZE));
    if (rc) {
        return rc;
    }

    rc = fs_seek(file, 0, FS_SEEK_SET);
    if (rc) {
        return rc;
    }

    while ((n = fs_read(file, copy_buf, sizeof(copy_buf))) > 0) {
        rc = flash_area_write(fa, off, copy_buf, n);
        if (rc) {
            return rc;
        }
        off += n;
    }
    if (n < 0) {
        return (int)n;
    }

    rc = partition_crc(fa, len, &copy_crc);
    if (rc) {
        return rc;
    }
    if (off != BG_DATA_OFFSET + len || copy_crc != crc) {
        return -EIO;
    }

    return flash_area_write(fa, 0, &header, sizeof(header));
}

int zsw_custom_bg_store(const char *path)
{
    const struct flash_area *fa;
    struct fs_file_t file;
    bg_header_t header;
    uint32_t len;
    uint32_t crc;
    int rc;

    stored_len = 0;

    fs_file_t_init(&file);
    rc = fs_open(&file, path, FS_O_READ);
    if (rc) {
        return rc;
    }

    rc = flash_area_open(BG_PARTITION_ID, &fa);
    if (rc) {
        fs_close(&file);
        return rc;
    }

    rc = file_crc(&file, &len, &crc);
    if (rc == 0 && (len == 0 || len > fa->fa_size - BG_DATA_OFFSET)) {
        rc = -EFBIG;
    }
    if (rc == 0) {
        rc = flash_area_read(fa, 0, &header, sizeof(header));
    }
    if (rc == 0 && (header.magic != BG_MAGIC || header.len != len || header.crc != crc)) {
        uint32_t start = k_uptime_get_32();

        rc = copy_file(fa, &file, len, crc);
        LOG_INF("Copied %s (%u bytes) to flash in %u ms: %d", path, len, k_uptime_get_32() - start, rc);
    }
    if (rc == 0) {
        stored_len = len;
    } else {
        LOG_ERR("Custom background copy failed: %d", rc);
    }

    flash_area_close(fa);
    fs_close(&file);

    return rc;
}

int zsw_custom_bg_read(uint32_t offset, void *buf, uint32_t size, uint32_t *len)
{
    const struct flash_area *fa;
    uint32_t stored = stored_len;
    int rc;

    if (stored == 0) {
        return -ENOENT;
    }
    if (len) {
        *len = stored;
    }
    if (offset >= stored || size == 0) {
        return 0;
    }

    size = MIN(size, stored - offset);
    rc = flash_area_open(BG_PARTITION_ID, &fa);
    if (rc) {
        return rc;
    }
    rc = flash_area_read(fa, BG_DATA_OFFSET + offset, buf, size);
    flash_area_close(fa);

    return rc ? rc : (int)size;
}

const uint8_t *zsw_custom_bg_get(uint32_t *len)
{
    uint32_t stored = stored_len;

    if (stored == 0 || !zsw_xip_is_enabled()) {
        return NULL;
    }

    *len = stored;
    return (const uint8_t *)BG_XIP_BASE + BG_DATA_OFFSET;
}

#else

int zsw_custom_bg_store(const char *path)
{
    ARG_UNUSED(path);
    return -ENOTSUP;
}

int zsw_custom_bg_read(uint32_t offset, void *buf, uint32_t size, uint32_t *len)
{
    ARG_UNUSED(offset);
    ARG_UNUSED(buf);
    ARG_UNUSED(size);
    ARG_UNUSED(len);
    return -ENOTSUP;
}

const uint8_t *zsw_custom_bg_get(uint32_t *len)
{
    ARG_UNUSED(len);
    return NULL;
}

#endif
