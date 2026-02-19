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

/*
 * LLEXT version of the full Battery app.
 *
 * Battery app logic and UI (battery_ui.c) are compiled into this LLEXT module
 * as separate source files linked into a single shared library.
 *
 * Differences from the original built-in version:
 *   - Zbus observer registered at runtime (not compile-time)
 *   - Icon image compiled into .rodata → lives in XIP flash
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/settings/settings.h>
#include <lvgl.h>

#include "managers/zsw_app_manager.h"
#include "events/battery_event.h"
#include "ui/utils/zsw_ui_utils.h"
#include "history/zsw_history.h"

#include "managers/zsw_llext_iflash.h"

#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
#include "fuel_gauge/zsw_pmic.h"
#endif

#define SETTING_BATTERY_HIST    "battery/hist"
#define SAMPLE_INTERVAL_MIN     15
#define SAMPLE_INTERVAL_MS      (SAMPLE_INTERVAL_MIN * 60 * 1000)
#define SAMPLE_INTERVAL_TICKS   ((int64_t)SAMPLE_INTERVAL_MS * CONFIG_SYS_CLOCK_TICKS_PER_SEC / 1000)
#define MAX_SAMPLES             (7 * 24 * (60 / SAMPLE_INTERVAL_MIN))

typedef struct {
    uint8_t mv_with_decimals;
    uint8_t percent;
} zsw_battery_sample_t;

static zsw_battery_sample_t samples[MAX_SAMPLES];
static zsw_history_t battery_context;
static int64_t last_battery_sample_ticks;

/* ---- Image data compiled into .rodata (goes to XIP alongside code) ---- */
#include "battery_app_icon.c"

/* ---- Full battery UI (all 3 pages: chart, charger info, regulator info) ---- */
#include "battery_ui.h"

/* ---------- Forward declarations ---------- */
static void battery_app_start(lv_obj_t *root, lv_group_t *group, void *user_data);
static void battery_app_stop(void *user_data);
static void zbus_battery_callback(const struct zbus_channel *chan);
static void on_battery_hist_clear_cb(void);
static int decompress_voltage_from_byte(uint8_t voltage_byte);

/* ---------- Zbus: runtime observer (replaces compile-time ZBUS_CHAN_ADD_OBS) ---------- */
ZBUS_CHAN_DECLARE(battery_sample_data_chan);

static struct zbus_observer_data battery_real_ext_obs_data = {
    .enabled = true,
};

static struct zbus_observer battery_real_ext_listener = {
#if defined(CONFIG_ZBUS_OBSERVER_NAME)
    .name = "bat_real_lis",
#endif
    .type = ZBUS_OBSERVER_LISTENER_TYPE,
    .data = &battery_real_ext_obs_data,
    .callback = zbus_battery_callback,
};

/* ---------- App registration ---------- */
static application_t app = {
    .name = "Battery",
    .icon = &battery_app_icon,
    .start_func = battery_app_start,
    .stop_func = battery_app_stop,
    .category = ZSW_APP_CATEGORY_TOOLS,
};

/* ---------- App lifecycle ---------- */

static void battery_app_start(lv_obj_t *root, lv_group_t *group, void *user_data)
{
    ARG_UNUSED(user_data);
    LV_UNUSED(group);
    struct battery_sample_event initial_sample;
    zsw_battery_sample_t sample;

#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
    battery_ui_show(root, on_battery_hist_clear_cb, zsw_history_samples(&battery_context) + 1, true);
#else
    battery_ui_show(root, on_battery_hist_clear_cb, zsw_history_samples(&battery_context) + 1, false);
#endif

    for (int i = 0; i < zsw_history_samples(&battery_context); i++) {
        zsw_history_get(&battery_context, &sample, i);
        battery_ui_add_measurement(sample.percent, decompress_voltage_from_byte(sample.mv_with_decimals));
    }

    /* Read the latest battery sample and display it */
    if (zbus_chan_read(&battery_sample_data_chan, &initial_sample, K_MSEC(100)) == 0) {
#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
        battery_ui_update(initial_sample.ttf, initial_sample.tte,
                          zsw_pmic_charger_status_str(initial_sample.status),
                          zsw_pmic_charger_error_str(initial_sample.error),
                          initial_sample.is_charging);
#else
        battery_ui_update(initial_sample.ttf, initial_sample.tte,
                          "N/A", "N/A",
                          initial_sample.is_charging);
#endif
        battery_ui_add_measurement(initial_sample.percent, initial_sample.mV);
    }
}

