#include <magnetometer.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <inttypes.h>
#include <math.h>

LOG_MODULE_REGISTER(magnetometer, LOG_LEVEL_WRN);

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

static const struct device *const magnetometer = DEVICE_DT_GET_ONE(st_lis2mdl);

static double xyz_to_rotation(double x, double y, double z);
static void lis2mdl_trigger_handler(const struct device *dev, const struct sensor_trigger *trig);

static double last_heading = 0;

void magnetometer_init(void)
{
    if (!device_is_ready(magnetometer)) {
        LOG_ERR("Device %s is not ready\n", magnetometer->name);
        return;
    }

    struct sensor_trigger trig;
    struct sensor_value odr_attr;

    odr_attr.val1 = 10; // TODO what value
    odr_attr.val2 = 0;

    if (sensor_attr_set(magnetometer, SENSOR_CHAN_ALL,
                SENSOR_ATTR_SAMPLING_FREQUENCY, &odr_attr) < 0) {
        LOG_ERR("Cannot set sampling frequency for LIS2MDL\n");
        return;
    }

    trig.type = SENSOR_TRIG_DATA_READY;
    trig.chan = SENSOR_CHAN_MAGN_XYZ;
    sensor_trigger_set(magnetometer, &trig, lis2mdl_trigger_handler);
}

double magnetometer_get_heading(void)
{
    return last_heading;
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
        sensor_value_to_double(&magn[0]),
        sensor_value_to_double(&magn[1]),
        sensor_value_to_double(&magn[2]));

    LOG_DBG("LIS2MDL: Temperature: %.1f C\n",
        sensor_value_to_double(&die_temp2));

    last_heading = xyz_to_rotation(sensor_value_to_double(&magn[0]),
        sensor_value_to_double(&magn[1]),
        sensor_value_to_double(&magn[2]));
    
    LOG_DBG("Rotation: %f", last_heading);
}

// https://arduino.stackexchange.com/questions/18625/converting-three-axis-magnetometer-to-degrees/88707#88707
// Note this assumes watch is flat to eath, TODO use accelerometer to compensate when tilted.
static double xyz_to_rotation(double x, double y, double z)
{
    double heading = atan2(y, x) * 180 / M_PI;
    if (heading < 0) {
        heading = 360 + heading;
    }
    return heading;
}