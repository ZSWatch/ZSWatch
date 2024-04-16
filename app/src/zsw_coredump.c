#include <zsw_coredump.h>
#if !defined(CONFIG_BOARD_NATIVE_POSIX) && defined(CONFIG_FILE_SYSTEM)
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zsw_retained_ram_storage.h>
#include <stdlib.h>
#include <zephyr/device.h>
#include <zephyr/retention/retention.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/sys/util.h>
#include <zephyr/debug/coredump.h>
#include <zephyr/fs/fs.h>

LOG_MODULE_REGISTER(zsw_coredump, LOG_LEVEL_DBG);

/**
 * Those are picked from coredump_internal.h in zephyr.
 * Save the coredump it in the same format.
 * Allows to use the coredump uart command to process the coredump file.
 */
#define COREDUMP_BEGIN_STR  "BEGIN#\r\n"
#define COREDUMP_END_STR    "END#\r\n"
#define COREDUMP_ERROR_STR  "ERROR CANNOT DUMP#\r\n"
#define COREDUMP_PREFIX_STR "#CD:"

#define COREDUMP_LINE_OVERHEAD (sizeof(COREDUMP_PREFIX_STR) + sizeof("\r\n") + 1)

#define MAX_FILENAME_LEN 32
#define FILE_CHUNK_LENGTH 256

struct crash_info_header {
    uint32_t crash_line;
    uint8_t crash_file[MAX_FILENAME_LEN];
    uint32_t length;
};

static const struct device *retention_area = DEVICE_DT_GET(DT_NODELABEL(retention_coredump));

int zsw_coredump_to_log(void)
{
    int err;
    struct fs_file_t file;
    const char *path = "/lvgl_lfs/coredump.txt";

    fs_file_t_init(&file);
    err = fs_open(&file, path, FS_O_READ);
    if (err) {
        LOG_ERR("Failed to open %s (%d)", path, err);
        return err;
    }

    err = fs_seek(&file, 0, FS_SEEK_SET);
    if (err) {
        LOG_ERR("Failed to seek %s (%d)", path, err);
        err = fs_close(&file);
        return err;
    }

    uint8_t buf[FILE_CHUNK_LENGTH + 1];
    while (true) {
        ssize_t read;

        read = fs_read(&file, buf, FILE_CHUNK_LENGTH);
        if (read <= 0) {
            break;
        }

        buf[read] = '\0';

        LOG_PRINTK("%s", buf);
    }

    return 0;
}

static int read_crash_header(struct crash_info_header *header)
{
    return retention_read(retention_area, 0, (uint8_t *)header, sizeof(struct crash_info_header));
}

static int write_crash_header(struct crash_info_header *header)
{
    return retention_write(retention_area, 0, (const uint8_t *)header, sizeof(struct crash_info_header));
}

static void clear_stored_dump(void)
{
    struct crash_info_header header;
    if (read_crash_header(&header) == 0) {
        header.length = 0;
        write_crash_header(&header);
    } else {
        retention_clear(retention_area);
    }
}

static int write_coredump_to_filesystem(void)
{
    int err;
    int len;
    int i;
    int out_index;
    struct fs_file_t file;
    struct coredump_cmd_copy_arg args;
    const char *path = "/lvgl_lfs/coredump.txt";
    char file_write_chunk[FILE_CHUNK_LENGTH] = {0};
    uint8_t coredump[FILE_CHUNK_LENGTH / 2 - COREDUMP_LINE_OVERHEAD] = {0};

    fs_unlink(path); // For now only handle one file and delete it always

    fs_file_t_init(&file);
    err = fs_open(&file, path, FS_O_CREATE | FS_O_WRITE);
    if (err) {
        LOG_ERR("Failed to open %s (%d)", path, err);
        return err;
    }

    err = fs_seek(&file, 0, FS_SEEK_SET);
    if (err) {
        LOG_ERR("Failed to seek %s (%d)", path, err);
        err = fs_close(&file);
        return err;
    }

    args.buffer = coredump;
    args.offset = 0;
    args.length = sizeof(coredump);

    err = fs_write(&file, COREDUMP_PREFIX_STR COREDUMP_BEGIN_STR, strlen(COREDUMP_PREFIX_STR COREDUMP_BEGIN_STR));
    while (err >= 0 && (len = coredump_cmd(COREDUMP_CMD_COPY_STORED_DUMP, &args)) != 0) {
        __ASSERT(len <= sizeof(coredump), "Invalid coredump read length");
        args.offset += len;
        strncpy(file_write_chunk, COREDUMP_PREFIX_STR, sizeof(file_write_chunk));
        out_index = strlen(COREDUMP_PREFIX_STR);
        i = 0;
        // Below code comes from zephyr/subsys/debug/coredump/coredump_backend_logging.c
        // Function coredump_logging_backend_buffer_output
        // Needed to output the coredump in a text represented format to the filesystem
        while (len > 0) {
            if (hex2char(args.buffer[i] >> 4, &file_write_chunk[out_index]) < 0) {
                err = -EINVAL;
                break;
            }
            out_index++;

            if (hex2char(args.buffer[i] & 0xf, &file_write_chunk[out_index]) < 0) {
                err = -EINVAL;
                break;
            }
            out_index++;
            len--;
            i++;
            __ASSERT(out_index < FILE_CHUNK_LENGTH, "Invalid coredump length");
        }
        out_index += snprintf(&file_write_chunk[out_index], FILE_CHUNK_LENGTH - out_index, "\r\n");
        err = fs_write(&file, file_write_chunk, out_index);
        if (err < 0) {
            LOG_ERR("Failed to write coredump: %d", err);
            break;
        }
        out_index = 0;
        memset(coredump, 0, sizeof(coredump));
    }
    if (err >= 0) {
        fs_write(&file, COREDUMP_PREFIX_STR COREDUMP_END_STR, strlen(COREDUMP_PREFIX_STR COREDUMP_END_STR));
    }

    fs_close(&file);

    if (err < 0) {
        err = fs_unlink(path);
    }

    coredump_cmd(COREDUMP_CMD_INVALIDATE_STORED_DUMP, &args);

    return err;
}

