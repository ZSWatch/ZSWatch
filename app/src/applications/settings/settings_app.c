#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/settings/settings.h>

#include "settings_ui.h"
#include "ble/ble_aoa.h"
#include "ble/ble_comm.h"
#include "sensors/zsw_imu.h"
#include "drivers/zsw_display_control.h"
#include "managers/zsw_app_manager.h"
#include "zsw_settings.h"
#include <filesystem/zsw_rtt_flash_loader.h>
#include "ui/popup/zsw_popup_window.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(settings_app, CONFIG_ZSW_SETTINGS_APP_LOG_LEVEL);

static void settings_app_start(lv_obj_t *root, lv_group_t *group);
static void settings_app_stop(void);

static void on_close_settings(void);
static void on_brightness_changed(lv_setting_value_t value, bool final);
static void on_display_on_changed(lv_setting_value_t value, bool final);
static void on_display_vib_press_changed(lv_setting_value_t value, bool final);
static void on_aoa_enable_changed(lv_setting_value_t value, bool final);
static void on_aoa_interval_changed(lv_setting_value_t value, bool final);
static void on_pairing_enable_changed(lv_setting_value_t value, bool final);
static void on_reset_steps_changed(lv_setting_value_t value, bool final);
static void on_clear_bonded_changed(lv_setting_value_t value, bool final);
static void on_clear_storage_changed(lv_setting_value_t value, bool final);
static void on_reboot_changed(lv_setting_value_t value, bool final);
static void on_restart_screen_changed(lv_setting_value_t value, bool final);
static void on_watchface_animation_changed(lv_setting_value_t value, bool final);
static void on_watchface_tick_interval_changed(lv_setting_value_t value, bool final);

static void ble_pairing_work_handler(struct k_work *work);
static void display_restart_work_handler(struct k_work *work);

LV_IMG_DECLARE(settings);

typedef struct setting_app {
    zsw_settings_brightness_t           brightness;
    zsw_settings_vib_on_press_t         vibration_on_click;
    zsw_settings_display_always_on_t    display_always_on;
    zsw_settings_ble_aoa_en_t           ble_aoa_enabled;
    zsw_settings_ble_aoa_int_t          ble_aoa_tx_interval;
    zsw_settings_watchface_t            watchface;
} setting_app_t;

K_WORK_DELAYABLE_DEFINE(ble_pairing_dwork, ble_pairing_work_handler);
K_WORK_DEFINE(display_restart_work, display_restart_work_handler);

// Default values.
static setting_app_t settings_app = {
    .brightness = 30,
    .vibration_on_click = true,
    .display_always_on = false,
    .ble_aoa_enabled = false,
    .ble_aoa_tx_interval = 100,
    .watchface = {
        .animations_on = false,
        .default_index = 0,
        .smooth_second_hand = false
    },
};

static application_t app = {
    .name = "Settings",
    .icon = &settings,
    .start_func = settings_app_start,
    .stop_func = settings_app_stop
};

static lv_settings_item_t display_page_items[] = {
    {
        .type = LV_SETTINGS_TYPE_SLIDER,
        .icon = LV_SYMBOL_SETTINGS,
        .change_callback = on_brightness_changed,
        .item = {
            .slider = {
                .name = "Brightness",
                .inital_val = &settings_app.brightness,
                .min_val = 1,
                .max_val = 100,
            }
        }
    },
    {
        .type = LV_SETTINGS_TYPE_SWITCH,
        .icon = LV_SYMBOL_TINT,
        .change_callback = on_display_on_changed,
        .item = {
            .sw = {
                .name = "Display always on",
                .inital_val = &settings_app.display_always_on
            }
        }
    },
    {
        .type = LV_SETTINGS_TYPE_BTN,
        .icon = LV_SYMBOL_IMAGE,
        .change_callback = on_restart_screen_changed,
        .item = {
            .btn = {
                .name = "Restart screen",
                .text = LV_SYMBOL_REFRESH
            }
        }
    },
};

