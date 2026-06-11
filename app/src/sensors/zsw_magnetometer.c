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

#define CONFIG_BOARD_NATIVE_SIM

typedef struct {
    float offset_x; //for hard correction
    float offset_y;
    float offset_z;
    float transform[3][3]; //for soft correction
} magn_calib_data_t;

static magn_calib_data_t calibration_data = { //transform needs an identity default
    .transform = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
    },
};

static void copy_offset_and_matrix_to_calibration_data(
    const Callibration_t calib);

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

#ifdef CONFIG_BOARD_NATIVE_SIM
static int dummy_count;
#endif

#define CAL_MIN_RANGE 20.0 // Axis range required for 100% progress; tune on hardware.

#define CAL_SAMPLE_MAX 200 // Max calibration samples to retain; tune on hardware if needed.

static double cal_x[CAL_SAMPLE_MAX];
static double cal_y[CAL_SAMPLE_MAX];
static double cal_z[CAL_SAMPLE_MAX];
static int cal_sample_count;





static int axis_progress(double min, double max)
{
    double range = max - min;
    int progress = (int)((range * 100.0) / CAL_MIN_RANGE);

    return MIN(progress, 100);
}

//dummy parametric ellipsoid calibration data as have no hardware for data gathering

#define DUMMY_N 80

double dummy_x[DUMMY_N], dummy_y[DUMMY_N], dummy_z[DUMMY_N];

static void debug_fill_dummy_samples(double x[], double y[], double z[])
{
    const double ox = 8.0, oy = -5.0, oz = 3.0;
    const double ax = 40.0, by = 25.0, cz = 15.0;


    /*

    I verified the ellipsoid fitting using synthetic magnetometer data generated from a known ellipsoid with hard-iron offsets (8, -5, 3) and axis radii (40, 25, 15)
    The recovered hard-iron offsets matched the synthetic offsets
    The recovered scale factors matched the expected inverse axis scaling (1/40, 1/25, 1/15), transforming the ellipsoid back into a unit sphere




    */



    for (int i = 0; i < DUMMY_N; i++) {
        double t = (2.0 * M_PI * i) / DUMMY_N;
        double p = M_PI * ((i * 37) % DUMMY_N) / DUMMY_N;

        x[i] = ox + ax * cos(t) * sin(p);
        y[i] = oy + by * sin(t) * sin(p);
        z[i] = oz + cz * cos(p);
    }

    //for (int i = 0; i < DUMMY_N; i++) {
    //printf("%f\n", dummy_x[i]);
    //}
      //ok so below shows the same sample format as the library used
      /*
      8.000000
      47.600286
      ...

      */

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

    /* gathering data for subsequent calibration */
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

        if (cal_sample_count < CAL_SAMPLE_MAX) {  //storing real data
            cal_x[cal_sample_count] = last_x;
            cal_y[cal_sample_count] = last_y;
            cal_z[cal_sample_count] = last_z;
            cal_sample_count++;
        }



        int px = axis_progress(min_x, max_x);
        int py = axis_progress(min_y, max_y);
        int pz = axis_progress(min_z, max_z);

        compass_ui_set_calibration_progress(px, py, pz);

        calibration_ready =
        (px == 100) &&
        (py == 100) &&
        (pz == 100);

    }

    /* apply last known Hard-iron correction */
    const double x = last_x - calibration_data.offset_x;
    const double y = last_y - calibration_data.offset_y;
    const double z = last_z - calibration_data.offset_z;

    /* apply last known Soft-iron correction */
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