static void coredump_logging_backend_start(void)
{
    clear_stored_dump();
}

static void coredump_logging_backend_end(void)
{
}

static void coredump_logging_backend_buffer_output(uint8_t *buf, size_t buflen)
{
    int ret;
    struct crash_info_header header;

    ret = read_crash_header(&header);
    if (ret != 0) {
        LOG_ERR("Failed to read crash header: %d", ret);
    }

    if (header.length + buflen > retention_size(retention_area)) {
        buflen = retention_size(retention_area) - header.length;
        LOG_ERR("Core dump too large cropped to %d", buflen);
    }

    ret = retention_write(retention_area, sizeof(struct crash_info_header) + header.length, buf, buflen);
    LOG_DBG("Writing at offset %d length %d", sizeof(struct crash_info_header) + header.length, buflen);
    if (ret != 0) {
        LOG_ERR("Failed to write coredump: %d", ret);
    }
    header.length = header.length + buflen;
    ret = write_crash_header(&header);
    if (ret != 0) {
        LOG_ERR("Failed to write crash header: %d", ret);
    }
}

static int coredump_logging_backend_query(enum coredump_query_id query_id,
                                          void *arg)
{
    int ret;

    switch (query_id) {
        case COREDUMP_QUERY_GET_ERROR:
            ret = 0;
            break;
        default:
            ret = -ENOTSUP;
            break;
    }

    return ret;
}

static int coredump_logging_backend_cmd(enum coredump_cmd_id cmd_id,
                                        void *arg)
{
    int ret = 0;
    struct crash_info_header header;

    switch (cmd_id) {
        case COREDUMP_CMD_CLEAR_ERROR:
            ret = 0;
            break;
        case COREDUMP_CMD_VERIFY_STORED_DUMP:
            if (read_crash_header(&header)) {
                ret = retention_is_valid(retention_area) && header.length > 0;
            } else {
                ret = 0;
            }
        case COREDUMP_CMD_ERASE_STORED_DUMP:
            clear_stored_dump();
            break;
        case COREDUMP_CMD_COPY_STORED_DUMP:
            ret = read_crash_header(&header);
            if (ret == 0 && arg && retention_is_valid(retention_area)) {
                struct coredump_cmd_copy_arg *copy_arg = (struct coredump_cmd_copy_arg *)arg;
                int toCopy = MIN(header.length - copy_arg->offset, copy_arg->length);
                retention_read(retention_area, sizeof(struct crash_info_header) + copy_arg->offset,
                               copy_arg->buffer, toCopy);
                ret = toCopy;
            } else {
                ret = -EINVAL;
            }
            break;
        case COREDUMP_CMD_INVALIDATE_STORED_DUMP:
            clear_stored_dump();
            break;
        default:
            ret = -ENOTSUP;
            break;
    }

    return ret;
}

struct coredump_backend_api coredump_backend_other = {
    .start = coredump_logging_backend_start,
    .end = coredump_logging_backend_end,
    .buffer_output = coredump_logging_backend_buffer_output,
    .query = coredump_logging_backend_query,
    .cmd = coredump_logging_backend_cmd,
};

void assert_post_action(const char *file, unsigned int line)
{
    const char *fileName;
    struct crash_info_header header;

    fileName = file;

    if (strlen(file) > MAX_FILENAME_LEN) {
        // Copy end of filename as it's more relevant than the path to the file.
        fileName += strlen(file) - MAX_FILENAME_LEN;
    }
    strncpy(header.crash_file, fileName, MAX_FILENAME_LEN);
    header.crash_line = line;
    write_crash_header(&header);

    k_panic();
    sys_reboot(SYS_REBOOT_COLD);
}

static int coredump_init(void)
{
    if (retention_is_valid(retention_area)) {
        struct crash_info_header header;
        int ret = read_crash_header(&header);
        if (ret == 0 && header.crash_line != 0 && header.crash_file[0] != '\0') {
            LOG_ERR("ASSERT: %s (line: %d)", header.crash_file, header.crash_line);
        } else {
            LOG_DBG("No assert found");
        }

        if (header.length > 0) {
            write_coredump_to_filesystem();
        }
    } else {
        retention_clear(retention_area);
    }

    return 0;
}

SYS_INIT(coredump_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#else
int zsw_coredump_to_log(void)
{
    return 0;
}
#endif