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
 * Both the app logic (battery_app) and the complete multi-page UI
 * (battery_ui) are compiled into this single LLEXT module.
 *
 * Differences from the built-in version:
 *   - No history persistence (no settings / zsw_history)
 *   - Zbus observer registered at runtime (not compile-time)
 *   - Icon image compiled into .rodata → lives in XIP flash
 *   - battery_ui.c is #included so everything is one compilation unit
 */

#include <zephyr/kernel.h>
#include <zephyr/llext/symbol.h>
#include <zephyr/sys/printk.h>
#include <zephyr/zbus/zbus.h>
#include <lvgl.h>

#include "managers/zsw_app_manager.h"
#include "events/battery_event.h"
#include "ui/utils/zsw_ui_utils.h"

#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
#include "fuel_gauge/zsw_pmic.h"
#endif

/* ---- Image data compiled into .rodata (goes to XIP alongside code) ---- */
#include "images/battery_app_icon.c"

/* ---- Full battery UI (all 3 pages: chart, charger info, regulator info) ----
 * We #include the .c file because add_llext_target only supports a single
 * source file for ELF object builds.  The preprocessor merges everything
 * into one translation unit.
 */
#include "applications/battery/battery_ui.c"

/* ---------- Forward declarations ---------- */
static void battery_app_start(lv_obj_t *root, lv_group_t *group);
static void battery_app_stop(void);
static void zbus_battery_callback(const struct zbus_channel *chan);
static void on_battery_hist_clear_cb(void);

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

static void battery_app_start(lv_obj_t *root, lv_group_t *group)
{
    LV_UNUSED(group);
    struct battery_sample_event initial_sample;

    printk("battery_real_ext: start\n");

#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
    battery_ui_show(root, on_battery_hist_clear_cb, 1, true);
#else
    battery_ui_show(root, on_battery_hist_clear_cb, 1, false);
#endif

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

static void battery_app_stop(void)
{
    printk("battery_real_ext: stop\n");
    battery_ui_remove();
}

/* ---------- Background: zbus battery listener ---------- */

static void zbus_battery_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event = zbus_chan_const_msg(chan);

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

/* ---------- History clear callback (no-op: LLEXT has no persistent history) ---------- */

static void on_battery_hist_clear_cb(void)
{
    printk("battery_real_ext: history clear requested (no-op in LLEXT)\n");
}

/* ---------- Entry point ---------- */

application_t *app_entry(void)
{
    printk("battery_real_ext: app_entry called\n");

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
