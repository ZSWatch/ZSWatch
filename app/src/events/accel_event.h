#include <app_event_manager.h>
#include <accelerometer.h>

struct accel_event {
    struct app_event_header header;
    accelerometer_evt_t data;
};

APP_EVENT_TYPE_DECLARE(accel_event);