#include <accelerometer.h>
#include <zephyr/logging/log.h>
#include <events/accel_event.h>

LOG_MODULE_REGISTER(accel, LOG_LEVEL_DBG);

static void data_ready_xyz(const struct device *dev,
                           const struct sensor_trigger *trig);
static int configure_double_tap_detection(void);
static int configure_pedometer(void);
static int configure_tilt_detection(void);
static void send_accel_event(accelerometer_evt_t *data);

static const struct device *sensor;
static accel_event_cb accel_evt_cb;

int accelerometer_init(accel_event_cb cb)
{
    struct sensor_value odr;
    struct sensor_trigger trig;
    sensor = DEVICE_DT_GET_ONE(bosch_bmi270);
    if (!device_is_ready(sensor)) {
        LOG_ERR("Error: Device \"%s\" is not ready; "
                "check the driver initialization logs for errors.",
                sensor->name);
        return -ENODEV;
    }

    // 400Hz needed for double tap detection
    odr.val1 = 400;
    odr.val2 = 0;
    int err = sensor_attr_set(sensor, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr);
    if (!err) {
        trig.type = SENSOR_TRIG_DATA_READY;
        trig.chan = SENSOR_CHAN_ALL;
        err = sensor_trigger_set(sensor, &trig, data_ready_xyz);
        if (err == 0) {
            accel_evt_cb = cb;
            err = configure_double_tap_detection();
            if (err < 0) {
                LOG_ERR("configure_double_tap_detection failed: %d", err);
            }
            err = configure_pedometer();
            if (err < 0) {
                LOG_ERR("configure_pedometer failed: %d", err);
            }
            err = configure_tilt_detection();
            if (err < 0) {
                LOG_ERR("configure_tilt_detection failed: %d", err);
            }
        }
    } else {
        LOG_ERR("sensor_attr_set fail: %d", err);
    }

    return err;
}

int accelerometer_fetch_xyz(int16_t *x, int16_t *y, int16_t *z)
{
    __ASSERT(device_is_ready(sensor), "Accelerometer not initialized correctly");
    int err = sensor_sample_fetch(sensor);
    if (err) {
        LOG_ERR("Could not fetch sample from %s", sensor->name);
    }
    struct sensor_value acc_val[3];
    if (!err) {
        err = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, acc_val);
        if (err < 0) {
            LOG_ERR("\nERROR: Unable to read accel XYZ:%d\n", err);
        } else {
            *x = (int16_t)(sensor_value_to_double(&acc_val[0]) * (32768 / 16));
            *y = (int16_t)(sensor_value_to_double(&acc_val[1]) * (32768 / 16));
            *z = (int16_t)(sensor_value_to_double(&acc_val[2]) * (32768 / 16));
        }
    }

    return err;
}

int accelerometer_fetch_num_steps(int16_t *num_steps)
{
    return -ENOENT;
}

int accelerometer_fetch_temperature(struct sensor_value *temperature)
{
    return -ENOENT;
}

int accelerometer_reset_step_count(void)
{
    return -ENOENT;
}

static void data_ready_xyz(const struct device *dev, const struct sensor_trigger *trig)
{
    /*
    int err;
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t steps;
    accelerometer_evt_t evt;
    struct sensor_value acc_val[3];

    __ASSERT(device_is_ready(sensor), "Accelerometer not initialized correctly");

    err = lis2ds12_all_sources_get(ctx, &isr_srcs);
    if (isr_srcs.tap_src.double_tap) {
        LOG_DBG("Double TAP ISR");
        evt.type = ACCELEROMETER_EVT_TYPE_DOOUBLE_TAP;
        if (accel_evt_cb) {
            accel_evt_cb(&evt);
        }
        send_accel_event(&evt);
    } else if (isr_srcs.tap_src.single_tap) {
        LOG_DBG("Single TAP ISR");
    } else if (isr_srcs.func_ck_gate.step_detect) {
        accelerometer_fetch_num_steps(&steps);
        LOG_DBG("Step Detect: %d", steps);
        evt.type = ACCELEROMETER_EVT_TYPE_STEP;
        evt.data.step.count = steps;
        if (accel_evt_cb) {
            accel_evt_cb(&evt);
        }
        send_accel_event(&evt);
    } else if (isr_srcs.func_ck_gate.tilt_int) {
        LOG_DBG("Tilt Detected");
        evt.type = ACCELEROMETER_EVT_TYPE_TILT;
        if (accel_evt_cb) {
            accel_evt_cb(&evt);
        }
        send_accel_event(&evt);
    } else if (isr_srcs.status_dup.drdy) {
        LOG_DBG("DRDY ISR");
        err = sensor_sample_fetch_chan(dev, SENSOR_CHAN_ACCEL_XYZ);
        if (!err) {
            err = sensor_channel_get(sensor, SENSOR_CHAN_ACCEL_XYZ, acc_val);
            if (err < 0) {
                LOG_DBG("\nERROR: Unable to read accel XYZ:%d\n", err);
            } else {
                x = (int16_t)(sensor_value_to_double(&acc_val[0]) * (32768 / 16));
                y = (int16_t)(sensor_value_to_double(&acc_val[1]) * (32768 / 16));
                z = (int16_t)(sensor_value_to_double(&acc_val[2]) * (32768 / 16));
                evt.type = ACCELEROMETER_EVT_TYPE_XYZ;
                evt.data.xyz.x = x;
                evt.data.xyz.y = y;
                evt.data.xyz.z = z;
                if (accel_evt_cb) {
                    accel_evt_cb(&evt);
                }
                send_accel_event(&evt);
            }
        }
    } else {
        LOG_WRN("Unknown ISR");
    }

    */

}

static int configure_double_tap_detection(void)
{
    return 0;
}

static int configure_pedometer(void)
{
    return 0;
}

static int configure_tilt_detection(void)
{
    return 0;
}

static void send_accel_event(accelerometer_evt_t *data)
{
    struct accel_event *event = new_accel_event();

    memcpy(&event->data, data, sizeof(accelerometer_evt_t));
    APP_EVENT_SUBMIT(event);
}