static lv_settings_item_t bluetooth_page_items[] = {
    {
        .type = LV_SETTINGS_TYPE_BTN,
        .icon = LV_SYMBOL_BLUETOOTH,
        .change_callback = on_pairing_enable_changed,
        .item = {
            .btn = {
                .name = "Pairable",
                .text = "Enable"
            }
        }
    },
    {
        .type = LV_SETTINGS_TYPE_BTN,
        .icon = LV_SYMBOL_BACKSPACE,
        .change_callback = on_clear_bonded_changed,
        .item = {
            .btn = {
                .name = "Delete all bonded peers",
                .text = LV_SYMBOL_TRASH
            }
        }
    },
    {
        .type = LV_SETTINGS_TYPE_SWITCH,
        .icon = "",
        .change_callback = on_aoa_enable_changed,
        .item = {
            .sw = {
                .name = "AoA",
                .inital_val = &settings_app.ble_aoa_enabled
            }
        }
    },
    {
        .type = LV_SETTINGS_TYPE_SLIDER,
        .icon = LV_SYMBOL_SHUFFLE,
        .change_callback = on_aoa_interval_changed,
        .item = {
            .slider = {
                .name = "CTE Tx interval",
                .inital_val = &settings_app.ble_aoa_tx_interval,
                .min_val = 10,
                .max_val = 5000
            }
        }
    },
};

static lv_settings_item_t other_page_items[] = {
    {
        .type = LV_SETTINGS_TYPE_BTN,
        .icon = LV_SYMBOL_POWER,
        .change_callback = on_reboot_changed,
        .item = {
            .btn = {
                .name = "Reboot",
                .text = LV_SYMBOL_REFRESH
            }
        }
    },
    {
        .type = LV_SETTINGS_TYPE_BTN,
        .icon = LV_SYMBOL_BACKSPACE,
        .change_callback = on_clear_storage_changed,
        .item = {
            .btn = {
                .name = "Erase external flash.",
                .text = LV_SYMBOL_TRASH
            }
        }
    },
    {
        .type = LV_SETTINGS_TYPE_SWITCH,
        .icon = LV_SYMBOL_AUDIO,
        .change_callback = on_display_vib_press_changed,
        .item = {
            .sw = {
                .name = "Vibrate on click",
                .inital_val = &settings_app.vibration_on_click,
            }
        }
    },
    {
        .type = LV_SETTINGS_TYPE_BTN,
        .icon = LV_SYMBOL_REFRESH,
        .change_callback = on_reset_steps_changed,
        .item = {
            .btn = {
                .name = "Reset step counter",
                .text = LV_SYMBOL_REFRESH
            }
        }
    },
};

static lv_settings_item_t ui_page_items[] = {
    {
        .type = LV_SETTINGS_TYPE_SWITCH,
        .icon = LV_SYMBOL_VIDEO,
        .change_callback = on_watchface_animation_changed,
        .item = {
            .sw = {
                .name = "Enable animations on watchface.",
                .inital_val = &settings_app.watchface.animations_on,
            }
        }
    },
    {
        .type = LV_SETTINGS_TYPE_SWITCH,
        .icon = LV_SYMBOL_SETTINGS,
        .change_callback = on_watchface_tick_interval_changed,
        .item = {
            .sw = {
                .name = "Smooth watchface second hand",
                .inital_val = &settings_app.watchface.smooth_second_hand,
            }
        }
    },
};

static lv_settings_page_t settings_menu[] = {
    {
        .name = "Display",
        .num_items = ARRAY_SIZE(display_page_items),
        .items = display_page_items
    },
    {
        .name = "Bluetooth",
        .num_items = ARRAY_SIZE(bluetooth_page_items),
        .items = bluetooth_page_items
    },
    {
        .name = "UI",
        .num_items = ARRAY_SIZE(ui_page_items),
        .items = ui_page_items
    },
    {
        .name = "Other",
        .num_items = ARRAY_SIZE(other_page_items),
        .items = other_page_items
    },
};

static void ble_pairing_work_handler(struct k_work *work)
{
    LOG_DBG("Timer expired. Disable pairing");
    ble_comm_set_pairable(false);
}

