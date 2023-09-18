#include <zsw_light_sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

LOG_MODULE_REGISTER(light_sensor, LOG_LEVEL_DBG);

#define APDS_9306_065_ADDRESS   0x52
#define APDS_9306_065_REG_ID    0x06
#define APDS_9306_065_CHIP_ID   0xB3

// TODO: Proper driver implementation
// Currently just reads ID to verify the HW

#define I2C_DEV DT_NODELABEL(i2c1)

int zsw_light_sensor_detect(void)
{
    uint8_t id = 0;
    const struct device *i2c_dev = DEVICE_DT_GET_OR_NULL(I2C_DEV);

    if (i2c_dev == NULL) {
        LOG_ERR("Error: no APDS device found.");
        return false;
    }

    if (!device_is_ready(i2c_dev)) {
        LOG_ERR("Error: Device \"%s\" is not ready; "
                "check the driver initialization logs for errors.",
                i2c_dev->name);
        return false;
    }

    /* Verify sensor working by reading the ID */
    int err = i2c_reg_read_byte(i2c_dev, APDS_9306_065_ADDRESS, APDS_9306_065_REG_ID, &id);
    if (err) {
        LOG_ERR("Failed reading device id from APDS");
        return false;
    }

    if (id == APDS_9306_065_CHIP_ID) {
        LOG_INF("APDS id: %d", id);
        return true;
    } else {
        LOG_ERR("Wrong APDS id: %d", id);
    }

    return false;
}

SYS_INIT(zsw_light_sensor_detect, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
