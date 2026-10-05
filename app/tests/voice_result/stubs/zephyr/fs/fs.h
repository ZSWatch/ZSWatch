#pragma once
#include <dirent.h>
#include <stdbool.h>
#include <stddef.h>
#define FS_O_READ 1
#define FS_O_WRITE 2
#define FS_O_CREATE 4
#define FS_O_TRUNC 8
#define FS_DIR_ENTRY_FILE 1
struct fs_file_t { int fd; };
struct fs_dir_t { DIR *dir; };
struct fs_dirent { char name[256]; int type; };
void fs_file_t_init(struct fs_file_t *file);
void fs_dir_t_init(struct fs_dir_t *dir);
int fs_open(struct fs_file_t *file, const char *path, int flags);
int fs_read(struct fs_file_t *file, void *data, size_t length);
int fs_write(struct fs_file_t *file, const void *data, size_t length);
int fs_sync(struct fs_file_t *file);
int fs_close(struct fs_file_t *file);
int fs_rename(const char *from, const char *to);
int fs_unlink(const char *path);
int fs_mkdir(const char *path);
int fs_opendir(struct fs_dir_t *dir, const char *path);
int fs_readdir(struct fs_dir_t *dir, struct fs_dirent *entry);
int fs_closedir(struct fs_dir_t *dir);
int fs_stat(const char *path, struct fs_dirent *entry);
