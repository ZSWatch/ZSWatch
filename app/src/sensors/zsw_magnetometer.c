/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/settings/settings.h>
#include <zephyr/pm/pm.h>
#include <zephyr/pm/device.h>
#include <zephyr/pm/policy.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <inttypes.h>
#include <math.h>

#include "events/zsw_periodic_event.h"
#include "events/magnetometer_event.h"
#include "sensors/zsw_magnetometer.h"

#include "sensor_calibration.h"
#include "vector.h"
#include "matrix.h"

LOG_MODULE_REGISTER(zsw_magnetometer, CONFIG_ZSW_SENSORS_LOG_LEVEL);

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

#define SETTINGS_NAME_MAGN              "magn"
#define SETTINGS_KEY_CALIB              "calibr"
#define SETTINGS_MAGN_CALIB             SETTINGS_NAME_MAGN "/" SETTINGS_KEY_CALIB

typedef struct {
    float offset_x;
    float offset_y;
    float offset_z;
    float transform[3][3];
} magn_calib_data_t;

static magn_calib_data_t calibration_data = {
    .transform = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
    },
};

static void copy_offset_and_matrix_to_calibration_data(
    const Calibration_t calib);

static double last_x;
static double last_y;
static double last_z;
static double max_x;
static double max_y;
static double max_z;
static double min_x;
static double min_y;
static double min_z;
static bool getting_data;
static bool calibration_ready;
static bool recalibration_required;

#define CAL_MIN_RANGE 20.0 // Axis range required for 100% progress; tune on hardware.

#define CAL_SAMPLE_MAX 200 // Max calibration samples to retain; tune on hardware if needed.

static double cal_x[CAL_SAMPLE_MAX];
static double cal_y[CAL_SAMPLE_MAX];
static double cal_z[CAL_SAMPLE_MAX];
static int cal_sample_count;

static int cal_progress_x;
static int cal_progress_y;
static int cal_progress_z;

bool zsw_magnetometer_recalibration_required(void)
{
    return recalibration_required;
}


static int axis_progress(double min, double max)
{
    double range = max - min;
    int progress = (int)((range * 100.0) / CAL_MIN_RANGE);

    return MIN(progress, 100);
}


static void zbus_periodic_slow_callback(const struct zbus_channel *chan);

ZBUS_CHAN_DECLARE(magnetometer_data_chan);
ZBUS_CHAN_DECLARE(periodic_event_1s_chan);
ZBUS_LISTENER_DEFINE(zsw_magnetometer_lis, zbus_periodic_slow_callback);
static const struct device *const magnetometer = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(lis2mdl));

static void zbus_periodic_slow_callback(const struct zbus_channel *chan)
{
    float x;
    float y;
    float z;

    if (zsw_magnetometer_get_all(&x, &y, &z)) {
        return;
    }

    struct magnetometer_event evt = {
        .x = x,
        .y = y,
        .z = z
    };

    zbus_chan_pub(&magnetometer_data_chan, &evt, K_MSEC(250));
}

static void lis2mdl_trigger_handler(const struct device *dev,
                                    const struct sensor_trigger *trig)
{
    struct sensor_value die_temp2;
    struct sensor_value magn[3];
    sensor_sample_fetch_chan(dev, SENSOR_CHAN_ALL);

    sensor_channel_get(magnetometer, SENSOR_CHAN_MAGN_XYZ, magn);
    sensor_channel_get(magnetometer, SENSOR_CHAN_DIE_TEMP, &die_temp2);

    LOG_DBG("LIS2MDL: Magn (gauss): x: %.3f, y: %.3f, z: %.3f\n",
            sensor_value_to_float(&magn[1]),
            sensor_value_to_float(&magn[0]),
            sensor_value_to_float(&magn[2]));

    // Convert Gauss to micro Tesla
    last_x = sensor_value_to_float(&magn[1]) * 10; // Swap x, y to match IMU orientation
    last_y = sensor_value_to_float(&magn[0]) * 10;
    last_z = sensor_value_to_float(&magn[2]) * 10;

    if (getting_data) {
        if (last_x < min_x) {
            min_x = last_x;
        }
        if (last_x > max_x) {
            max_x = last_x;
        }

        if (last_y < min_y) {
            min_y = last_y;
        }
        if (last_y > max_y) {
            max_y = last_y;
        }

        if (last_z < min_z) {
            min_z = last_z;
        }
        if (last_z > max_z) {
            max_z = last_z;
        }

        if (cal_sample_count < CAL_SAMPLE_MAX) {
            cal_x[cal_sample_count] = last_x;
            cal_y[cal_sample_count] = last_y;
            cal_z[cal_sample_count] = last_z;
            cal_sample_count++;
        }

        cal_progress_x = axis_progress(min_x, max_x);
        cal_progress_y = axis_progress(min_y, max_y);
        cal_progress_z = axis_progress(min_z, max_z);

        calibration_ready =
            (cal_progress_x == 100) &&
            (cal_progress_y == 100) &&
            (cal_progress_z == 100);

    }


    const double x = last_x - calibration_data.offset_x;
    const double y = last_y - calibration_data.offset_y;
    const double z = last_z - calibration_data.offset_z;


    last_x =
        calibration_data.transform[0][0] * x +
        calibration_data.transform[0][1] * y +
        calibration_data.transform[0][2] * z;

    last_y =
        calibration_data.transform[1][0] * x +
        calibration_data.transform[1][1] * y +
        calibration_data.transform[1][2] * z;

    last_z =
        calibration_data.transform[2][0] * x +
        calibration_data.transform[2][1] * y +
        calibration_data.transform[2][2] * z;

    LOG_DBG("Corrected magn: %.3f %.3f %.3f",
            last_x, last_y, last_z);
    }



