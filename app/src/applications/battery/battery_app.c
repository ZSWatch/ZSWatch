#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/settings/settings.h>

#include "history/zsw_history.h"
#include "battery/battery_ui.h"
#include "events/battery_event.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "fuel_gauge/zsw_pmic.h"
#include "battery_ui.h"

#define SETTING_BATTERY_HIST    "battery/hist"
#define SAMPLE_INTERVAL_MS      (CONFIG_APPLICATIONS_BATTERY_SAMPLE_INTERVAL_MINUTES * 60 * 1000)
#define MAX_SAMPLES             (7 * 24 * (60 / CONFIG_APPLICATIONS_BATTERY_SAMPLE_INTERVAL_MINUTES))

static void battery_app_start(lv_obj_t *root, lv_group_t *group);
static void battery_app_stop(void);

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);
static void on_battery_hist_clear_cb(void);

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_LISTENER_DEFINE(battery_app_battery_event, zbus_battery_sample_data_callback);
ZBUS_CHAN_ADD_OBS(battery_sample_data_chan, battery_app_battery_event, 1);

ZSW_LV_IMG_DECLARE(battery_app_icon);

LOG_MODULE_REGISTER(pmic_app, LOG_LEVEL_WRN);

typedef struct {
    uint8_t mv_with_decimals;
    uint8_t percent;
} zsw_battery_sample_t;

static zsw_battery_sample_t samples[MAX_SAMPLES];
static zsw_history_t battery_context;
static uint64_t last_battery_sample_time = 0;

static application_t app = {
    .name = "Battery",
    .icon = ZSW_LV_IMG_USE(battery_app_icon),
    .start_func = battery_app_start,
    .stop_func = battery_app_stop
};

static void battery_app_start(lv_obj_t *root, lv_group_t *group)
{
    zsw_battery_sample_t sample;
    struct battery_sample_event initial_sample;

#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
    battery_ui_show(root, on_battery_hist_clear_cb, zsw_history_samples(&battery_context) + 1, true);
#else
    battery_ui_show(root, on_battery_hist_clear_cb, zsw_history_samples(&battery_context) + 1, false);
#endif

    for (int i = 0; i < zsw_history_samples(&battery_context); i++) {
        zsw_history_get(&battery_context, &sample, i);
        battery_ui_add_measurement(sample.percent, (sample.mv_with_decimals * 10) + 2000);
    }

    if (zbus_chan_read(&battery_sample_data_chan, &initial_sample, K_MSEC(100)) == 0) {
        battery_ui_update(initial_sample.ttf, initial_sample.tte, initial_sample.status, initial_sample.error,
                          initial_sample.is_charging);
        battery_ui_add_measurement(initial_sample.percent, initial_sample.mV);
    }
}

static void battery_app_stop(void)
{
    battery_ui_remove();
}

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event = zbus_chan_const_msg(chan);

    if ((k_uptime_get() - last_battery_sample_time) >= SAMPLE_INTERVAL_MS) {
        zsw_battery_sample_t sample;
        sample.mv_with_decimals = ((event->mV - 2000) / 1000) * 100 + ((event->mV / 10) % 100);
        sample.percent = event->percent;

        if (zsw_history_save(&battery_context, &sample)) {
            LOG_ERR("Error during saving of battery samples!");
        }

        last_battery_sample_time = k_uptime_get();
        battery_ui_add_measurement(event->percent, event->mV);
    }
    battery_ui_update(event->ttf, event->tte, event->status, event->error, event->is_charging);
}

static void on_battery_hist_clear_cb(void)
{
    zsw_history_del(&battery_context);
    if (settings_delete(SETTING_BATTERY_HIST) != 0) {
        LOG_ERR("Error during settings_delete!");
    }
}

static int battery_app_add(void)
{
    zsw_app_manager_add_application(&app);

    if (settings_subsys_init()) {
        LOG_ERR("Error during settings_subsys_init!");
        return -EFAULT;
    }

    zsw_history_init(&battery_context, MAX_SAMPLES, sizeof(zsw_battery_sample_t), samples, SETTING_BATTERY_HIST);

    if (zsw_history_load(&battery_context)) {
        LOG_ERR("Error during settings_load_subtree!");
        return -EFAULT;
    }

    return 0;
}

SYS_INIT(battery_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);