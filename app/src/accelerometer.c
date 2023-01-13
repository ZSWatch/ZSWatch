#include <accelerometer.h>
#include <zephyr/logging/log.h>
#include "lis2ds12_reg.h"
#include <events/accel_event.h>

LOG_MODULE_REGISTER(accel, LOG_LEVEL_DBG);

static void data_ready_xyz(const struct device *dev,
                           const struct sensor_trigger *trig);
static int configure_double_tap_detection(void);
static int configure_pedometer(void);
static int configure_tilt_detection(void);
static int configure_int2_to_int1(void);
static void send_accel_event(accelerometer_evt_t *data);

static const struct device *sensor;
static accel_event_cb accel_evt_cb;

int accelerometer_init(accel_event_cb cb)
{
    struct sensor_value odr;
    struct sensor_trigger trig;
    sensor = device_get_binding(DT_LABEL(DT_INST(0, st_lis2ds12)));
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

    if (err == 0) {
        err = configure_int2_to_int1(); // For now route INT2 to INT1 GPIO as it's already set up in LIS2DS12 trigger code.
        if (err < 0) {
            LOG_ERR("configure_int2_to_int1 failed: %d", err);
        }
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
    __ASSERT(device_is_ready(sensor), "Accelerometer not initialized correctly");
    stmdev_ctx_t *ctx = (stmdev_ctx_t *)sensor->config;
    int err = lis2ds12_number_of_steps_get(ctx, num_steps);
    if (err < 0) {
        LOG_ERR("lis2ds12_number_of_steps_get: %d", err);
        return err;
    }
    LOG_ERR("STEP COUNT: %d", *num_steps);
    return err;
}

int accelerometer_fetch_temperature(struct sensor_value *temperature)
{
    __ASSERT(device_is_ready(sensor), "Accelerometer not initialized correctly");
    stmdev_ctx_t *ctx = (stmdev_ctx_t *)sensor->config;
    int err = lis2ds12_temperature_raw_get(ctx, (uint8_t *)temperature);

    if (err < 0) {
        LOG_ERR("\nERROR: Unable to read temperature:%d\n", err);
    } else {
        LOG_DBG("Temp (TODO convert from 2 complement) %d\n", temperature->val1);
    }

    return err;
}

int accelerometer_reset_step_count(void)
{
    __ASSERT(device_is_ready(sensor), "Accelerometer not initialized correctly");
    stmdev_ctx_t *ctx = (stmdev_ctx_t *)sensor->config;

    int err = lis2ds12_pedo_step_reset_set(ctx, 1);
    if (err < 0) {
        LOG_ERR("lis2ds12_pedo_step_reset_set failed: %d", err);
    }
    return err;
}

static void data_ready_xyz(const struct device *dev, const struct sensor_trigger *trig)
{
    int err;
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t steps;
    accelerometer_evt_t evt;
    lis2ds12_all_sources_t isr_srcs;
    struct sensor_value acc_val[3];
    stmdev_ctx_t *ctx = (stmdev_ctx_t *)sensor->config;

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
}

static int configure_double_tap_detection(void)
{
    int err;
    lis2ds12_pin_int1_route_t route;
    __ASSERT(device_is_ready(sensor), "Accelerometer not initialized correctly");
    stmdev_ctx_t *ctx = (stmdev_ctx_t *)sensor->config;

    err = lis2ds12_tap_detection_on_z_set(ctx, 1);
    if (err < 0) {
        LOG_ERR("lis2ds12_tap_detection_on_z_set: %d", err);
        return err;
    }

    err = lis2ds12_tap_threshold_set(ctx, 0x0c); // Set tap threshold
    if (err < 0) {
        LOG_ERR("lis2ds12_tap_threshold_set: %d", err);
        return err;
    }

    err = lis2ds12_tap_shock_set(ctx, 0b11); // Set duration, quiet and shock time windows
    if (err < 0) {
        LOG_ERR("lis2ds12_tap_shock_set: %d", err);
        return err;
    }

    err = lis2ds12_tap_quiet_set(ctx, 0b11); // Set duration, quiet and shock time windows
    if (err < 0) {
        LOG_ERR("lis2ds12_tap_quiet_set: %d", err);
        return err;
    }

    err = lis2ds12_tap_dur_set(ctx, 0b0111); // Set duration, quiet and shock time windows
    if (err < 0) {
        LOG_ERR("lis2ds12_tap_dur_set: %d", err);
        return err;
    }

    err = lis2ds12_tap_mode_set(ctx, 1); // // Single & double-tap enabled (SINGLE_DOUBLE_TAP = 1)
    if (err < 0) {
        LOG_ERR("lis2ds12_tap_mode_set: %d", err);
        return err;
    }

    /* route double tap interrupt on int1 */
    err = lis2ds12_pin_int1_route_get(ctx, &route);
    if (err < 0) {
        return err;
    }

    route.int1_drdy = 0; // Default set to 1 when regging trigger, clear it here.
    route.int1_tap = 1;
    err = lis2ds12_pin_int1_route_set(ctx, route);
    if (err < 0) {
        LOG_ERR("lis2ds12_pin_int1_route_set: %d", err);
        return err;
    }

    return 0;
}

static int configure_pedometer(void)
{
    int err;
    lis2ds12_pin_int2_route_t route_int2;
    __ASSERT(device_is_ready(sensor), "Accelerometer not initialized correctly");
    stmdev_ctx_t *ctx = (stmdev_ctx_t *)sensor->config;

    err = lis2ds12_pedo_sens_set(ctx, 1); // Enable pedometer algorithm
    if (err < 0) {
        LOG_ERR("lis2ds12_pedo_sens_set: %d", err);
        return err;
    }

    err = lis2ds12_pin_int2_route_get(ctx, &route_int2);
    if (err < 0) {
        LOG_ERR("lis2ds12_pin_int2_route_get: %d", err);
        return err;
    }

    route_int2.int2_step_det = 1;
    err = lis2ds12_pin_int2_route_set(ctx, route_int2); // Step detector interrupt driven to INT2 pin
    if (err < 0) {
        LOG_ERR("lis2ds12_pin_int2_route_set: %d", err);
        return err;
    }

    return 0;
}

static int configure_tilt_detection(void)
{
    int err;
    lis2ds12_pin_int2_route_t route_int2;
    __ASSERT(device_is_ready(sensor), "Accelerometer not initialized correctly");
    stmdev_ctx_t *ctx = (stmdev_ctx_t *)sensor->config;

    err = lis2ds12_tilt_sens_set(ctx, 1); // Enable tilt algorithm
    if (err < 0) {
        LOG_ERR("lis2ds12_tilt_sens_set: %d", err);
        return err;
    }

    err = lis2ds12_pin_int2_route_get(ctx, &route_int2);
    if (err < 0) {
        LOG_ERR("lis2ds12_pin_int2_route_get: %d", err);
        return err;
    }

    route_int2.int2_tilt = 1;
    err = lis2ds12_pin_int2_route_set(ctx, route_int2); // Tilt detector interrupt driven to INT2 pin
    if (err < 0) {
        LOG_ERR("lis2ds12_pin_int2_route_set: %d", err);
        return err;
    }

    return 0;
}

static int configure_int2_to_int1(void)
{
    int err;
    __ASSERT(device_is_ready(sensor), "Accelerometer not initialized correctly");
    stmdev_ctx_t *ctx = (stmdev_ctx_t *)sensor->config;

    err = lis2ds12_all_on_int1_set(ctx, 1);
    if (err < 0) {
        LOG_ERR("lis2ds12_all_on_int1_set: %d", err);
        return err;
    }

    return 0;
}

static void send_accel_event(accelerometer_evt_t *data)
{
    struct accel_event *event = new_accel_event();

    memcpy(&event->data, data, sizeof(accelerometer_evt_t));
    APP_EVENT_SUBMIT(event);
}