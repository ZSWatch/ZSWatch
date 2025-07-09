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

#include "utils/zsw_ui_utils.h"
#include "ble/ble_ancs.h"
#include "managers/zsw_notification_manager.h"
#include <lvgl.h>

#if CONFIG_WATCHFACE_BACKGROUND_SPACE
ZSW_LV_IMG_DECLARE(space_blur_bg);
const lv_img_dsc_t *global_watchface_bg_img = (const lv_img_dsc_t *)ZSW_LV_IMG_USE(space_blur_bg);
#elif CONFIG_WATCHFACE_BACKGROUND_FLOWER
ZSW_LV_IMG_DECLARE(flower_watchface_bg);
const lv_img_dsc_t *global_watchface_bg_img = (const lv_img_dsc_t *)ZSW_LV_IMG_USE(flower_watchface_bg);
#elif CONFIG_WATCHFACE_BACKGROUND_PLANET
ZSW_LV_IMG_DECLARE(earth_blur_move);
const lv_img_dsc_t *global_watchface_bg_img = (const lv_img_dsc_t *)ZSW_LV_IMG_USE(earth_blur_move);
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

ZSW_LV_IMG_DECLARE(ui_img_call_png);
ZSW_LV_IMG_DECLARE(ui_img_gadget_png);
ZSW_LV_IMG_DECLARE(ui_img_apple);
ZSW_LV_IMG_DECLARE(ui_img_mail_png);
ZSW_LV_IMG_DECLARE(ui_img_whatsapp_png);
ZSW_LV_IMG_DECLARE(ui_img_trash_png);
ZSW_LV_IMG_DECLARE(ui_img_homeassistant_png);
ZSW_LV_IMG_DECLARE(ui_img_discord_png);
ZSW_LV_IMG_DECLARE(ui_img_linkedin_png);
ZSW_LV_IMG_DECLARE(fb_messenger_logo);
ZSW_LV_IMG_DECLARE(reddit_logo);
ZSW_LV_IMG_DECLARE(youtube_logo);
ZSW_LV_IMG_DECLARE(sms_icon);
ZSW_LV_IMG_DECLARE(google_calendar_icon);

ZSW_LV_IMG_DECLARE(clear);
ZSW_LV_IMG_DECLARE(dense_drizzle);
ZSW_LV_IMG_DECLARE(dense_freezing_drizzle);
ZSW_LV_IMG_DECLARE(fog);
ZSW_LV_IMG_DECLARE(heavy_freezing_rain);
ZSW_LV_IMG_DECLARE(heavy_rain);
ZSW_LV_IMG_DECLARE(heavy_snowfall);
ZSW_LV_IMG_DECLARE(light_drizzle);
ZSW_LV_IMG_DECLARE(light_freezing_drizzle);
ZSW_LV_IMG_DECLARE(light_freezing_rain);
ZSW_LV_IMG_DECLARE(light_rain);
ZSW_LV_IMG_DECLARE(moderate_drizzle);
ZSW_LV_IMG_DECLARE(moderate_rain);
ZSW_LV_IMG_DECLARE(moderate_snowfall);
ZSW_LV_IMG_DECLARE(mostly_clear);
ZSW_LV_IMG_DECLARE(overcast);
ZSW_LV_IMG_DECLARE(partly_cloudy1);
ZSW_LV_IMG_DECLARE(rime_fog);
ZSW_LV_IMG_DECLARE(slight_snowfall);
ZSW_LV_IMG_DECLARE(snowflake);
ZSW_LV_IMG_DECLARE(thunderstorm_with_hail);
ZSW_LV_IMG_DECLARE(thunderstorm);

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
            printk("Unhandled weather code: %d", code);
            return &unknown;
        }
    }
}

int wmo_code_to_weather_code(int wmo_code)
{
    switch (wmo_code) {
        case 0: {
            return 800;
        }
        case 1: {
            return 800;
        }
        case 2: {
            return 800;
        }
        case 3: {
            return 803;
        }
        case 45: {
            return 700;
        }
        case 48: {
            return 700;
        }
        case 51: {
            return 700;
        }
        case 53: {
            return 512;
        }
        case 55: {
            return 512;
        }
        case 80: {
            return 512;
        }
        case 81: {
            return 512;
        }
        case 82: {
            return 512;
        }
        case 61: {
            return 512;
        }
        case 63: {
            return 512;
        }
        case 65: {
            return 512;
        }
        case 56: {
            return 512;
        }
        case 57: {
            return 512;
        }
        case 66: {
            return 512;
        }
        case 67: {
            return 512;
        }
        case 71: {
            return 511;
        }
        case 73: {
            return 511;
        }
        case 75: {
            return 511;
        }
        case 77: {
            return 511;
        }
        case 85: {
            return 511;
        }
        case 86: {
            return 511;
        }
        case 95: {
            return 200;
        }
        case 96: {
            return 200;
        }
        case 99: {
            return 200;
        }
    }
    return -1;
}

