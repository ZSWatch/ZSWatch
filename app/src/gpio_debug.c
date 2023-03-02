#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/__assert.h>
#include <assert.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <gpio_debug.h>

LOG_MODULE_REGISTER(gpio_debug, LOG_LEVEL_WRN);

static struct gpio_dt_spec watch_gpios[] = {
    //[DISPLAY_EN] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio1)), .pin = 1 },
    //[TOUCH_RST] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 20 },
    //[DISPLAY_RST] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 23 },
    //[BTN_1] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 2 },
    //[DISPLAY_DC] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 16 },
    //[QSPI_1_SO] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 22 },
    //[V5_REG_EN] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 31 },
    //[MAX30_INT] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 30 },
    //[QSPI_0_SO] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio1)), .pin = 9 },
    //[DRV_VIB_PWM] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 11 },
    //[DRV_VIB_EN] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 12 },
//
    //[DISPLAY_DATA] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio1)), .pin = 0 },
    //[BAT_MON] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 5 },
    //[BAT_MON_EN] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 4 },
    //[INT1_LIS] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 29 },
    //[INT2_LIS] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 28 },
    //[BTN_2] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 23 },
    //[QSPI_2_WP] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 7 },
    //[DISPLAY_CLK] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 0 },
    //[DISPLAY_CS] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 1 },
//
    //[BTN_3] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 9},
    //[SDA_ANNA] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 27 },
    //[SCL_ANNA] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 6 },
    //[DISPLAY_BLK] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 26 },
    //[QSPI_CS] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 15 },
    //[QSPI_3_RST] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 20 },
    //[QSPI_CLK] = { .port = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(gpio0)), .pin = 14 },
};

static uint8_t pinDefaultState[ARRAY_SIZE(watch_gpios)];



void gpio_debug_init(void)
{
    int ret;

    for (int i = 0; i < ARRAY_SIZE(watch_gpios); i++) {
        __ASSERT(device_is_ready(watch_gpios[i].port), "Error: button device %s is not ready\n",  watch_gpios[i].port->name);
        ret = gpio_pin_configure_dt(&watch_gpios[i], GPIO_OUTPUT);
        __ASSERT_NO_MSG(ret == 0);
        ret = gpio_pin_set_dt(&watch_gpios[i], 0);
        __ASSERT_NO_MSG(ret == 0);
    }
}

static bool check_all_match(gpioWatchId_t skip)
{
    int val;
    bool ret = true;

    for (int i = 0; i < ARRAY_SIZE(watch_gpios); i++) {
        if (i == (int)skip) {
            continue;
        }
        val = gpio_pin_get_dt(&watch_gpios[i]);
        if (val != pinDefaultState[i]) {
            LOG_ERR("Missmatch, pin %d changed value of %d, was %d, expected %d", skip, i, val, pinDefaultState[i]);
            ret = false;
        }
    }
    return ret;
}

void gpio_debug_test_all(void)
{
    int ret;
    bool pass = true;

    // Configure all as input
    for (int i = 0; i < ARRAY_SIZE(watch_gpios); i++) {
        __ASSERT(device_is_ready(watch_gpios[i].port), "Error: button device %s is not ready\n",  watch_gpios[i].port->name);
        ret = gpio_pin_configure_dt(&watch_gpios[i], GPIO_OUTPUT);
        __ASSERT_NO_MSG(ret == 0);
        ret = gpio_pin_set_dt(&watch_gpios[i], 0);
        __ASSERT_NO_MSG(ret == 0);
        k_msleep(10);
        ret = gpio_pin_configure_dt(&watch_gpios[i], GPIO_INPUT | GPIO_PULL_DOWN);
        __ASSERT_NO_MSG(ret == 0);
    }
    k_msleep(1000);
    for (int i = 0; i < ARRAY_SIZE(watch_gpios); i++) {
        int val = gpio_pin_get_dt(&watch_gpios[i]);
        printk("%d => %d\n", i, val);
        pinDefaultState[i] = val;
        k_msleep(10);
    }

    for (int i = 0; i < ARRAY_SIZE(watch_gpios); i++) {
        printk("Check: %d\n", i);
        k_msleep(250);
        ret = gpio_pin_configure_dt(&watch_gpios[i], GPIO_OUTPUT);
        __ASSERT_NO_MSG(ret == 0);
        ret = gpio_pin_set_dt(&watch_gpios[i], 1);
        __ASSERT_NO_MSG(ret == 0);
        k_msleep(250);
        ret = check_all_match(i);
        if (!ret) {
            pass = false;
            printk("Short detected for %d", i);
        }
        ret = gpio_pin_set_dt(&watch_gpios[i], 0);
        __ASSERT_NO_MSG(ret == 0);
        ret = gpio_pin_configure_dt(&watch_gpios[i], GPIO_INPUT | GPIO_PULL_DOWN);
        __ASSERT_NO_MSG(ret == 0);
        k_msleep(250);
    }

    if (pass) {
        printk("No shorts detected!");
    } else {
        printk("Shorts detected!");
    }
}

void gpio_debug_test(gpioWatchId_t gpioId, int val)
{
    LOG_INF("Testing %d (%s.%d)", gpioId, watch_gpios[gpioId].port->name, watch_gpios[gpioId].pin);
    int ret = gpio_pin_configure_dt(&watch_gpios[gpioId], GPIO_OUTPUT);
    __ASSERT_NO_MSG(ret == 0);
    gpio_pin_set_dt(&watch_gpios[gpioId], val);
    __ASSERT_NO_MSG(ret == 0);
}