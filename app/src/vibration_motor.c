#include <vibration_motor.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>

static const struct pwm_dt_spec pwm_led1 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led1));

static const struct gpio_dt_spec enable_gpio = GPIO_DT_SPEC_GET(DT_NODELABEL(vib_pwr), enable_gpios);
//static const struct device *const reg2 = DEVICE_DT_GET(DT_NODELABEL(reg2));
//regulator_disable(reg2)

void vibration_motor_init(void)
{
    if (!gpio_pin_configure_dt(&enable_gpio, GPIO_OUTPUT_LOW)) {
        printk("Failed init vibration motor enable pin\n");
    }
}

void vibration_motor_set_power(uint8_t percent)
{
    int ret;
    uint32_t step = pwm_led1.period / 100;
    uint32_t pulse_width = step * percent;

    if (!device_is_ready(pwm_led1.dev)) {
        return;
    }

    ret = pwm_set_pulse_dt(&pwm_led1, pulse_width);
    __ASSERT(ret == 0, "pwm error: %d for pulse: %d", ret, pulse_width);
}

void vibration_motor_set_on(bool on)
{
    gpio_pin_set_dt(&enable_gpio, on);
}