static int magn_cal_load(const char *p_key, size_t len,
                         settings_read_cb read_cb, void *p_cb_arg, void *p_param)
{
    ARG_UNUSED(p_key);

    if (len != sizeof(magn_calib_data_t)) {
        LOG_ERR("Invalid length of magn calibration data");
        return -EINVAL;
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

int zsw_magnetometer_gather_data(void)
{
    calibration_ready = false;
    cal_sample_count = 0;

    //commented out as on simulator
    /*
    if (!device_is_ready(magnetometer)) {
        return -ENODEV;
    }
    */
    #ifdef CONFIG_BOARD_NATIVE_SIM
    dummy_count = 0;
    #endif

    max_x = -100000;
    max_y = -100000;
    max_z = -100000;
    min_x = 100000;
    min_y = 100000;
    min_z = 100000;

    getting_data = true;  //trigger data gathering part of lis2mdl_trigger_handler

    return 0;
}

bool zsw_magnetometer_calibration_ready(void) //exercising the whole UI flow without hardware
{
#ifdef CONFIG_BOARD_NATIVE_SIM

    dummy_count++;
    return dummy_count > 20;   // ready after ~20 timer ticks

#else
    return calibration_ready;
#endif
}

int zsw_magnetometer_compute_compensation(void)   //this used to be called zsw_magnetometer_stop_calibration
{


    //calibration_data.offset_x = (max_x + min_x) / 2;  //old simple method
    //calibration_data.offset_y = (max_y + min_y) / 2;
    //calibration_data.offset_z = (max_z + min_z) / 2;







    //return 1;

    //commented out as on simulator
    //if (!device_is_ready(magnetometer)) {
        //return -ENODEV;
    //}

    getting_data = false;

    //following hard and soft calibration method from https://github.com/michal34512/Magnetometer-calibration
    //LOG_INF("dummy: creating vectors");

    //debug_fill_dummy_samples(dummy_x, dummy_y, dummy_z);

    //Vector vx = vec_from_array(dummy_x, DUMMY_N);
    //Vector vy = vec_from_array(dummy_y, DUMMY_N);
    //Vector vz = vec_from_array(dummy_z, DUMMY_N);




    //running on real data


    Vector vx = vec_from_array(cal_x, cal_sample_count);
    Vector vy = vec_from_array(cal_y, cal_sample_count);
    Vector vz = vec_from_array(cal_z, cal_sample_count);

    LOG_INF("Data: running fit");
    Callibration_t calib = calib_calibrate_sensor(vx, vy, vz);






    /*
    //checking the end result of the compensation:

    <pre>[00:00:04.908,400] &lt;inf&gt; zsw_magnetometer: Calibration successful
    [00:00:04.908,400] &lt;inf&gt; zsw_magnetometer: Offset: 8.000 -5.000 3.000 //stored but not applied hard iron cal paramaters
    [00:00:04.908,400] &lt;inf&gt; zsw_magnetometer: Transform:
    [00:00:04.908,400] &lt;inf&gt; zsw_magnetometer: 0.025 0.000 0.000  //stored but not applied soft iron cal parameters
    [00:00:04.908,400] &lt;inf&gt; zsw_magnetometer: 0.000 0.040 0.000
    [00:00:04.908,400] &lt;inf&gt; zsw_magnetometer: 0.000 0.000 0.067
    </pre>




    //soft iron ie distortion
    The corrected points should lie on a sphere.



    //hard iron re offset
    The corrected points should be centred approximately at (0, 0, 0)






    */


    LOG_INF("dummy: success=%d", calib_calibration_success(calib));
    if (!calib_calibration_success(calib)) {
        calib_free(calib);
        return -EINVAL;
    }

    LOG_INF("dummy: copying result");
    copy_offset_and_matrix_to_calibration_data(calib);

    LOG_INF("dummy: saving settings");
    settings_save_one(SETTINGS_MAGN_CALIB, &calibration_data, sizeof(calibration_data));

    /* this could do with eg:
    if (settings_save_one(...) != 0) {
        calib_free(calib);
        return -EIO;
    }
    */

    LOG_INF("Calibration successful");
    LOG_INF("Offset: %.3f %.3f %.3f",
      calibration_data.offset_x,  //new hard correction
      calibration_data.offset_y,
      calibration_data.offset_z);

    LOG_INF("Transform:");
    LOG_INF("%.3f %.3f %.3f", calibration_data.transform[0][0], calibration_data.transform[0][1], calibration_data.transform[0][2]);  //new soft correction
    LOG_INF("%.3f %.3f %.3f", calibration_data.transform[1][0], calibration_data.transform[1][1], calibration_data.transform[1][2]);
    LOG_INF("%.3f %.3f %.3f", calibration_data.transform[2][0], calibration_data.transform[2][1], calibration_data.transform[2][2]);







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





/*

debug only function that bypasses the sensor
dummy samples
library fit
save settings
later reads use the created matrix.
nb this provides both hard and soft correction

https://github.com/michal34512/Magnetometer-calibration

Vendor the source files

sensor_calibration.c/.h
ellipsoid_fit.c/.h
eigen.c/.h
qr.c/.h
matrix.c/.h
vector.c/.h

copied into new folder src/ext/magnetometer_calibration/

Then add them to the Zephyr build with CMakeLists.txt.

For a dummy calibration path, just use a handful of points that obviously aren't centered

temporarily call dummy function from zsw_magnetometer_start_calibration()

*/


/*
lib guves me:
Callibration_t calib
calib.offset      // Vector *
calib.transform   // Matrix *

But zsw wants:
magn_calib_data_t calibration_data;

*/

static void copy_offset_and_matrix_to_calibration_data( //converting to zsw friendly format
    const Callibration_t calib)
{

    //convert hard
    calibration_data.offset_x = VEC_X(calib.offset);
    calibration_data.offset_y = VEC_Y(calib.offset);
    calibration_data.offset_z = VEC_Z(calib.offset);

    //convert soft
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            calibration_data.transform[r][c] =
                MAT_ELEM(calib.transform, r, c);
        }
    }
}
