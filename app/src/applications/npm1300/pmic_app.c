#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/init.h>
#include <zephyr/zbus/zbus.h>

#include "battery/battery.h"
#include "battery/battery_ui.h"
#include "events/battery_event.h"
#include "managers/zsw_app_manager.h"
#include "ui/utils/zsw_ui_utils.h"

LOG_MODULE_REGISTER(pmic_app, LOG_LEVEL_WRN);

static void pmic_app_start(lv_obj_t *root, lv_group_t *group);
static void pmic_app_stop(void);

typedef struct battery_sample_t {
    int mV;
    int64_t timestamp;
} battery_sample_t;

LV_IMG_DECLARE(battery_app_icon);

static application_t app = {
    .name = "nPM1300",
    .icon = &(battery_app_icon),
    .start_func = pmic_app_start,
    .stop_func = pmic_app_stop
};

static void pmic_app_start(lv_obj_t *root, lv_group_t *group)
{
    pmic_ui_show(root);
}

static void pmic_app_stop(void)
{
}

static int pmic_app_add(void)
{
    zsw_app_manager_add_application(&app);
    return 0;
}

SYS_INIT(pmic_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);