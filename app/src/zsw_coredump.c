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

LOG_MODULE_REGISTER(zsw_coredump, LOG_LEVEL_DBG);

#define MAX_FILENAME_LEN 32

struct crash_info_header {
    uint32_t crash_line;
    uint8_t crash_file[MAX_FILENAME_LEN];
    uint32_t length;
};

static const struct device *retention_area = DEVICE_DT_GET(DT_NODELABEL(retention_coredump));

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
    int ret;
    struct crash_info_header header;
    if (read_crash_header(&header) == 0) {
        header.length = 0;
        write_crash_header(&header);
    } else {
        retention_clear(retention_area);
    }
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
        // TODO Write to Filesystem
    } else {
        retention_clear(retention_area);
    }

    return 0;
}

SYS_INIT(coredump_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
