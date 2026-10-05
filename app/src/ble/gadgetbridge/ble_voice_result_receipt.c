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

static int read_receipt(const char *path, struct voice_receipt *receipt)
{
    struct fs_file_t file;
    fs_file_t_init(&file);
    int rc = fs_open(&file, path, FS_O_READ);
    if (rc < 0) {
        return rc;
    }
    int bytes = fs_read(&file, receipt, sizeof(*receipt));
    unsigned char trailing;
    int tail = bytes == sizeof(*receipt) ? fs_read(&file, &trailing, 1) : -EIO;
    int closed = fs_close(&file);
    return bytes == sizeof(*receipt) && tail == 0 && closed == 0 && receipt->magic == RECEIPT_MAGIC &&
           receipt->checksum == crc32_ieee((const uint8_t *)receipt, offsetof(struct voice_receipt, checksum)) ? 0 : -EIO;
}

/* Explicitly deleting watch audio ends retention of its draft receipt. Never
 * evict a receipt for retained audio, or infer deletion from an I/O error.
 * External calendar/Clock actions are owned and deduplicated by the phone.
 */
static int prune_deleted_recordings(void)
{
    struct fs_dir_t dir;
    struct fs_dirent entry;
    fs_dir_t_init(&dir);
    int rc = fs_opendir(&dir, RECEIPT_DIR);
    if (rc < 0) {
        return rc;
    }
    while ((rc = fs_readdir(&dir, &entry)) == 0 && entry.name[0]) {
        if (entry.type != FS_DIR_ENTRY_FILE || strlen(entry.name) != 68 || strcmp(entry.name + 64, ".bin")) {
            continue;
        }
        char path[100], audio[80];
        struct voice_receipt receipt;
        snprintf(path, sizeof(path), RECEIPT_DIR "/%.68s", entry.name);
        if (read_receipt(path, &receipt) < 0 ||
            !memchr(receipt.event.filename, '\0', sizeof(receipt.event.filename)) ||
            !receipt.event.filename[0] ||
            strspn(receipt.event.filename, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") !=
            strlen(receipt.event.filename)) {
            continue; /* Keep corrupt/unknown receipts rather than resetting deduplication. */
        }
        snprintf(audio, sizeof(audio), "/user/recordings/%s.zsw_opus", receipt.event.filename);
        struct fs_dirent source;
        if (fs_stat(audio, &source) == -ENOENT && (rc = fs_unlink(path)) < 0) {
            break;
        }
    }
    int closed = fs_closedir(&dir);
    return rc < 0 ? rc : closed;
}

static int capacity(bool prune)
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
    if (rc < 0 || close_rc < 0) {
        return rc < 0 ? rc : close_rc;
    }
    if (count < RECEIPT_LIMIT) {
        return 0;
    }
    if (prune) {
        rc = prune_deleted_recordings();
        return rc < 0 ? rc : capacity(false);
    }
    return -ENOSPC;
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
    int rc = read_receipt(path, &previous);
    bool exists = rc == 0;
    if (exists) {
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
    if (!exists && (rc = capacity(true)) < 0) {
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