int zsw_magnetometer_get_calibration_progress(int *px, int *py, int *pz)
  {
    if (!px || !py || !pz) {
        return -EINVAL;
    }

    *px = cal_progress_x;
    *py = cal_progress_y;
    *pz = cal_progress_z;

    return 0;
  }



static int magn_cal_load(const char *p_key, size_t len,
                         settings_read_cb read_cb, void *p_cb_arg, void *p_param)
{
    ARG_UNUSED(p_key);

    if (len != sizeof(magn_calib_data_t)) {
        LOG_ERR("Ignoring old magnetometer calibration data. Recalibration required.");

        calibration_data.offset_x = 0.0f;
        calibration_data.offset_y = 0.0f;
        calibration_data.offset_z = 0.0f;

        recalibration_required = true;

        return 0;


    }

    if (read_cb(p_cb_arg, &calibration_data, len) != sizeof(magn_calib_data_t)) {
        LOG_ERR("Error reading magn calibration data");
        return -EIO;
    }

    LOG_WRN("Calibration data loaded: x: %f, y: %f, z: %f",
            calibration_data.offset_x, calibration_data.offset_y, calibration_data.offset_z);

    return 0;
}

int zsw_magnetometer_init(void)
{
    if (!device_is_ready(magnetometer)) {
        LOG_ERR("Device magnetometer is not ready");
        return -ENODEV;
    }

    if (settings_subsys_init()) {
        LOG_ERR("Error during settings_subsys_init!");
        return -EFAULT;
    }

    if (settings_load_subtree_direct(SETTINGS_MAGN_CALIB, magn_cal_load, NULL)) {
        LOG_ERR("Error during settings_load_subtree!");
        return -EFAULT;
    }

    struct sensor_trigger trig;
    struct sensor_value odr_attr;

    odr_attr.val1 = 20; // TODO what value
    odr_attr.val2 = 0;

    if (sensor_attr_set(magnetometer, SENSOR_CHAN_ALL,
                        SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) != 0) {
        LOG_ERR("Cannot set sampling frequency for LIS2MDL");
        return -EFAULT;
    }

    trig.type = SENSOR_TRIG_DATA_READY;
    trig.chan = SENSOR_CHAN_MAGN_XYZ;
    sensor_trigger_set(magnetometer, &trig, lis2mdl_trigger_handler);

    // TODO handle power save, enable/disable etc. to save power
    if (pm_device_action_run(magnetometer, PM_DEVICE_ACTION_SUSPEND) != 0) {
        LOG_ERR("Failed to suspend LIS2MDL!");
        return -EFAULT;
    }

    zsw_periodic_chan_add_obs(&periodic_event_1s_chan, &zsw_magnetometer_lis);

    return 0;
}

int zsw_magnetometer_set_enable(bool enabled)
{
    int ret;
    if (!device_is_ready(magnetometer)) {
        LOG_ERR("No magnetometer found!");
        return -ENODEV;
    }

    if (enabled) {
        ret = pm_device_action_run(magnetometer, PM_DEVICE_ACTION_RESUME);
        if (ret != 0 && ret != -EALREADY) {
            LOG_ERR("Failed to resume LIS2MDL!");
            return -EFAULT;
        }
    } else {
        ret = pm_device_action_run(magnetometer, PM_DEVICE_ACTION_SUSPEND);
        if (ret != 0 && ret != -EALREADY) {
            LOG_ERR("Failed to suspend LIS2MDL!");
            return -EFAULT;
        }
    }

    return 0;
}



int zsw_magnetometer_cancel_calibration(void)
{
    getting_data = false;
    calibration_ready = false;
    cal_sample_count = 0;

    return 0;
}


int zsw_magnetometer_gather_data(void)
{
    calibration_ready = false;
    cal_sample_count = 0;



    if (!device_is_ready(magnetometer)) {
        return -ENODEV;
    }

    max_x = -100000;
    max_y = -100000;
    max_z = -100000;
    min_x = 100000;
    min_y = 100000;
    min_z = 100000;

    getting_data = true;

    return 0;
}

bool zsw_magnetometer_calibration_ready(void)
{
    return calibration_ready;

}

int zsw_magnetometer_compute_compensation(void)
{

    if (!device_is_ready(magnetometer)) {
        return -ENODEV;
    }

    getting_data = false;

    Vector vx = vec_from_array(cal_x, cal_sample_count);
    Vector vy = vec_from_array(cal_y, cal_sample_count);
    Vector vz = vec_from_array(cal_z, cal_sample_count);

    Calibration_t calib = calib_calibrate_sensor(vx, vy, vz);

    if (!calib_calibration_success(calib)) {
        calib_free(calib);
        return -EINVAL;
    }

    copy_offset_and_matrix_to_calibration_data(calib);

    int ret = settings_save_one(SETTINGS_MAGN_CALIB,
                            &calibration_data,
                            sizeof(calibration_data));
    if (ret != 0) {
        LOG_ERR("Failed to save magnetometer calibration: %d", ret);
        calib_free(calib);
        return ret;
    }

    calib_free(calib);

    return 0;
}




int zsw_magnetometer_get_all(float *x, float *y, float *z)
{
    if (!device_is_ready(magnetometer)) {
        return -ENODEV;
    }

    *x = last_x;
    *y = last_y;
    *z = last_z;

    return 0;
}


static void copy_offset_and_matrix_to_calibration_data(
    const Calibration_t calib)
{


    calibration_data.offset_x = VEC_X(calib.offset);
    calibration_data.offset_y = VEC_Y(calib.offset);
    calibration_data.offset_z = VEC_Z(calib.offset);

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            calibration_data.transform[r][c] =
                MAT_ELEM(calib.transform, r, c);
        }
    }
}