static void battery_app_stop(void *user_data)
{
    ARG_UNUSED(user_data);
    printk("battery_real_ext: stop\n");
    battery_ui_remove();
}

/* ---------- Background: zbus battery listener ---------- */

LLEXT_IFLASH
static void zbus_battery_callback(const struct zbus_channel *chan)
{
    /* Use direct member access instead of zbus_chan_const_msg() to avoid
     * a GOT-routed function call.  LLEXT_IFLASH functions must not call
     * anything in XIP .text; zbus_chan_const_msg() is static-inline in the
     * header but -fPIC can emit an out-of-line copy in .text. */
    const struct battery_sample_event *event =
        (const struct battery_sample_event *)chan->message;

    /* History sampling (runs from IFLASH, calls firmware exports only).
     * NOTE: k_uptime_get() is an inline wrapper compiled into this module's
     * XIP .text, which is unreachable from IFLASH when XIP is off.
     * Use z_impl_k_uptime_ticks() directly (resolved via GOT to firmware). */
    extern int64_t z_impl_k_uptime_ticks(void);
    int64_t now_ticks = z_impl_k_uptime_ticks();
    if ((now_ticks - last_battery_sample_ticks) >= SAMPLE_INTERVAL_TICKS) {
        zsw_battery_sample_t sample;
        int mV = event->mV;
        if (mV < 3000) {
            mV = 3000;
        } else if (mV >= 5000) {
            mV = 5000;
        }
        uint8_t voltage_byte = (mV / 1000);
        voltage_byte -= 3;
        voltage_byte *= 100;
        voltage_byte += (mV / 10) % 100;
        sample.mv_with_decimals = voltage_byte;
        sample.percent = event->percent;

        zsw_history_add(&battery_context, &sample);
        zsw_history_save(&battery_context);

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
        battery_ui_update(event->ttf, event->tte,
                          "N/A", "N/A",
                          event->is_charging);
#endif
    }
}

/* ---------- History clear callback ---------- */

static void on_battery_hist_clear_cb(void)
{
    zsw_history_del(&battery_context);
    settings_delete(SETTING_BATTERY_HIST);
    printk("battery_real_ext: history cleared\n");
}

/* ---------- Voltage compression helpers ---------- */

static int decompress_voltage_from_byte(uint8_t voltage_byte)
{
    return (voltage_byte * 10) + 3000;
}

/* ---------- Entry point ---------- */

application_t *app_entry(void)
{
    printk("battery_real_ext: app_entry called\n");

    /* Initialize settings subsystem and load battery history */
    if (settings_subsys_init()) {
        printk("battery_real_ext: settings_subsys_init failed\n");
    }
    zsw_history_init(&battery_context, MAX_SAMPLES, sizeof(zsw_battery_sample_t), samples, SETTING_BATTERY_HIST);
    if (zsw_history_load(&battery_context)) {
        printk("battery_real_ext: history load failed\n");
    }
    printk("battery_real_ext: loaded %d history samples\n", zsw_history_samples(&battery_context));

    /* Runtime zbus registration */
    int ret = zbus_chan_add_obs(&battery_sample_data_chan,
                                &battery_real_ext_listener, K_MSEC(100));
    if (ret != 0) {
        printk("battery_real_ext: failed to add zbus observer: %d\n", ret);
    } else {
        printk("battery_real_ext: zbus observer registered OK\n");
    }

    return &app;
}
EXPORT_SYMBOL(app_entry);
