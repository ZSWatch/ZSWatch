#include "accel_event.h"
#include <zephyr/zbus/zbus.h>

ZBUS_CHAN_DEFINE(accel_data_chan,
                 struct accel_event,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(watchface_accel_lis, power_manager_accel_lis, zds_app_accel_lis),
                 ZBUS_MSG_INIT()
                );