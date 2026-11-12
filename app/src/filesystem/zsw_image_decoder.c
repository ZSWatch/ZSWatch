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

/*
 * LVGL image decoder for the image resource filesystem (S:).
 *
 * LVGL's own decoder reads a file image row by row through the filesystem API and
 * copies each row to RAM before it is drawn. While the external flash is memory
 * mapped this decoder instead hands LVGL a pointer straight into the flash, the same
 * way an image compiled into the firmware is drawn. When the flash is not mapped, or
 * the image has a format that must be decoded to RAM, LVGL's decoder does the work.
 *
 * The copy of the custom watchface background (ZSW_CUSTOM_BG_SRC) is drawn the same way.
 * LVGL opens every image file before it asks a decoder, so the copy also gets a drive of
 * its own (B:) that reads it from the flash.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>
#include <lvgl.h>
#include <lvgl_private.h>
#include <libs/bin_decoder/lv_bin_decoder.h>
#include <filesystem/zsw_filesystem.h>
#include <filesystem/zsw_custom_bg.h>

LOG_MODULE_REGISTER(zsw_image_decoder, LOG_LEVEL_INF);

#define ZSW_FS_DRIVE_PREFIX     "S:"
#define ZSW_FS_DRIVE_PREFIX_LEN (sizeof(ZSW_FS_DRIVE_PREFIX) - 1)

static uint32_t num_opened_from_flash;
static uint32_t num_opened_by_lvgl;

static bool can_draw_from_flash(const lv_image_header_t *header)
{
    if (header->magic != LV_IMAGE_HEADER_MAGIC || (header->flags & LV_IMAGE_FLAGS_COMPRESSED)) {
        return false;
    }

    // Formats LVGL draws without first decoding them to RAM.
    switch (header->cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_L8:
            return true;
        default:
            return false;
    }
}

static bool is_custom_bg(const void *src)
{
    return strcmp(src, ZSW_CUSTOM_BG_SRC) == 0;
}

static const uint8_t *map_file(const char *src, uint32_t *len)
{
    if (is_custom_bg(src)) {
        return zsw_custom_bg_map(len);
    }
    return zsw_filesystem_map_file(src + ZSW_FS_DRIVE_PREFIX_LEN, len);
}

static void unmap_file(const char *src)
{
    if (is_custom_bg(src)) {
        zsw_custom_bg_unmap();
    } else {
        zsw_filesystem_unmap_file();
    }
}

static lv_draw_buf_t *open_from_flash(lv_image_decoder_dsc_t *dsc)
{
    uint32_t len;
    const uint8_t *file;
    lv_draw_buf_t *draw_buf;
    const lv_image_header_t *header;

    file = map_file(dsc->src, &len);
    if (!file) {
        return NULL;
    }

    // dsc->header can not be used for the check, LVGL has patched it for old format images.
    header = (const lv_image_header_t *)file;
    draw_buf = lv_malloc(sizeof(lv_draw_buf_t));

    if (draw_buf && len > sizeof(lv_image_header_t) && can_draw_from_flash(header) &&
        lv_draw_buf_init(draw_buf, header->w, header->h, header->cf, dsc->header.stride,
                         (void *)(file + sizeof(lv_image_header_t)),
                         len - sizeof(lv_image_header_t)) == LV_RESULT_OK) {
        draw_buf->header.flags = header->flags;

        // Makes a copy if the caller asked for something the image is not, then it is not worth it.
        lv_draw_buf_t *adjusted = lv_image_decoder_post_process(dsc, draw_buf);
        if (adjusted == draw_buf) {
            return draw_buf;
        }
        if (adjusted) {
            lv_draw_buf_destroy(adjusted);
        }
    }

    lv_free(draw_buf);
    unmap_file(dsc->src);

    return NULL;
}

static bool is_opened_from_flash(const lv_image_decoder_dsc_t *dsc)
{
    // LVGL's decoder never sets these two to the same thing.
    return dsc->user_data && dsc->decoded == dsc->user_data;
}

static lv_result_t decoder_info(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc, lv_image_header_t *header)
{
    if (dsc->src_type != LV_IMAGE_SRC_FILE) {
        return LV_RESULT_INVALID;
    }
    if (!is_custom_bg(dsc->src) && strncmp(dsc->src, ZSW_FS_DRIVE_PREFIX, ZSW_FS_DRIVE_PREFIX_LEN) != 0) {
        return LV_RESULT_INVALID;
    }

    return lv_bin_decoder_info(decoder, dsc, header);
}

static lv_result_t decoder_open(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc)
{
    lv_draw_buf_t *draw_buf = open_from_flash(dsc);

    if (draw_buf) {
        dsc->decoded = draw_buf;
        dsc->user_data = draw_buf;
        num_opened_from_flash++;
        return LV_RESULT_OK;
    }

    num_opened_by_lvgl++;
    return lv_bin_decoder_open(decoder, dsc);
}

static void decoder_close(lv_image_decoder_t *decoder, lv_image_decoder_dsc_t *dsc)
{
    if (is_opened_from_flash(dsc)) {
        lv_free(dsc->user_data);
        dsc->user_data = NULL;
        dsc->decoded = NULL;
        unmap_file(dsc->src);
        return;
    }

    lv_bin_decoder_close(decoder, dsc);
}

#ifdef CONFIG_SHELL
static int cmd_img_stats(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(sh, "S: images drawn from flash: %u, read to RAM by LVGL: %u", num_opened_from_flash,
                num_opened_by_lvgl);
    num_opened_from_flash = 0;
    num_opened_by_lvgl = 0;

    return 0;
}

SHELL_CMD_REGISTER(img_stats, NULL, "Show and reset how S: images were opened for drawing", cmd_img_stats);
#endif

typedef struct {
    uint32_t pos;
    uint32_t len;
} bg_file_t;

static void *bg_fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode)
{
    uint32_t len;
    bg_file_t *file;

    LV_UNUSED(drv);

    // LVGL has removed the drive letter.
    if (mode != LV_FS_MODE_RD || strcmp(path, ZSW_CUSTOM_BG_SRC + 2) != 0 ||
        zsw_custom_bg_read(0, NULL, 0, &len) < 0) {
        return NULL;
    }

    file = lv_malloc(sizeof(bg_file_t));
    if (file) {
        file->pos = 0;
        file->len = len;
    }

    return file;
}

static lv_fs_res_t bg_fs_close(lv_fs_drv_t *drv, void *file_p)
{
    LV_UNUSED(drv);
    lv_free(file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t bg_fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br)
{
    bg_file_t *file = file_p;
    int rc = zsw_custom_bg_read(file->pos, buf, btr, NULL);

    LV_UNUSED(drv);

    if (rc < 0) {
        *br = 0;
        return LV_FS_RES_HW_ERR;
    }
    file->pos += rc;
    *br = rc;

    return LV_FS_RES_OK;
}

static lv_fs_res_t bg_fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence)
{
    bg_file_t *file = file_p;

    LV_UNUSED(drv);

    switch (whence) {
        case LV_FS_SEEK_SET:
            file->pos = pos;
            break;
        case LV_FS_SEEK_CUR:
            file->pos += pos;
            break;
        case LV_FS_SEEK_END:
            file->pos = file->len + pos;
            break;
        default:
            return LV_FS_RES_INV_PARAM;
    }

    return LV_FS_RES_OK;
}

static lv_fs_res_t bg_fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos)
{
    LV_UNUSED(drv);
    *pos = ((bg_file_t *)file_p)->pos;
    return LV_FS_RES_OK;
}

static void register_bg_drive(void)
{
    static lv_fs_drv_t drv;

    lv_fs_drv_init(&drv);
    drv.letter = ZSW_CUSTOM_BG_SRC[0];
    drv.open_cb = bg_fs_open;
    drv.close_cb = bg_fs_close;
    drv.read_cb = bg_fs_read;
    drv.seek_cb = bg_fs_seek;
    drv.tell_cb = bg_fs_tell;
    lv_fs_drv_register(&drv);
}

static int zsw_image_decoder_init(void)
{
    // The newest decoder is asked first, so this one gets the S: images before LVGL's.
    lv_image_decoder_t *decoder = lv_image_decoder_create();

    register_bg_drive();

    if (!decoder) {
        LOG_ERR("Failed to create image decoder");
        return -ENOMEM;
    }

    lv_image_decoder_set_info_cb(decoder, decoder_info);
    lv_image_decoder_set_open_cb(decoder, decoder_open);
    lv_image_decoder_set_get_area_cb(decoder, lv_bin_decoder_get_area);
    lv_image_decoder_set_close_cb(decoder, decoder_close);
    decoder->name = "ZSW_FLASH";

    return 0;
}

SYS_INIT(zsw_image_decoder_init, APPLICATION, 99);
