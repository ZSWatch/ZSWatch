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

#include <zephyr/kernel.h>
#include <zephyr/fs/fs.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "voice_memo_store.h"
#include "zsw_clock.h"

LOG_MODULE_REGISTER(voice_memo_store, CONFIG_ZSW_VOICE_MEMO_LOG_LEVEL);

#define FLASH_WRITE_BUF_SIZE   512
#define MAX_PATH_LEN           64
#define COUNTER_FILE_PATH      VOICE_MEMO_DIR "/.counter"

static struct fs_file_t current_file;
static bool file_open;
static bool recording_active;
static uint32_t frame_count;
static char current_filepath[MAX_PATH_LEN];
static char current_filename[VOICE_MEMO_MAX_FILENAME];

/* Batched flash write buffer */
static uint8_t write_buf[FLASH_WRITE_BUF_SIZE];
static size_t write_buf_pos;

static int flush_write_buf(void)
{
    if (write_buf_pos == 0 || !file_open) {
        return 0;
    }

    ssize_t written = fs_write(&current_file, write_buf, write_buf_pos);
    if (written < 0) {
        LOG_ERR("Flash write failed: %d", (int)written);
        return (int)written;
    }
    if ((size_t)written != write_buf_pos) {
        LOG_ERR("Short write: %d/%u", (int)written, (unsigned int)write_buf_pos);
        return -EIO;
    }

    write_buf_pos = 0;
    return 0;
}

static int buffered_write(const void *data, size_t len)
{
    const uint8_t *src = (const uint8_t *)data;
    size_t remaining = len;

    while (remaining > 0) {
        size_t space = FLASH_WRITE_BUF_SIZE - write_buf_pos;
        size_t chunk = remaining < space ? remaining : space;

        memcpy(&write_buf[write_buf_pos], src, chunk);
        write_buf_pos += chunk;
        src += chunk;
        remaining -= chunk;

        if (write_buf_pos >= FLASH_WRITE_BUF_SIZE) {
            int ret = flush_write_buf();
            if (ret < 0) {
                return ret;
            }
        }
    }
    return 0;
}

static bool is_time_valid(void)
{
    zsw_timeval_t ztm;
    struct tm tm;

    zsw_clock_get_time(&ztm);
    zsw_timeval_to_tm(&ztm, &tm);

    return (tm.tm_year + 1900) >= 2025;
}

static uint32_t get_unix_timestamp(void)
{
    zsw_timeval_t ztm;
    struct tm tm;

    zsw_clock_get_time(&ztm);
    zsw_timeval_to_tm(&ztm, &tm);

    return (uint32_t)mktime(&tm);
}

uint32_t voice_memo_store_get_unix_timestamp(void)
{
    return get_unix_timestamp();
}

static int read_persistent_counter(void)
{
    struct fs_file_t fp;
    uint32_t counter = 0;

    fs_file_t_init(&fp);
    if (fs_open(&fp, COUNTER_FILE_PATH, FS_O_READ) == 0) {
        fs_read(&fp, &counter, sizeof(counter));
        fs_close(&fp);
    }
    return (int)counter;
}

static void write_persistent_counter(int value)
{
    struct fs_file_t fp;
    uint32_t counter = (uint32_t)value;

    fs_file_t_init(&fp);
    if (fs_open(&fp, COUNTER_FILE_PATH, FS_O_CREATE | FS_O_WRITE) == 0) {
        fs_write(&fp, &counter, sizeof(counter));
        fs_close(&fp);
    }
}

static void generate_filename(char *buf, size_t buflen)
{
    if (is_time_valid()) {
        zsw_timeval_t ztm;
        struct tm tm;

        zsw_clock_get_time(&ztm);
        zsw_timeval_to_tm(&ztm, &tm);

        snprintf(buf, buflen, "%04d%02d%02d_%02d%02d%02d",
                 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                 tm.tm_hour, tm.tm_min, tm.tm_sec);
    } else {
        /* RTC fallback: use persistent monotonic counter that survives file deletions */
        int next_num = read_persistent_counter() + 1;
        write_persistent_counter(next_num);
        snprintf(buf, buflen, "REC_%03d", next_num);
    }
}

