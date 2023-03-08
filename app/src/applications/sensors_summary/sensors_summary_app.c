#include <sensors_summary/sensors_summary_ui.h>
#include <application_manager.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor/bmp581_user.h>
#include <math.h>

LOG_MODULE_REGISTER(sensors_summary_app, LOG_LEVEL_DBG);

#define SENSOR_REFRESH_INTERVAL_MS  100

// Functions needed for all applications
static void sensors_summary_app_start(lv_obj_t *root, lv_group_t *group);
static void sensors_summary_app_stop(void);
static void on_close_sensors_summary(void);
static void on_ref_set(void);

// Functions related to app functionality
static void timer_callback(lv_timer_t *timer);

LV_IMG_DECLARE(move);

static application_t app = {
    .name = "Sensors",
    .icon = &move,
    .start_func = sensors_summary_app_start,
    .stop_func = sensors_summary_app_stop
};

static const struct device *const bme680 = DEVICE_DT_GET_ONE(bosch_bme680);
static const struct device *const bmp581 = DEVICE_DT_GET_ONE(bosch_bmp581);

static lv_timer_t *refresh_timer;

static double relative_pressure;

static void sensors_summary_app_start(lv_obj_t *root, lv_group_t *group)
{
    if (!device_is_ready(bmp581)) {
		printk("Device %s is not ready\n", bmp581->name);
	}
    sensors_summary_ui_show(root, on_close_sensors_summary, on_ref_set);
    on_ref_set(); // Set inital relative pressure
    refresh_timer = lv_timer_create(timer_callback, SENSOR_REFRESH_INTERVAL_MS,  NULL);
}

static void sensors_summary_app_stop(void)
{
    lv_timer_del(refresh_timer);
    sensors_summary_ui_remove();
}

static double get_relative_height_m(double relative_pressure, double new_pressure, double temperature)
{
    return ((powf((relative_pressure / new_pressure), 1.f/5.257f) - 1.f) * (temperature + 273.15f)) / 0.0065f;
}

static void timer_callback(lv_timer_t *timer)
{
    struct sensor_value temp_sensor_val;
    double temp_bmp581;
    double temp_bme688;
    double pressure;
    double gas_res;
    double humidity;
    int rc;

    rc = sensor_sample_fetch(bmp581);
    if (rc != 0) {
        printf("bmp581 sensor_sample_fetch error: %d\n", rc);
        return;
    }

    rc = sensor_sample_fetch(bme680);
    if (rc != 0) {
        printf("bme680 sensor_sample_fetch error: %d\n", rc);
        return;
    }

    sensor_channel_get(bmp581, SENSOR_CHAN_PRESS, &temp_sensor_val);
    pressure = sensor_value_to_double(&temp_sensor_val);

    sensor_channel_get(bmp581, SENSOR_CHAN_AMBIENT_TEMP, &temp_sensor_val);
    temp_bmp581 = sensor_value_to_double(&temp_sensor_val);

    sensor_channel_get(bme680, SENSOR_CHAN_AMBIENT_TEMP, &temp_sensor_val);
    temp_bme688 = sensor_value_to_double(&temp_sensor_val);

    sensor_channel_get(bme680, SENSOR_CHAN_GAS_RES, &temp_sensor_val);
    gas_res = sensor_value_to_double(&temp_sensor_val);

    sensor_channel_get(bme680, SENSOR_CHAN_HUMIDITY, &temp_sensor_val);
    humidity = sensor_value_to_double(&temp_sensor_val);

    sensors_summary_ui_set_pressure(pressure);
    sensors_summary_ui_set_temp(temp_bmp581);
    sensors_summary_ui_set_gas(gas_res);
    sensors_summary_ui_set_humidity(humidity);
    sensors_summary_ui_set_rel_height(get_relative_height_m(relative_pressure, pressure, temp_bmp581));
}

static int init_pressure_sensor(void)
{
    struct sensor_value odr;
    if (!device_is_ready(bmp581)) {
        LOG_ERR("Error: pressure sensor not detected");
        return -ENODEV;
    }

    odr.val1 = BMP5_POWERMODE_NORMAL;
    odr.val2 = 0;
    int err = sensor_attr_set(bmp581, SENSOR_CHAN_ALL, BMP5_ATTR_POWER_MODE, &odr);
    if (err) {
        LOG_ERR("Failed setting pressure ODR");
    }

    odr.val1 = 240;
    odr.val2 = 1;
    err = sensor_attr_set(bmp581, SENSOR_CHAN_ALL, SENSOR_ATTR_SAMPLING_FREQUENCY, &odr);
    if (err) {
        LOG_ERR("Failed setting pressure ODR");
    }
    odr.val1 = BMP5_OVERSAMPLING_2X;
    odr.val2 = BMP5_OVERSAMPLING_2X;
    err = sensor_attr_set(bmp581, SENSOR_CHAN_ALL, SENSOR_ATTR_OVERSAMPLING, &odr);
    if (err) {
        LOG_ERR("Failed setting pressure ODR");
    }

    odr.val1 = BMP5_IIR_FILTER_COEFF_1;
    odr.val2 = BMP5_IIR_FILTER_COEFF_1;
    err = sensor_attr_set(bmp581, SENSOR_CHAN_ALL, BMP5_ATTR_IIR_CONFIG, &odr);
    if (err) {
        LOG_ERR("Failed setting pressure ODR");
    }
    return 0;
}

static void on_close_sensors_summary(void)
{
    application_manager_app_close_request(&app);
}

static void on_ref_set(void)
{
    struct sensor_value temp_sensor_val;

    sensor_channel_get(bmp581, SENSOR_CHAN_PRESS, &temp_sensor_val);
    relative_pressure = sensor_value_to_double(&temp_sensor_val);
}

static int sensors_summary_app_add(const struct device *arg)
{
    application_manager_add_application(&app);
    init_pressure_sensor();
    return 0;
}

SYS_INIT(sensors_summary_app_add, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