const void *zsw_ui_utils_icon_from_wmo_weather_code(int code, lv_color_t *color, char **text)
{
    switch (code) {
        case 0: {
            *color = lv_color_hex(0xF1F1F1);
            *text = "Clear";
            return ZSW_LV_IMG_USE(clear);
            break;
        }
        case 1: {
            *color = lv_color_hex(0xE2E2E2);
            *text = "Mostly Clear";
            return ZSW_LV_IMG_USE(mostly_clear);
            break;
        }
        case 2: {
            *color = lv_color_hex(0xC6C6C6);
            *text = "Partly Cloudy";
            return ZSW_LV_IMG_USE(partly_cloudy1);
            break;
        }
        case 3: {
            *color = lv_color_hex(0xABABAB);
            *text = "Overcast";
            return ZSW_LV_IMG_USE(overcast);
            break;
        }
        case 45: {
            *color = lv_color_hex(0xA4ACBA);
            *text = "Fog";
            return ZSW_LV_IMG_USE(fog);
            break;
        }
        case 48: {
            *color = lv_color_hex(0x8891A4);
            *text = "Icy Fog";
            return ZSW_LV_IMG_USE(rime_fog);
            break;
        }
        case 51: {
            *color = lv_color_hex(0x3DECEB);
            *text = "Light Drizzle";
            return ZSW_LV_IMG_USE(light_drizzle);
            break;
        }
        case 53: {
            *color = lv_color_hex(0x0CCECE);
            *text = "Drizzle";
            return ZSW_LV_IMG_USE(moderate_drizzle);
            break;
        }
        case 55: {
            *color = lv_color_hex(0x0AB1B1);
            *text = "Heavy Drizzle";
            return ZSW_LV_IMG_USE(dense_drizzle);
            break;
        }
        case 80: {
            *color = lv_color_hex(0x9BCCFD);
            *text = "Light Showers";
            return ZSW_LV_IMG_USE(light_rain);
            break;
        }
        case 81: {
            *color = lv_color_hex(0x51B4FF);
            *text = "Showers";
            return ZSW_LV_IMG_USE(moderate_rain);
            break;
        }
        case 82: {
            *color = lv_color_hex(0x029AE8);
            *text = "Heavy Showers";
            return ZSW_LV_IMG_USE(heavy_rain);
            break;
        }
        case 61: {
            *color = lv_color_hex(0xBFC3FA);
            *text = "Light Rain";
            return ZSW_LV_IMG_USE(light_rain);
            break;
        }
        case 63: {
            *color = lv_color_hex(0x9CA7FA);
            *text = "Rain";
            return ZSW_LV_IMG_USE(moderate_rain);
            break;
        }
        case 65: {
            *color = lv_color_hex(0x748BF8);
            *text = "Heavy Rain";
            return ZSW_LV_IMG_USE(heavy_rain);
            break;
        }
        case 56: {
            *color = lv_color_hex(0xD3BFE8);
            *text = "Light Freezing Drizzle";
            return ZSW_LV_IMG_USE(light_freezing_drizzle);
            break;
        }
        case 57: {
            *color = lv_color_hex(0xA780D4);
            *text = "Freezing Drizzle";
            return ZSW_LV_IMG_USE(dense_freezing_drizzle);
            break;
        }
        case 66: {
            *color = lv_color_hex(0xCAC1EE);
            *text = "Light Freezing Rain";
            return ZSW_LV_IMG_USE(light_freezing_rain);
            break;
        }
        case 67: {
            *color = lv_color_hex(0x9486E1);
            *text = "Freezing Rain";
            return ZSW_LV_IMG_USE(heavy_freezing_rain);
            break;
        }
        case 71: {
            *color = lv_color_hex(0xF9B1D8);
            *text = "Light Snow";
            return ZSW_LV_IMG_USE(slight_snowfall);
            break;
        }
        case 73: {
            *color = lv_color_hex(0xF983C7);
            *text = "Snow";
            return ZSW_LV_IMG_USE(moderate_snowfall);
            break;
        }
        case 75: {
            *color = lv_color_hex(0xF748B7);
            *text = "Heavy Snow";
            return ZSW_LV_IMG_USE(heavy_snowfall);
            break;
        }
        case 77: {
            *color = lv_color_hex(0xE7B6EE);
            *text = "Snow Grains";
            return ZSW_LV_IMG_USE(snowflake);
            break;
        }
        case 85: {
            *color = lv_color_hex(0xE7B6EE);
            *text = "Light Snow Showers";
            return ZSW_LV_IMG_USE(slight_snowfall);
            break;
        }
        case 86: {
            *color = lv_color_hex(0xCD68E0);
            *text = "Snow Showers";
            return ZSW_LV_IMG_USE(heavy_snowfall);
            break;

            case 95: {
                *color = lv_color_hex(0x525F7A);
                *text = "Thunderstorm";
                return ZSW_LV_IMG_USE(thunderstorm);
                break;
            }
            case 96: {
                *color = lv_color_hex(0x3D475C);
                *text = "Light T-storm w/ Hail";
                return ZSW_LV_IMG_USE(thunderstorm_with_hail);
                break;
            }
            case 99: {
                *color = lv_color_hex(0x2A3140);
                *text = "T-storm w/ Hail";
                return ZSW_LV_IMG_USE(thunderstorm_with_hail);
                break;
            }
        }
    }
    return NULL;
}

