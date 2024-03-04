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

#include <lvgl.h>
#include "utils/zsw_ui_utils.h"
#include "managers/zsw_notification_manager.h"

#if CONFIG_WATCHFACE_BACKGROUND_SPACE
LV_IMG_DECLARE(space_blur_bg);
const lv_img_dsc_t *global_watchface_bg_img = &space_blur_bg;
#elif CONFIG_WATCHFACE_BACKGROUND_FLOWER
LV_IMG_DECLARE(flower_watchface_bg);
const lv_img_dsc_t *global_watchface_bg_img = &flower_watchface_bg;
#elif CONFIG_WATCHFACE_BACKGROUND_PLANET
LV_IMG_DECLARE(earth_blur_move);
const lv_img_dsc_t *global_watchface_bg_img = &earth_blur_move;
#else
const lv_img_dsc_t *global_watchface_bg_img = NULL;
#endif

LV_IMG_DECLARE(stormy);
LV_IMG_DECLARE(snowy);
LV_IMG_DECLARE(rainy);
LV_IMG_DECLARE(snowy);
LV_IMG_DECLARE(foggy);
LV_IMG_DECLARE(sunny);
LV_IMG_DECLARE(partly_cloudy);
LV_IMG_DECLARE(cloudy);
LV_IMG_DECLARE(unknown);

ZSW_LV_IMG_DECLARE(ui_img_whatsapp_png);
ZSW_LV_IMG_DECLARE(ui_img_gadget_png);
ZSW_LV_IMG_DECLARE(ui_img_mail_png);
ZSW_LV_IMG_DECLARE(ui_img_call_png);
ZSW_LV_IMG_DECLARE(ui_img_trash_png);
ZSW_LV_IMG_DECLARE(ui_img_youtube_png);
ZSW_LV_IMG_DECLARE(ui_img_homeassistant_png);
ZSW_LV_IMG_DECLARE(ui_img_discord_png);
ZSW_LV_IMG_DECLARE(ui_img_linkedin_png);
ZSW_LV_IMG_DECLARE(fb_messenger_logo);
ZSW_LV_IMG_DECLARE(reddit_logo);
ZSW_LV_IMG_DECLARE(youtube_logo);

const lv_img_dsc_t *zsw_ui_utils_icon_from_weather_code(int code, lv_color_t *icon_color)
{
    int code_group = code / 100;

    *icon_color = lv_color_white();

    switch (code_group) {
        case 2:
            *icon_color = lv_color_hex(0xD5DFE7);
            return &stormy;
        case 3:
            *icon_color = lv_color_hex(0xD5DFE7);
            return &cloudy;
        case 5:
            switch (code) {
                case 511:
                    *icon_color = lv_color_white();
                    return &snowy;
                default:
                    *icon_color = lv_color_hex(0x5275A2);
                    return &rainy;
            }
        case 6:
            *icon_color = lv_color_white();
            return &snowy;
        case 7:
            *icon_color = lv_color_hex(0x5275A2);
            return &foggy;
        case 8:
            switch (code) {
                case 800:
                    // TODO if day sunny else moon
                    *icon_color = lv_color_hex(0xFBD92A);
                    return &sunny;
                case 801:
                    *icon_color = lv_color_white();
                    return &partly_cloudy;
                case 802:
                    *icon_color = lv_color_hex(0xD5DFE7);
                    return &partly_cloudy;
                default:
                    *icon_color = lv_color_hex(0xD5DFE7);
                    return &cloudy;
            }
        default: {
            printf("Unhandled weather code: %d", code);
            return &unknown;
        }
    }
}

const void *zsw_ui_utils_icon_from_notification(zsw_notification_src_t src)
{
    switch (src) {
        case NOTIFICATION_SRC_FB_MESSENGER:
            return ZSW_LV_IMG_USE(fb_messenger_logo);
            break;
        case NOTIFICATION_SRC_COMMON_MESSENGER:
            return ZSW_LV_IMG_USE(ui_img_whatsapp_png);
            break;
        case NOTIFICATION_SRC_WHATSAPP:
            return ZSW_LV_IMG_USE(ui_img_whatsapp_png);
            break;
        case NOTIFICATION_SRC_DISCORD:
            return ZSW_LV_IMG_USE(ui_img_discord_png);
            break;
        case NOTIFICATION_SRC_GMAIL:
            return ZSW_LV_IMG_USE(ui_img_mail_png);
            break;
        case NOTIFICATION_SRC_COMMON_MAIL:
            return ZSW_LV_IMG_USE(ui_img_mail_png);
            break;
        case NOTIFICATION_SRC_HOME_ASSISTANT:
            return ZSW_LV_IMG_USE(ui_img_homeassistant_png);
            break;
        case NOTIFICATION_SRC_LINKEDIN:
            return ZSW_LV_IMG_USE(ui_img_linkedin_png);
            break;
        case NOTIFICATION_SRC_REDDIT:
            return ZSW_LV_IMG_USE(reddit_logo);
        case NOTIFICATION_SRC_YOUTUBE:
            return ZSW_LV_IMG_USE(youtube_logo);
        default:
            return ZSW_LV_IMG_USE(ui_img_gadget_png);
    }
}

const char *zsw_ui_utils_source_from_notification(zsw_notification_src_t src)
{
    switch (src) {
        case NOTIFICATION_SRC_FB_MESSENGER:
            return "Messenger";
            break;
        case NOTIFICATION_SRC_COMMON_MESSENGER:
            return "Messages";
            break;
        case NOTIFICATION_SRC_WHATSAPP:
            return "WhatsApp";
            break;
        case NOTIFICATION_SRC_DISCORD:
            return "Discord";
            break;
        case NOTIFICATION_SRC_GMAIL:
            return "Gmail";
            break;
        case NOTIFICATION_SRC_COMMON_MAIL:
            return "Mail";
            break;
        case NOTIFICATION_SRC_HOME_ASSISTANT:
            return "Home Assistant";
            break;
        case NOTIFICATION_SRC_LINKEDIN:
            return "LinkedIn";
            break;
        case NOTIFICATION_SRC_REDDIT:
            return "Reddit";
            break;
        case NOTIFICATION_SRC_YOUTUBE:
            return "YouTube";
            break;
        default:
            return "Unknown";
            break;
    }
}