static void settings_app_start(lv_obj_t *root, lv_group_t *group)
{
    lv_settings_create(root, settings_menu, ARRAY_SIZE(settings_menu), "N/A", group, on_close_settings);
}

static void settings_app_stop(void)
{
    settings_ui_remove();
}

static void on_close_settings(void)
{
    zsw_app_manager_app_close_request(&app);
}

static void on_brightness_changed(lv_setting_value_t value, bool final)
{
    settings_app.brightness = value.item.slider;
    zsw_display_control_set_brightness(settings_app.brightness);
    if (final) {
        settings_save_one(ZSW_SETTINGS_BRIGHTNESS, &settings_app.brightness, sizeof(settings_app.brightness));
    }
}

static void on_display_on_changed(lv_setting_value_t value, bool final)
{
    settings_app.display_always_on = value.item.sw;
    settings_save_one(ZSW_SETTINGS_DISPLAY_ALWAYS_ON, &settings_app.display_always_on,
                      sizeof(settings_app.display_always_on));
}

static void on_display_vib_press_changed(lv_setting_value_t value, bool final)
{
    settings_app.vibration_on_click = value.item.sw;
    settings_save_one(ZSW_SETTINGS_VIBRATE_ON_PRESS, &settings_app.vibration_on_click,
                      sizeof(settings_app.vibration_on_click));
}

static void on_aoa_enable_changed(lv_setting_value_t value, bool final)
{
    settings_app.ble_aoa_enabled = value.item.sw;
    bleAoaAdvertise(settings_app.ble_aoa_tx_interval, settings_app.ble_aoa_tx_interval, settings_app.ble_aoa_enabled);
    settings_save_one(ZSW_SETTINGS_BLE_AOA_EN, &settings_app.ble_aoa_enabled, sizeof(settings_app.ble_aoa_enabled));
}

static void on_aoa_interval_changed(lv_setting_value_t value, bool final)
{
    settings_app.ble_aoa_tx_interval = value.item.slider;
    if (final) {
        settings_save_one(ZSW_SETTINGS_BLE_AOA_INT, &settings_app.ble_aoa_tx_interval,
                          sizeof(settings_app.ble_aoa_tx_interval));
    }
}

static void on_pairing_enable_changed(lv_setting_value_t value, bool final)
{
    if (final) {
        ble_comm_set_pairable(true);

        LOG_DBG("Schedule new work");
        k_work_reschedule(&ble_pairing_dwork, K_MSEC(60 * 1000UL));
    } else {
        ble_comm_set_pairable(false);
    }
}

static void on_clear_bonded_changed(lv_setting_value_t value, bool final)
{
    if (final) {
        int err = bt_unpair(BT_ID_DEFAULT, NULL);
        if (err) {
            LOG_ERR("Cannot unpair for default ID");
            return;
        }
    }
}

static void on_reset_steps_changed(lv_setting_value_t value, bool final)
{
    if (final) {
        zsw_imu_reset_step_count();
    }
}

static void on_clear_storage_confirm(bool yes_pressed)
{
    if (yes_pressed) {
#ifdef CONFIG_SPI_FLASH_LOADER
        zsw_rtt_flash_loader_reboot_and_erase_flash();
#endif
    }
}

static void on_clear_storage_changed(lv_setting_value_t value, bool final)
{
    if (final) {
        zsw_popup_show("Erase all settings?",
                       "Are you sure?\nThis can take up to 300s, but probably less.\nThe watch will restart once done.",
                       on_clear_storage_confirm, 10, true);
    }
}

static void display_restart_work_handler(struct k_work *work)
{
    zsw_display_control_sleep_ctrl(false);
    zsw_display_control_pwr_ctrl(false);
    zsw_display_control_pwr_ctrl(true);
    zsw_display_control_sleep_ctrl(false);
    zsw_display_control_sleep_ctrl(true);
}

static void on_restart_screen_changed(lv_setting_value_t value, bool final)
{
    if (final) {
        // Display functions can not run from LVGL context so a context switch is needed.
        // Due to display_off function will wait for LVGL to finish rendering, so we will hang as we wait
        // for ourselves to finish.
        k_work_submit(&display_restart_work);
    }
}