const void *zsw_ui_utils_icon_from_notification(zsw_notification_src_t src)
{
    switch (src) {
        case NOTIFICATION_SRC_FB_MESSENGER:
            return ZSW_LV_IMG_USE(fb_messenger_logo);
        case NOTIFICATION_SRC_COMMON_MESSENGER:
            return ZSW_LV_IMG_USE(sms_icon);
        case NOTIFICATION_SRC_WHATSAPP:
            return ZSW_LV_IMG_USE(ui_img_whatsapp_png);
        case NOTIFICATION_SRC_DISCORD:
            return ZSW_LV_IMG_USE(ui_img_discord_png);
        case NOTIFICATION_SRC_GMAIL:
            return ZSW_LV_IMG_USE(ui_img_mail_png);
        case NOTIFICATION_SRC_COMMON_MAIL:
            return ZSW_LV_IMG_USE(ui_img_mail_png);
        case NOTIFICATION_SRC_HOME_ASSISTANT:
            return ZSW_LV_IMG_USE(ui_img_homeassistant_png);
        case NOTIFICATION_SRC_LINKEDIN:
            return ZSW_LV_IMG_USE(ui_img_linkedin_png);
        case NOTIFICATION_SRC_REDDIT:
            return ZSW_LV_IMG_USE(reddit_logo);
        case NOTIFICATION_SRC_YOUTUBE:
            return ZSW_LV_IMG_USE(youtube_logo);
        case NOTIFICATION_SRC_CALENDAR:
            return ZSW_LV_IMG_USE(google_calendar_icon);
        default:
#ifdef CONFIG_BT_ANCS_CLIENT
            if (ble_ancs_present()) {
                return ZSW_LV_IMG_USE(ui_img_apple);
            } else {
#else
            {
#endif
                return ZSW_LV_IMG_USE(ui_img_gadget_png);
            }
    }
}

const char *zsw_ui_utils_source_from_notification(zsw_notification_src_t src)
{
    switch (src) {
        case NOTIFICATION_SRC_FB_MESSENGER:
            return "Messenger";
        case NOTIFICATION_SRC_COMMON_MESSENGER:
            return "Messages";
        case NOTIFICATION_SRC_WHATSAPP:
            return "WhatsApp";
        case NOTIFICATION_SRC_DISCORD:
            return "Discord";
        case NOTIFICATION_SRC_GMAIL:
            return "Gmail";
        case NOTIFICATION_SRC_COMMON_MAIL:
            return "Mail";
        case NOTIFICATION_SRC_HOME_ASSISTANT:
            return "Home Assistant";
        case NOTIFICATION_SRC_LINKEDIN:
            return "LinkedIn";
        case NOTIFICATION_SRC_REDDIT:
            return "Reddit";
        case NOTIFICATION_SRC_YOUTUBE:
            return "YouTube";
        case NOTIFICATION_SRC_CALENDAR:
            return "Calendar";
        default:
            return "Unknown";
    }
}

void zsw_ui_utils_seconds_to_day_hour_min(int seconds, int *days, int *hours, int *minutes)
{
    *days = seconds / (24 * 3600);
    seconds = seconds % (24 * 3600);
    *hours = seconds / 3600;
    seconds = seconds % 3600;
    *minutes = seconds / 60;
}