static int repair_dirty_file(const char *filepath)
{
    struct fs_file_t fp;
    voice_memo_header_t hdr;
    int ret;

    fs_file_t_init(&fp);
    ret = fs_open(&fp, filepath, FS_O_RDWR);
    if (ret < 0) {
        return ret;
    }

    ssize_t hdr_read = fs_read(&fp, &hdr, sizeof(hdr));
    if (hdr_read < (ssize_t)sizeof(hdr)) {
        LOG_WRN("Dirty file too small, deleting: %s", filepath);
        fs_close(&fp);
        fs_unlink(filepath);
        return 0;
    }

    if (memcmp(hdr.magic, VOICE_MEMO_MAGIC, 4) != 0) {
        LOG_WRN("Bad magic in dirty file, deleting: %s", filepath);
        fs_close(&fp);
        fs_unlink(filepath);
        return 0;
    }

    if (hdr.total_frames != 0xFFFFFFFF) {
        /* Already clean */
        fs_close(&fp);
        return 0;
    }

    /* Walk frames to count and compute duration */
    uint32_t counted_frames = 0;
    while (true) {
        uint16_t frame_len;
        ssize_t n = fs_read(&fp, &frame_len, sizeof(frame_len));
        if (n < (ssize_t)sizeof(frame_len)) {
            break;
        }
        if (frame_len == 0 || frame_len > 500) {
            /* Invalid frame length */
            break;
        }
        off_t pos = fs_tell(&fp);
        if (fs_seek(&fp, frame_len, FS_SEEK_CUR) < 0) {
            break;
        }
        /* Verify we actually have that many bytes */
        if (fs_tell(&fp) != pos + frame_len) {
            break;
        }
        counted_frames++;
    }

    if (counted_frames == 0) {
        LOG_WRN("No valid frames in dirty file, deleting: %s", filepath);
        fs_close(&fp);
        fs_unlink(filepath);
        return 0;
    }

    /* Repair header */
    hdr.total_frames = counted_frames;
    hdr.duration_ms = (uint32_t)((uint64_t)counted_frames * hdr.frame_size * 1000 / hdr.sample_rate);

    fs_seek(&fp, 0, FS_SEEK_SET);
    fs_write(&fp, &hdr, sizeof(hdr));
    fs_close(&fp);

    LOG_INF("Repaired dirty recording: %s, frames=%u, duration_ms=%u",
            filepath, counted_frames, hdr.duration_ms);
    return 0;
}

int voice_memo_store_init(void)
{
    int ret;
    struct fs_dir_t dirp;
    struct fs_dirent entry;

    fs_dir_t_init(&dirp);

    /* Create directory if needed */
    ret = fs_mkdir(VOICE_MEMO_DIR);
    if (ret < 0 && ret != -EEXIST) {
        LOG_ERR("Failed to create recordings dir: %d", ret);
        return ret;
    }

    /* Scan for dirty files */
    ret = fs_opendir(&dirp, VOICE_MEMO_DIR);
    if (ret < 0) {
        LOG_ERR("Failed to open recordings dir: %d", ret);
        return ret;
    }

    while (fs_readdir(&dirp, &entry) == 0 && entry.name[0] != '\0') {
        if (entry.type != FS_DIR_ENTRY_FILE) {
            continue;
        }
        char path[MAX_PATH_LEN];
        snprintf(path, sizeof(path), "%s/%s", VOICE_MEMO_DIR, entry.name);
        repair_dirty_file(path);
    }

    fs_closedir(&dirp);
    LOG_INF("[TEST] voice_memo_store: initialized");
    return 0;
}