static void on_watchface_animation_changed(lv_setting_value_t value, bool final)
{
    settings_app.watchface.animations_on = value.item.sw;
    settings_save_one(ZSW_SETTINGS_WATCHFACE, &settings_app.watchface, sizeof(settings_app.watchface));
}

static void on_watchface_tick_interval_changed(lv_setting_value_t value, bool final)
{
    settings_app.watchface.smooth_second_hand = value.item.sw;
    settings_save_one(ZSW_SETTINGS_WATCHFACE, &settings_app.watchface, sizeof(settings_app.watchface));
}

static void on_reboot_changed(lv_setting_value_t value, bool final)
{
    if (final) {
        sys_reboot(SYS_REBOOT_COLD);
    }
}

static int settings_load_cb(const char *name, size_t len,
                            settings_read_cb read_cb, void *cb_arg)
{
    const char *next;
    int rc;

    if (settings_name_steq(name, ZSW_SETTINGS_KEY_BRIGHTNESS, &next) && !next) {

        if (len != sizeof(settings_app.brightness)) {
            return -EINVAL;
        }

        rc = read_cb(cb_arg, &settings_app.brightness, sizeof(settings_app.brightness));
        zsw_display_control_set_brightness(settings_app.brightness);
        if (rc >= 0) {
            return 0;
        }
        return rc;
    }
    if (settings_name_steq(name, ZSW_SETTINGS_KEY_VIBRATION_ON_PRESS, &next) && !next) {
        if (len != sizeof(settings_app.vibration_on_click)) {
            return -EINVAL;
        }

        rc = read_cb(cb_arg, &settings_app.vibration_on_click, sizeof(settings_app.vibration_on_click));
        settings_app.vibration_on_click = settings_app.vibration_on_click;
        if (rc >= 0) {
            return 0;
        }
        return rc;
    }
    if (settings_name_steq(name, ZSW_SETTINGS_KEY_DISPLAY_ALWAYS_ON, &next) && !next) {
        if (len != sizeof(settings_app.display_always_on)) {
            return -EINVAL;
        }

        rc = read_cb(cb_arg, &settings_app.display_always_on, sizeof(settings_app.display_always_on));
        settings_app.display_always_on = settings_app.display_always_on;
        if (rc >= 0) {
            return 0;
        }
        return rc;
    }
    if (settings_name_steq(name, ZSW_SETTINGS_KEY_BLE_AOA_EN, &next) && !next) {
        if (len != sizeof(settings_app.ble_aoa_enabled)) {
            return -EINVAL;
        }

        rc = read_cb(cb_arg, &settings_app.ble_aoa_enabled, sizeof(settings_app.ble_aoa_enabled));
        settings_app.ble_aoa_enabled = settings_app.ble_aoa_enabled;
        if (rc >= 0) {
            return 0;
        }
        return rc;
    }
    if (settings_name_steq(name, ZSW_SETTINGS_KEY_BLE_AOA_INT, &next) && !next) {
        if (len != sizeof(settings_app.ble_aoa_tx_interval)) {
            return -EINVAL;
        }

        rc = read_cb(cb_arg, &settings_app.ble_aoa_tx_interval, sizeof(settings_app.ble_aoa_tx_interval));
        settings_app.ble_aoa_tx_interval = settings_app.ble_aoa_tx_interval;
        if (rc >= 0) {
            return 0;
        }
        return rc;
    }
    if (settings_name_steq(name, ZSW_SETTINGS_KEY_WATCHFACE, &next) && !next) {
        if (len != sizeof(settings_app.watchface)) {
            return -EINVAL;
        }

        rc = read_cb(cb_arg, &settings_app.watchface, sizeof(settings_app.watchface));
        if (rc >= 0) {
            return 0;
        }
        return rc;
    }

    return -ENOENT;
}

static int settings_app_add(void)
{
    zsw_app_manager_add_application(&app);
    memset(&settings_app, 0, sizeof(settings_app));

    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(settings_app_handler, ZSW_SETTINGS_PATH, NULL,
                               settings_load_cb, NULL, NULL);

SYS_INIT(settings_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);