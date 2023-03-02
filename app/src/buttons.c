#include "buttons.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/__assert.h>
#include <assert.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(buttons, LOG_LEVEL_DBG);

#define STACKSIZE               1024
#define PRIORITY                7

#define BTN_LONG_PRESS_LIMIT  1000
#define NUM_BUTTONS           3

static void buttonPressedIsr(const struct device *dev, struct gpio_callback *cb, uint32_t pins);
static void handleButtonThread(void);

typedef struct buttons_t {
    struct gpio_dt_spec btn;
    struct gpio_callback callback_data;
} buttons_t;

static buttons_t buttons[NUM_BUTTONS] = {
    { .btn = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw0), gpios, {0}) },
    { .btn = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw1), gpios, {0}) },
    { .btn = GPIO_DT_SPEC_GET_OR(DT_ALIAS(sw2), gpios, {0}) }
};


static buttonHandlerCallback_t callback;
static struct k_sem btnSem;
static uint8_t pressed_button_id;

K_THREAD_DEFINE(buttonThreadId, STACKSIZE, handleButtonThread, NULL, NULL, NULL, PRIORITY, 0, K_TICKS_FOREVER);


void buttonsInit(buttonHandlerCallback_t handler)
{
    int ret;
    callback = handler;

    k_sem_init(&btnSem, 0, 1);

    for (int i = 0; i < NUM_BUTTONS; i++) {
        __ASSERT(device_is_ready(buttons[i].btn.port), "Error: button device %s is not ready\n",  buttons[i].btn.port->name);
        ret = gpio_pin_configure_dt(&buttons[i].btn, GPIO_INPUT | GPIO_PULL_UP);
        __ASSERT_NO_MSG(ret == 0);
        gpio_init_callback(&buttons[i].callback_data, buttonPressedIsr,  BIT(buttons[i].btn.pin));
        ret = gpio_add_callback(buttons[i].btn.port, &buttons[i].callback_data);
        __ASSERT_NO_MSG(ret == 0);
        ret = gpio_pin_interrupt_configure_dt(&buttons[i].btn, GPIO_INT_EDGE_TO_ACTIVE);
        __ASSERT_NO_MSG(ret == 0);
        LOG_INF("Registrated Button %d", i);
    }

    k_thread_start(buttonThreadId);
}

int button_read(buttonId_t button)
{
    return gpio_pin_get_dt(&buttons[button].btn);
}

static void buttonPressedIsr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    pressed_button_id = 0xFF;
    for (int i = 0; i < NUM_BUTTONS; i++) {
        if (dev == buttons[i].btn.port && pins & BIT(buttons[i].btn.pin)) {
            pressed_button_id = i;
        }
        gpio_pin_interrupt_configure(buttons[i].btn.port, buttons[i].btn.pin, GPIO_INT_DISABLE);
    }
    __ASSERT_NO_MSG(pressed_button_id != 0xFF);
    k_sem_give(&btnSem);
}

static void handleButtonThread(void)
{
    int ret;
    uint32_t val;
    int64_t btn_press_start_ms;
    int64_t btn_pressed_ms;
    buttonPressType_t press_type;

    while (true) {
        ret = k_sem_take(&btnSem, K_FOREVER);
        assert(ret == 0);
        LOG_DBG("Pressed button index: %d", pressed_button_id);
        btn_press_start_ms = k_uptime_get_32();
        k_sleep(K_MSEC(100)); // Let debounce stabalize
        val = gpio_pin_get_dt(&buttons[pressed_button_id].btn);

        while (val) {
            val = gpio_pin_get_dt(&buttons[pressed_button_id].btn);
            k_sleep(K_MSEC(10));
        }

        btn_pressed_ms = k_uptime_delta(&btn_press_start_ms);

        if (btn_pressed_ms < BTN_LONG_PRESS_LIMIT) {
            press_type = BUTTONS_SHORT_PRESS;
        } else {
            press_type = BUTTONS_LONG_PRESS;
        }
        callback(press_type, pressed_button_id);
        for (int i = 0; i < NUM_BUTTONS; i++) {
            gpio_pin_interrupt_configure(buttons[i].btn.port, buttons[i].btn.pin, GPIO_INT_EDGE_TO_ACTIVE);
        }
    }
}