int voice_memo_store_start_recording(void)
{
    int ret;
    voice_memo_header_t hdr;

    if (recording_active) {
        LOG_WRN("Already recording");
        return -EALREADY;
    }

    /* Check free space */
    uint32_t free_bytes;
    ret = voice_memo_store_get_free_space(&free_bytes);
    if (ret < 0) {
        return ret;
    }
    if (free_bytes < (uint32_t)CONFIG_APPLICATIONS_CONFIGURATION_VOICE_MEMO_MIN_FREE_SPACE_KB * 1024) {
        LOG_ERR("Not enough free space: %u KB", free_bytes / 1024);
        return -ENOSPC;
    }

    generate_filename(current_filename, sizeof(current_filename));
    snprintf(current_filepath, sizeof(current_filepath),
             "%s/%s.zsw_opus", VOICE_MEMO_DIR, current_filename);

    fs_file_t_init(&current_file);
    ret = fs_open(&current_file, current_filepath, FS_O_CREATE | FS_O_RDWR);
    if (ret < 0) {
        LOG_ERR("Failed to create recording file: %d", ret);
        return ret;
    }
    file_open = true;

    /* Write header with sentinel values */
    memset(&hdr, 0, sizeof(hdr));
    memcpy(hdr.magic, VOICE_MEMO_MAGIC, 4);
    hdr.version = VOICE_MEMO_HEADER_VERSION;
    hdr.sample_rate = 16000;
    hdr.frame_size = CONFIG_ZSW_OPUS_FRAME_SIZE_SAMPLES;
    hdr.bitrate = CONFIG_ZSW_OPUS_BITRATE;
    hdr.timestamp = get_unix_timestamp();
    hdr.total_frames = 0xFFFFFFFF;  /* Sentinel: dirty until clean stop */
    hdr.duration_ms = 0xFFFFFFFF;

    ssize_t written = fs_write(&current_file, &hdr, sizeof(hdr));
    if (written != sizeof(hdr)) {
        LOG_ERR("Failed to write header");
        fs_close(&current_file);
        file_open = false;
        fs_unlink(current_filepath);
        return -EIO;
    }

    frame_count = 0;
    write_buf_pos = 0;
    recording_active = true;

    LOG_INF("[TEST] voice_memo: recording_started file=%s", current_filename);
    return 0;
}

int voice_memo_store_write_frame(const uint8_t *opus_data, size_t len)
{
    if (!recording_active || !file_open) {
        return -EINVAL;
    }

    /* Write frame: [2B length LE][N bytes data] */
    uint16_t frame_len = (uint16_t)len;
    int ret;

    ret = buffered_write(&frame_len, sizeof(frame_len));
    if (ret < 0) {
        return ret;
    }

    ret = buffered_write(opus_data, len);
    if (ret < 0) {
        return ret;
    }

    frame_count++;
    return 0;
}

int voice_memo_store_flush(void)
{
    return flush_write_buf();
}

int voice_memo_store_stop_recording(uint32_t *out_duration_ms, uint32_t *out_size_bytes)
{
    if (!recording_active) {
        LOG_WRN("stop_recording: not active");
        return -EINVAL;
    }

    /* Flush remaining data */
    int ret = flush_write_buf();
    if (ret < 0) {
        LOG_ERR("Flush on stop failed: %d", ret);
    }

    /* Compute duration */
    uint32_t duration_ms = (uint32_t)((uint64_t)frame_count *
                                       CONFIG_ZSW_OPUS_FRAME_SIZE_SAMPLES * 1000 / 16000);

    /* Update header with final values */
    voice_memo_header_t hdr;
    fs_seek(&current_file, 0, FS_SEEK_SET);
    fs_read(&current_file, &hdr, sizeof(hdr));

    hdr.total_frames = frame_count;
    hdr.duration_ms = duration_ms;

    fs_seek(&current_file, 0, FS_SEEK_SET);
    fs_write(&current_file, &hdr, sizeof(hdr));

    /* Sync and close */
    fs_sync(&current_file);
    fs_close(&current_file);

    struct fs_dirent stat_entry;
    uint32_t file_size = 0;
    if (fs_stat(current_filepath, &stat_entry) == 0) {
        file_size = stat_entry.size;
    }

    file_open = false;
    recording_active = false;

    if (out_duration_ms) {
        *out_duration_ms = duration_ms;
    }
    if (out_size_bytes) {
        *out_size_bytes = file_size;
    }

    LOG_INF("[TEST] voice_memo: recording_stopped file=%s duration_ms=%u size=%u",
            current_filename, duration_ms, file_size);
    return 0;
}

