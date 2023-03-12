#include <accelerometer.h>
#include <bmi270_port.h>
#include <bmi270.h>
#include <zephyr/logging/log.h>
#include <events/accel_event.h>
#include <zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(accel, LOG_LEVEL_DBG);

ZBUS_CHAN_DECLARE(accel_data_chan);

static int8_t set_accel_gyro_config(struct bmi2_dev *bmi2_dev);

static void send_accel_event(accelerometer_evt_t *data);

static struct bmi2_dev bmi2_dev;

/*! Earth's gravity in m/s^2 */
#define GRAVITY_EARTH  (9.80665f)

/*! Macros to select the sensors                   */
#define ACCEL          UINT8_C(0x00)
#define GYRO           UINT8_C(0x01)

int accelerometer_init(void)
{
    int8_t rslt;
    
    /* Assign accel and gyro sensor to variable. */
    uint8_t sensor_list[2] = { BMI2_ACCEL, BMI2_GYRO };

    rslt = bmi2_interface_init(&bmi2_dev, BMI2_I2C_INTF);
    bmi2_error_codes_print_result(rslt);

    /* Initialize bmi270. */
    rslt = bmi270_init(&bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK) {
        /* Accel and gyro configuration settings. */
        rslt = set_accel_gyro_config(&bmi2_dev);
        bmi2_error_codes_print_result(rslt);

        if (rslt == BMI2_OK) {
            /* NOTE:
             * Accel and Gyro enable must be done after setting configurations
             */
            rslt = bmi270_sensor_enable(sensor_list, 2, &bmi2_dev);
            bmi2_error_codes_print_result(rslt);
        }
    }

    return 0;
}

int accelerometer_fetch_xyz(int16_t *x, int16_t *y, int16_t *z)
{
    int8_t rslt;
    struct bmi2_sens_data sensor_data = { { 0 } };

    /* Get accel and gyro data for x, y and z axis. */
    rslt = bmi2_get_sensor_data(&sensor_data, &bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK) {
        *x = sensor_data.acc.x;
        *y = sensor_data.acc.y;
        *z = sensor_data.acc.z;
    }

    return rslt == BMI2_OK ? 0 : -EIO;
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

/*
static void data_ready_xyz(const struct device *dev, const struct sensor_trigger *trig)
{
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

}
*/

static void send_accel_event(accelerometer_evt_t *data)
{
    struct accel_event evt;
    memcpy(&evt.data, data, sizeof(accelerometer_evt_t));
    zbus_chan_pub(&accel_data_chan, &evt, K_MSEC(250));
}

/*!
 * @brief This internal API is used to set configurations for accel and gyro.
 */
static int8_t set_accel_gyro_config(struct bmi2_dev *bmi2_dev)
{
    /* Status of api are returned to this variable. */
    int8_t rslt;

    /* Structure to define accelerometer and gyro configuration. */
    struct bmi2_sens_config config[2];

    /* Configure the type of feature. */
    config[ACCEL].type = BMI2_ACCEL;
    config[GYRO].type = BMI2_GYRO;

    /* Get default configurations for the type of feature selected. */
    rslt = bmi270_get_sensor_config(config, 2, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    /* Map data ready interrupt to interrupt pin. */
    rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, bmi2_dev);
    bmi2_error_codes_print_result(rslt);

    if (rslt == BMI2_OK)
    {
        /* NOTE: The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[ACCEL].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

        /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
        config[ACCEL].cfg.acc.range = BMI2_ACC_RANGE_2G;

        /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
         * if it is set to 2, then 2^(bandwidth parameter) samples
         * are averaged, resulting in 4 averaged samples.
         * Note1 : For more information, refer the datasheet.
         * Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
         * this has an adverse effect on the power consumed.
         */
        config[ACCEL].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

        /* Enable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         * For more info refer datasheet.
         */
        config[ACCEL].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

        /* The user can change the following configuration parameters according to their requirement. */
        /* Set Output Data Rate */
        config[GYRO].cfg.gyr.odr = BMI2_GYR_ODR_200HZ;

        /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
        config[GYRO].cfg.gyr.range = BMI2_GYR_RANGE_2000;

        /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
        config[GYRO].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

        /* Enable/Disable the noise performance mode for precision yaw rate sensing
         * There are two modes
         *  0 -> Ultra low power mode(Default)
         *  1 -> High performance mode
         */
        config[GYRO].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

        /* Enable/Disable the filter performance mode where averaging of samples
         * will be done based on above set bandwidth and ODR.
         * There are two modes
         *  0 -> Ultra low power mode
         *  1 -> High performance mode(Default)
         */
        config[GYRO].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

        /* Set the accel and gyro configurations. */
        rslt = bmi270_set_sensor_config(config, 2, bmi2_dev);
        bmi2_error_codes_print_result(rslt);
    }

    return rslt;
}
