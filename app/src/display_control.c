#include <vibration_motor.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

void display_control_init(void)
{
    const struct device *display_dev;

    display_dev = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Device not ready, aborting test");
        return;
    }
}

void display_control_set_brightness(uint8_t percent)
{
    int ret;
    uint32_t step = pwm_led0.period / 100;
    uint32_t pulse_width = step * percent;

    if (!device_is_ready(pwm_led0.dev)) {
        printk("Error: PWM device %s is not ready\n",
               pwm_led0.dev->name);
        return;
    }

    ret = pwm_set_pulse_dt(&pwm_led0, pulse_width);
    __ASSERT(ret == 0, "pwm error: %d for pulse: %d", ret, pulse_width);
}

