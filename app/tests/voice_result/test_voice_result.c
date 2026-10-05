#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <zephyr/fs/fs.h>
#include <zephyr/sys/crc.h>
#include "ble/gadgetbridge/ble_voice_result_receipt.h"

static int write_fault, rename_fault;
static const char *mapped(const char *path) { return path + strlen("/user/"); }
static int checked(int rc) { return rc < 0 ? -errno : rc; }
void fs_file_t_init(struct fs_file_t *file) { file->fd = -1; }
void fs_dir_t_init(struct fs_dir_t *dir) { dir->dir = NULL; }
int fs_open(struct fs_file_t *file, const char *path, int flags)
{
    int mode = flags & FS_O_WRITE ? O_WRONLY : O_RDONLY;
    if (flags & FS_O_CREATE) { mode |= O_CREAT; }
    if (flags & FS_O_TRUNC) { mode |= O_TRUNC; }
    file->fd = open(mapped(path), mode, 0600);
    return file->fd < 0 ? -errno : 0;
}
int fs_read(struct fs_file_t *file, void *data, size_t length) { return checked(read(file->fd, data, length)); }
int fs_write(struct fs_file_t *file, const void *data, size_t length)
{
    if (write_fault) { write_fault = 0; length /= 2; }
    return checked(write(file->fd, data, length));
}
int fs_sync(struct fs_file_t *file) { return checked(fsync(file->fd)); }
int fs_close(struct fs_file_t *file) { return checked(close(file->fd)); }
int fs_rename(const char *from, const char *to)
{
    if (rename_fault) { rename_fault = 0; return -EIO; }
    return checked(rename(mapped(from), mapped(to)));
}
int fs_unlink(const char *path) { return checked(unlink(mapped(path))); }
int fs_mkdir(const char *path) { return checked(mkdir(mapped(path), 0700)); }
int fs_opendir(struct fs_dir_t *dir, const char *path)
{
    dir->dir = opendir(mapped(path));
    return dir->dir ? 0 : -errno;
}
int fs_readdir(struct fs_dir_t *dir, struct fs_dirent *entry)
{
    errno = 0;
    struct dirent *next = readdir(dir->dir);
    if (!next) { entry->name[0] = 0; return errno ? -errno : 0; }
    snprintf(entry->name, sizeof(entry->name), "%s", next->d_name);
    entry->type = strcmp(next->d_name, ".") && strcmp(next->d_name, "..") ? FS_DIR_ENTRY_FILE : 0;
    return 0;
}
int fs_closedir(struct fs_dir_t *dir) { return checked(closedir(dir->dir)); }

int main(void)
{
    const char *key = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    const char *id = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
    const char *sha = "cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc";
    struct zsw_voice_memo_result_event event = {.title = "Owned reminder", .filename = "owned", .is_draft = true};
    assert(crc32_ieee((const uint8_t *)"123456789", 9) == 0xcbf43926U);
    assert(ble_voice_result_accept("../bad", id, sha, 1, &event) == -EINVAL);
    write_fault = 1;
    assert(ble_voice_result_accept(key, id, sha, 1, &event) == -EIO);
    assert(ble_voice_result_accept(key, id, sha, 1, &event) == 1);
    // Every call reopens disk: a reset/retry cannot republish the popup.
    assert(ble_voice_result_accept(key, id, sha, 1, &event) == 0);
    strcpy(event.title, "Changed payload");
    assert(ble_voice_result_accept(key, id, sha, 1, &event) == -EINVAL);
    rename_fault = 1;
    assert(ble_voice_result_accept(key, id, sha, 2, &event) == -EIO);
    strcpy(event.title, "Owned reminder");
    assert(ble_voice_result_accept(key, id, sha, 1, &event) == 0);
    assert(ble_voice_result_accept(key, id, sha, 2, &event) == 1);
    assert(ble_voice_result_accept(key, id, sha, 1, &event) == -ESTALE);
    event.is_draft = false;
    assert(ble_voice_result_accept(key, id, sha, 3, &event) == -EINVAL);
    event.is_draft = true;
    char path[100];
    snprintf(path, sizeof(path), "voice_results/%s.bin", key);
    int fd = open(path, O_WRONLY);
    assert(fd >= 0 && write(fd, "corrupt", 7) == 7 && close(fd) == 0);
    assert(ble_voice_result_accept(key, id, sha, 3, &event) == -EIO);
    assert(unlink(path) == 0);
    // Filling capacity does not evict old deduplication receipts.
    for (unsigned i = 0; i < 50; i++) {
        char other[65];
        snprintf(other, sizeof(other), "%064x", i);
        assert(ble_voice_result_accept(other, id, sha, 1, &event) == 1);
    }
    assert(ble_voice_result_accept(key, id, sha, 1, &event) == -ENOSPC);
    puts("Receipt recovery, duplicate, conflict, corruption and capacity checks passed");
    return 0;
}
