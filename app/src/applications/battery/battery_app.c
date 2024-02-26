#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>

#include "battery/battery.h"
#include "battery/battery_ui.h"
#include "events/battery_event.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(battery_app, LOG_LEVEL_WRN);

static void battery_app_start(lv_obj_t *root, lv_group_t *group);
static void battery_app_stop(void);

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);
static int get_num_samples(void);

typedef struct battery_sample_t {
    int mV;
    int64_t timestamp;
} battery_sample_t;

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_LISTENER_DEFINE(battery_app_battery_event, zbus_battery_sample_data_callback);

LV_IMG_DECLARE(battery_app_icon);

static application_t app = {
    .name = "Battery",
    .icon = &(battery_app_icon),
    .start_func = battery_app_start,
    .stop_func = battery_app_stop
};

static battery_sample_t battery_samples[CONFIG_DEFAULT_CONFIGURATION_BATTERY_NUM_SAMPLES_MAX];
static int next_battery_sample_index;

static void battery_app_start(lv_obj_t *root, lv_group_t *group)
{
    int rc;
    int batt_mv;

    rc = battery_measure_enable(true);
    if (rc != 0) {
        LOG_ERR("Failed initialize battery measurement: %d\n", rc);
    }

    batt_mv = battery_sample();

    rc = battery_measure_enable(false);
    if (rc != 0) {
        LOG_ERR("Failed disable battery measurement: %d\n", rc);
    }

    battery_ui_show(root, get_num_samples() + 1);
    battery_ui_set_current_measurement(batt_mv);
    for (int i = 0; i < CONFIG_DEFAULT_CONFIGURATION_BATTERY_NUM_SAMPLES_MAX; i++) {
        if (battery_samples[(next_battery_sample_index + i) % CONFIG_DEFAULT_CONFIGURATION_BATTERY_NUM_SAMPLES_MAX].timestamp !=
            0) {
            battery_ui_add_measurement(battery_samples[(next_battery_sample_index + i) %
                                                       CONFIG_DEFAULT_CONFIGURATION_BATTERY_NUM_SAMPLES_MAX].mV);
        }
    }
}

static void battery_app_stop(void)
{
    battery_ui_remove();
}

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event = zbus_chan_const_msg(chan);
    int previous_sample_index;

    if (next_battery_sample_index == 0) {
        previous_sample_index = CONFIG_DEFAULT_CONFIGURATION_BATTERY_NUM_SAMPLES_MAX - 1;
    } else {
        previous_sample_index = next_battery_sample_index - 1;
    }
    if (battery_samples[previous_sample_index].timestamp == 0 ||
        ((k_uptime_get() - battery_samples[previous_sample_index].timestamp) >=
         (CONFIG_DEFAULT_CONFIGURATION_BATTERY_SAMPLE_INTERVAL_MINUTES * 60 * 1000))) {
        battery_samples[next_battery_sample_index].mV = event->mV;
        battery_samples[next_battery_sample_index].timestamp = k_uptime_get();
        LOG_DBG("Add %d\n", battery_samples[next_battery_sample_index].mV);
        next_battery_sample_index = (next_battery_sample_index + 1) % CONFIG_DEFAULT_CONFIGURATION_BATTERY_NUM_SAMPLES_MAX;
    } else {
        LOG_DBG("Discard sample: %d, %d\n", battery_samples[previous_sample_index].mV,
                (int)(k_uptime_get() - battery_samples[previous_sample_index].timestamp));
    }
}

static int get_num_samples(void)
{
    int num_samples = 0;

    for (int i = 0; i < CONFIG_DEFAULT_CONFIGURATION_BATTERY_NUM_SAMPLES_MAX; i++) {
        if (battery_samples[i].timestamp != 0) {
            num_samples++;
        }
    }

    return num_samples;
}

static int battery_app_add(void)
{
    zsw_app_manager_add_application(&app);
    memset(battery_samples, 0, sizeof(battery_samples));
    return 0;
}

SYS_INIT(battery_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);