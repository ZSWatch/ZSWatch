#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/__assert.h>
#include <assert.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <heart_rate_sensor.h>

LOG_MODULE_REGISTER(hr_sensor, LOG_LEVEL_WRN);
/*
1. Just plot the RED HR
2. Imlement alg. to detect a beat

Inspiration:
https://github.com/MikroElektronika/HEXIWEAR/blob/master/SW/FTF/HEXIWEAR_OLED_sensors_RTOS/Sources/apps/heartRate/src/heartRate_driver.c
*/


static const struct device *const hr_dev = DEVICE_DT_GET_ANY(maxim_max30101);

int heart_rate_sensor_init(void)
{
    if (hr_dev == NULL) {
        LOG_ERR("Could not get max30101 device\n");
        return -ENODEV;
    }
    if (!device_is_ready(hr_dev)) {
        LOG_ERR("max30101 device %s is not ready\n", hr_dev->name);
        return -ENODEV;
    }

    return 0;
}

int heart_rate_sensor_fetch(plot_page_led_values_t *sample)
{
    struct sensor_value green, red, ir;
    sensor_sample_fetch(hr_dev);
    sensor_channel_get(hr_dev, SENSOR_CHAN_GREEN, &green);
    sensor_channel_get(hr_dev, SENSOR_CHAN_RED, &red);
    sensor_channel_get(hr_dev, SENSOR_CHAN_IR, &ir);

    /* Print green LED data*/
    LOG_INF("\nGREEN=%d\nRED=%d\nIR=%d", green.val1, red.val1, ir.val1);

    sample->green =  green.val1;
    sample->red = red.val1;
    sample->ir = ir.val1;

    return 0;
}
