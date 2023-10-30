#include "accel_event.h"
#include <zephyr/zbus/zbus.h>

ZBUS_CHAN_DEFINE(accel_data_chan,
                 struct accel_event,
                 NULL,
                 NULL,
#ifdef CONFIG_APPLICATIONS_USE_ACCELEROMETER
                 ZBUS_OBSERVERS(watchface_accel_lis, power_manager_accel_lis),
#else
                 ZBUS_OBSERVERS(watchface_accel_lis, power_manager_accel_lis),
#endif
                 ZBUS_MSG_INIT()
                );