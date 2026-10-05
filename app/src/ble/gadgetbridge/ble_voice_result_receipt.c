/* SPDX-License-Identifier: GPL-3.0-only */
#include "ble_voice_result_receipt.h"
#include <zephyr/fs/fs.h>
#include <zephyr/sys/crc.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define RECEIPT_DIR "/user/voice_results"
#define RECEIPT_LIMIT 50
#define RECEIPT_MAGIC 0x3152565aU

struct voice_receipt {
    uint32_t magic;
    uint32_t generation;
    char result_id[65];
    char result_sha256[65];
    struct zsw_voice_memo_result_event event;
    uint32_t checksum;
};

static bool valid_hash(const char *value)
{
    if (!value || strlen(value) != 64) {
        return false;
    }
    return strspn(value, "0123456789abcdef") == 64;
}

static int capacity(void)
{
    struct fs_dir_t dir;
    struct fs_dirent entry;
    fs_dir_t_init(&dir);
    int rc = fs_opendir(&dir, RECEIPT_DIR);
    if (rc < 0) {
        return rc;
    }
    unsigned count = 0;
    while ((rc = fs_readdir(&dir, &entry)) == 0 && entry.name[0]) {
        size_t length = strlen(entry.name);
        if (entry.type == FS_DIR_ENTRY_FILE && length > 4 && strcmp(entry.name + length - 4, ".bin") == 0) {
            count++;
        }
    }
    int close_rc = fs_closedir(&dir);
    return rc < 0 ? rc : close_rc < 0 ? close_rc : count < RECEIPT_LIMIT ? 0 : -ENOSPC;
}

int ble_voice_result_accept(const char *recording_key, const char *result_id,
                           const char *result_sha256, uint32_t generation,
                           const struct zsw_voice_memo_result_event *event)
{
    if (!valid_hash(recording_key) || !valid_hash(result_id) || !valid_hash(result_sha256) ||
        !generation || !event || !event->is_draft || !event->title[0] || !event->filename[0]) {
        return -EINVAL;
    }
    char path[100], temporary[104];
    snprintf(path, sizeof(path), RECEIPT_DIR "/%s.bin", recording_key);
    snprintf(temporary, sizeof(temporary), "%s.tmp", path);
    struct voice_receipt previous = {0};
    struct fs_file_t file;
    fs_file_t_init(&file);
    int rc = fs_open(&file, path, FS_O_READ);
    bool exists = rc == 0;
    if (exists) {
        int bytes = fs_read(&file, &previous, sizeof(previous));
        unsigned char trailing;
        int tail = bytes == sizeof(previous) ? fs_read(&file, &trailing, 1) : -EIO;
        int closed = fs_close(&file);
        if (bytes != sizeof(previous) || tail != 0 || closed < 0 || previous.magic != RECEIPT_MAGIC ||
            previous.checksum != crc32_ieee((const uint8_t *)&previous, offsetof(struct voice_receipt, checksum))) {
            return -EIO; /* Never silently reset a corrupt deduplication receipt. */
        }
        if (generation < previous.generation) {
            return -ESTALE;
        }
        if (generation == previous.generation) {
            return strncmp(previous.result_id, result_id, sizeof(previous.result_id)) == 0 &&
                   strncmp(previous.result_sha256, result_sha256, sizeof(previous.result_sha256)) == 0 &&
                   memcmp(&previous.event, event, sizeof(*event)) == 0 ? 0 : -EINVAL;
        }
    } else if (rc != -ENOENT) {
        return rc;
    }
    rc = fs_mkdir(RECEIPT_DIR);
    if (rc < 0 && rc != -EEXIST) {
        return rc;
    }
    if (!exists && (rc = capacity()) < 0) {
        return rc;
    }
    struct voice_receipt receipt = {.magic = RECEIPT_MAGIC, .generation = generation};
    memcpy(receipt.result_id, result_id, 65);
    memcpy(receipt.result_sha256, result_sha256, 65);
    memcpy(&receipt.event, event, sizeof(*event));
    receipt.checksum = crc32_ieee((const uint8_t *)&receipt, offsetof(struct voice_receipt, checksum));
    fs_file_t_init(&file);
    rc = fs_open(&file, temporary, FS_O_CREATE | FS_O_WRITE | FS_O_TRUNC);
    if (rc < 0) {
        return rc;
    }
    int bytes = fs_write(&file, &receipt, sizeof(receipt));
    rc = bytes == sizeof(receipt) ? fs_sync(&file) : -EIO;
    int closed = fs_close(&file);
    if (rc == 0 && closed < 0) {
        rc = closed;
    }
    if (rc == 0) {
        rc = fs_rename(temporary, path);
    }
    if (rc < 0) {
        fs_unlink(temporary);
        return rc;
    }
    return 1;
}