int voice_memo_store_abort_recording(void)
{
    if (!recording_active) {
        LOG_WRN("abort_recording: not active");
        return -EINVAL;
    }

    /* Discard write buffer */
    write_buf_pos = 0;

    /* Close and delete the partial file */
    if (file_open) {
        fs_close(&current_file);
        file_open = false;
    }

    int ret = fs_unlink(current_filepath);
    if (ret < 0) {
        LOG_ERR("abort_recording: failed to delete %s: %d", current_filepath, ret);
    }

    recording_active = false;

    LOG_INF("[TEST] voice_memo: recording_aborted file=%s", current_filename);
    return 0;
}

int voice_memo_store_list(voice_memo_entry_t *entries, size_t max_entries)
{
    struct fs_dir_t dirp;
    struct fs_dirent entry;
    int count = 0;

    fs_dir_t_init(&dirp);
    int ret = fs_opendir(&dirp, VOICE_MEMO_DIR);
    if (ret < 0) {
        return ret;
    }

    while (fs_readdir(&dirp, &entry) == 0 && entry.name[0] != '\0') {
        if (entry.type != FS_DIR_ENTRY_FILE) {
            continue;
        }
        /* Only list .zsw_opus files */
        const char *ext = strstr(entry.name, ".zsw_opus");
        if (!ext) {
            continue;
        }
        if ((size_t)count >= max_entries) {
            break;
        }

        /* Read header for metadata */
        char path[MAX_PATH_LEN];
        snprintf(path, sizeof(path), "%s/%s", VOICE_MEMO_DIR, entry.name);

        struct fs_file_t fp;
        voice_memo_header_t hdr;
        fs_file_t_init(&fp);
        if (fs_open(&fp, path, FS_O_READ) == 0) {
            if (fs_read(&fp, &hdr, sizeof(hdr)) == sizeof(hdr) &&
                memcmp(hdr.magic, VOICE_MEMO_MAGIC, 4) == 0) {
                /* Extract filename without extension */
                size_t name_len = ext - entry.name;
                if (name_len >= VOICE_MEMO_MAX_FILENAME) {
                    name_len = VOICE_MEMO_MAX_FILENAME - 1;
                }
                memcpy(entries[count].filename, entry.name, name_len);
                entries[count].filename[name_len] = '\0';
                entries[count].timestamp = hdr.timestamp;
                entries[count].duration_ms = hdr.duration_ms;
                entries[count].size_bytes = entry.size;
                count++;
            }
            fs_close(&fp);
        }
    }

    fs_closedir(&dirp);
    return count;
}

int voice_memo_store_delete(const char *filename)
{
    char path[MAX_PATH_LEN];
    snprintf(path, sizeof(path), "%s/%s.zsw_opus", VOICE_MEMO_DIR, filename);

    int ret = fs_unlink(path);
    if (ret < 0) {
        LOG_ERR("Failed to delete %s: %d", path, ret);
    } else {
        LOG_INF("[TEST] voice_memo: deleted file=%s", filename);
    }
    return ret;
}

int voice_memo_store_get_free_space(uint32_t *free_bytes)
{
    struct fs_statvfs sbuf;
    int ret = fs_statvfs(VOICE_MEMO_DIR, &sbuf);
    if (ret < 0) {
        /* If recordings dir doesn't exist yet, try parent */
        ret = fs_statvfs("/user", &sbuf);
        if (ret < 0) {
            return ret;
        }
    }

    *free_bytes = (uint32_t)(sbuf.f_frsize * sbuf.f_bfree);
    return 0;
}

int voice_memo_store_get_count(void)
{
    struct fs_dir_t dirp;
    struct fs_dirent entry;
    int count = 0;

    fs_dir_t_init(&dirp);
    if (fs_opendir(&dirp, VOICE_MEMO_DIR) < 0) {
        return 0;
    }

    while (fs_readdir(&dirp, &entry) == 0 && entry.name[0] != '\0') {
        if (entry.type == FS_DIR_ENTRY_FILE && strstr(entry.name, ".zsw_opus")) {
            count++;
        }
    }

    fs_closedir(&dirp);
    return count;
}

const char *voice_memo_store_get_current_filename(void)
{
    return recording_active ? current_filename : NULL;
}

bool voice_memo_store_is_recording(void)
{
    return recording_active;
}
