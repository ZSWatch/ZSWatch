/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
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

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/settings/settings.h>

#include "managers/zsw_llext_iflash.h"

#ifdef CONFIG_ZSW_LLEXT_APPS
#include <zephyr/llext/symbol.h>
#include "zsw_llext_log.h"
#else
#include <zephyr/logging/log.h>
#endif

#include "history/zsw_history.h"
#include "battery_ui.h"
#include "events/battery_event.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
#include "fuel_gauge/zsw_pmic.h"
#endif

LOG_MODULE_REGISTER(pmic_app, LOG_LEVEL_WRN);

#define SETTING_BATTERY_HIST    "battery/hist"
#define SAMPLE_INTERVAL_MIN     15
#define SAMPLE_INTERVAL_MS      (SAMPLE_INTERVAL_MIN * 60 * 1000)
#define SAMPLE_INTERVAL_TICKS   k_ms_to_ticks_ceil64(SAMPLE_INTERVAL_MS)
#define MAX_SAMPLES             (7 * 24 * (60 / SAMPLE_INTERVAL_MIN)) // One week of 15 minute samples

/* Use z_impl_k_uptime_ticks() directly instead of k_uptime_get().
* k_uptime_get() is an inline wrapper that -fPIC can compile into
* an out-of-line GOT-indirect call to XIP .text. */
extern int64_t z_impl_k_uptime_ticks(void);

static void battery_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void battery_app_stop(void *user_data);
static void on_battery_hist_clear_cb(void);
static uint8_t compresse_voltage_in_byte(int mV);
static int decompress_voltage_from_byte(uint8_t voltage_byte);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);

static struct zbus_observer_data battery_app_obs_data = { .enabled = true };
static struct zbus_observer battery_app_battery_event;

static int64_t last_battery_sample_ticks;

typedef struct {
    uint8_t mv_with_decimals;
    uint8_t percent;
} zsw_battery_sample_t;

static zsw_battery_sample_t samples[MAX_SAMPLES];
static zsw_history_t battery_context;

static application_t app = {
    .name = "Battery",
    .icon = ZSW_LV_IMG_USE(battery_app_icon),
    .start_func = battery_app_start,
    .stop_func = battery_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

static void battery_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    zsw_battery_sample_t sample;
    struct battery_sample_event initial_sample;

#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
    battery_ui_show(root, on_battery_hist_clear_cb, zsw_history_samples(&battery_context) + 1, true);
#else
    battery_ui_show(root, on_battery_hist_clear_cb, zsw_history_samples(&battery_context) + 1, false);
#endif

    for (int i = 0; i < zsw_history_samples(&battery_context); i++) {
        zsw_history_get(&battery_context, &sample, i);
        battery_ui_add_measurement(sample.percent, decompress_voltage_from_byte(sample.mv_with_decimals));
    }

    if (zbus_chan_read(&battery_sample_data_chan, &initial_sample, K_MSEC(100)) == 0) {
#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
        battery_ui_update(initial_sample.ttf, initial_sample.tte,
                          zsw_pmic_charger_status_str(initial_sample.status),
                          zsw_pmic_charger_error_str(initial_sample.error),
                          initial_sample.is_charging);
#else
        battery_ui_update(initial_sample.ttf, initial_sample.tte,
                          "N/A",
                          "N/A",
                          initial_sample.is_charging);
#endif
        battery_ui_add_measurement(initial_sample.percent, initial_sample.mV);
    }
}

static void battery_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    battery_ui_remove();
}

LLEXT_IFLASH
static void zbus_battery_sample_data_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event =
        (const struct battery_sample_event *)chan->message;


    int64_t now_ticks = z_impl_k_uptime_ticks();

    if ((now_ticks - last_battery_sample_ticks) >= SAMPLE_INTERVAL_TICKS) {
        zsw_battery_sample_t sample;
        sample.mv_with_decimals = compresse_voltage_in_byte(event->mV);
        sample.percent = event->percent;

        zsw_history_add(&battery_context, &sample);
        if (zsw_history_save(&battery_context)) {
            LOG_ERR("Error during saving of battery samples!");
        }

        last_battery_sample_ticks = now_ticks;
    }

    if (app.current_state == ZSW_APP_STATE_UI_VISIBLE) {
        battery_ui_add_measurement(event->percent, event->mV);
#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
        battery_ui_update(event->ttf, event->tte,
                          zsw_pmic_charger_status_str(event->status),
                          zsw_pmic_charger_error_str(event->error),
                          event->is_charging);
#else
        battery_ui_update(event->ttf, event->tte, "N/A", "N/A", event->is_charging);
#endif
    }
}

static void on_battery_hist_clear_cb(void)
{
    zsw_history_del(&battery_context);
    if (settings_delete(SETTING_BATTERY_HIST) != 0) {
        LOG_ERR("Error during settings_delete!");
    }
}

LLEXT_IFLASH
static uint8_t compresse_voltage_in_byte(int mV)
{
    uint8_t voltage_byte = 0;

    // Only support 3V to 5V
    if (mV < 3000) {
        mV = 3000;
    } else if (mV >= 5000) {
        mV = 5000;
    }

    // Voltage is always above 3000 mV and below 5000 mV
    // Store first digit in the byte and the rest is decimals in 10mV resolution
    voltage_byte = (mV / 1000); // Gives a 3 or a 4
    voltage_byte -= 3; // Gives a 0 or 1 representing 3V or 4V
    voltage_byte *= 100; // Shifts the value to the left by 2 digits
    voltage_byte += (mV / 10) % 100; // Adds the decimals
    return voltage_byte;
}

static int decompress_voltage_from_byte(uint8_t voltage_byte)
{
    return (voltage_byte * 10) + 3000;
}

static int battery_app_add(void)
{
    zsw_app_manager_add_application(&app);

    if (settings_subsys_init()) {
        LOG_ERR("settings_subsys_init failed");
        return -EFAULT;
    }

    zsw_history_init(&battery_context, MAX_SAMPLES, sizeof(zsw_battery_sample_t), samples, SETTING_BATTERY_HIST);

    if (zsw_history_load(&battery_context)) {
        LOG_ERR("history load failed");
        return -EFAULT;
    }

    return 0;
}

#ifdef CONFIG_ZSW_LLEXT_APPS
application_t *app_entry(void)
{
    battery_app_battery_event.name = "battery_app_bat_lis";
    battery_app_battery_event.type = ZBUS_OBSERVER_LISTENER_TYPE;
    battery_app_battery_event.data = &battery_app_obs_data;
    battery_app_battery_event.callback = zsw_llext_create_trampoline((void *)zbus_battery_sample_data_callback);
    zbus_chan_add_obs(&battery_sample_data_chan, &battery_app_battery_event, K_MSEC(100));

    LLEXT_TRAMPOLINE_APP_FUNCS(&app);
    battery_app_add();
    return &app;
}
EXPORT_SYMBOL(app_entry);
#else
SYS_INIT(battery_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#endif
