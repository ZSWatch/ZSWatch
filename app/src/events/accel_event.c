#include "accel_event.h"
#include <zephyr/zbus/zbus.h>

ZBUS_CHAN_DEFINE(accel_data_chan,
    struct accel_event,
    NULL,
    NULL,
    ZBUS_OBSERVERS(main_accel_lis, watchface_accel_lis),
    ZBUS_MSG_INIT()
);