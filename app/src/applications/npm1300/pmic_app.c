#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/settings/settings.h>

#include "battery/battery.h"
#include "battery/battery_ui.h"
#include "events/battery_event.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"
#include "fuel_gauge/zsw_pmic.h"
#include "pmic_ui.h"

LOG_MODULE_REGISTER(pmic_app, LOG_LEVEL_WRN);

#define SETTING_BATTERY_HIST "battery/hist"

#define MAX_SAMPLES (7 * 24 * 4) // 7 days of 15 minute samples
#define SAMPLE_INTERVAL (15 * 60 * 1000) // 15 minutes in milliseconds

static void pmic_app_start(lv_obj_t *root, lv_group_t *group);
static void pmic_app_stop(void);

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan);
static void on_battery_hist_clear_cb(void);

typedef struct battery_sample_t {
    uint8_t mv_with_decimals; // Calced by: 3.65V - 2 => 1.65 => 165
    uint8_t percent;
} battery_sample_t;

typedef struct battery_storage_t {
    uint16_t first_sample_index;
    uint16_t num_samples;
    battery_sample_t samples[MAX_SAMPLES];
} battery_storage_t;

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_LISTENER_DEFINE(pmic_app_battery_event, zbus_battery_sample_data_callback);
ZBUS_CHAN_ADD_OBS(battery_sample_data_chan, pmic_app_battery_event, 1);

LV_IMG_DECLARE(battery_app_icon);

static battery_storage_t battery_context = {0};
static uint64_t last_battery_sample_time = 0;

static application_t app = {
    .name = "nPM1300",
    .icon = &(battery_app_icon),
    .start_func = pmic_app_start,
    .stop_func = pmic_app_stop
};

static void pmic_app_start(lv_obj_t *root, lv_group_t *group)
{
    battery_sample_t *sample;
    struct battery_sample_event initial_sample;
    pmic_ui_show(root, on_battery_hist_clear_cb, battery_context.num_samples + 1);

    for (int i = 0; i < battery_context.num_samples; i++) {
        sample = &battery_context.samples[(battery_context.first_sample_index + i) % MAX_SAMPLES];
        pmic_ui_add_measurement(sample->percent, (sample->mv_with_decimals * 10) + 2000);
    }

    if (zbus_chan_read(&battery_sample_data_chan, &initial_sample, K_MSEC(100)) == 0) {
        LOG_WRN("Success");
        pmic_ui_update(initial_sample.tte, initial_sample.ttf, initial_sample.status, initial_sample.error,
                       initial_sample.is_charging);
        pmic_ui_add_measurement(initial_sample.percent, initial_sample.mV);
    }
}

static void pmic_app_stop(void)
{
    pmic_ui_remove();
}

static void add_battery_sample(const struct battery_sample_event *event)
{
    int next_battery_sample_index = (battery_context.first_sample_index + battery_context.num_samples) % MAX_SAMPLES;
    battery_context.samples[next_battery_sample_index].mv_with_decimals = ((event->mV - 2000) / 1000) * 100 + ((
                                                                              event->mV / 10) % 100);
    battery_context.samples[next_battery_sample_index].percent = event->percent;

    battery_context.num_samples++;
    if (battery_context.num_samples > MAX_SAMPLES) {
        battery_context.first_sample_index = (battery_context.first_sample_index + 1) % MAX_SAMPLES;
        battery_context.num_samples = MAX_SAMPLES;
    }
    LOG_WRN("%d, %d, %d", battery_context.first_sample_index, battery_context.num_samples, next_battery_sample_index);
}

static void zbus_battery_sample_data_callback(const struct zbus_channel *chan)
{
    const struct battery_sample_event *event = zbus_chan_const_msg(chan);

    if ((k_uptime_get() - last_battery_sample_time) >= SAMPLE_INTERVAL) {
        add_battery_sample(event);
        int ret = settings_save_one(SETTING_BATTERY_HIST, &battery_context, sizeof(battery_storage_t));
        if (ret) {
            LOG_ERR("Error during saving of battery_context.samples %d", ret);
        }
        last_battery_sample_time = k_uptime_get();
        pmic_ui_add_measurement(event->percent, event->mV);
    }
    pmic_ui_update(event->tte, event->ttf, event->status, event->error, event->is_charging);
}

static void on_battery_hist_clear_cb(void)
{
    memset(battery_context.samples, 0, sizeof(battery_context.samples));
    if (settings_delete(SETTING_BATTERY_HIST) != 0) {
        LOG_ERR("Error during settings_delete!");
    }
}

static int battery_load_state(const char *p_key, size_t len,
                              settings_read_cb read_cb, void *p_cb_arg, void *p_param)
{
    ARG_UNUSED(p_key);

    if (len != sizeof(battery_storage_t)) {
        return -EINVAL;
    }

    int num_bytes_read = read_cb(p_cb_arg, &battery_context, len);
    LOG_WRN("Read %d bytes", num_bytes_read);

    LOG_ERR("Number samples: %d", battery_context.num_samples);

    if (num_bytes_read == 0) {
        LOG_ERR("Currupt battery settings data");
    } else if (num_bytes_read != sizeof(battery_storage_t)) {
        LOG_WRN("No battery settings data read");
        return -ENODATA;
    }

    LOG_DBG("Loaded battery history data");

    return 0;
}

static int pmic_app_add(void)
{
    zsw_app_manager_add_application(&app);

    LOG_WRN("Number samples B4: %d, %d", battery_context.first_sample_index, battery_context.num_samples);

    if (settings_subsys_init()) {
        LOG_ERR("Error during settings_subsys_init!");
        return -EFAULT;
    }

    if (settings_load_subtree_direct(SETTING_BATTERY_HIST, battery_load_state, NULL)) {
        LOG_ERR("Error during settings_load_subtree!");
        return -EFAULT;
    }

    if (settings_delete(SETTING_BATTERY_HIST) != 0) {
        LOG_ERR("Error during settings_delete!");
    }

    LOG_WRN("Number samples After: %d, %d", battery_context.first_sample_index, battery_context.num_samples);
    return 0;
}

SYS_INIT(pmic_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);