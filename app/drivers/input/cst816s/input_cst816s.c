/*
 * Copyright (c) 2021 Qingsong Gou <gouqs@hotmail.com>
 * Copyright (c) 2022 Jakob Krantz <mail@jakobkrantz.se>
 * Copyright (c) 2023 Daniel Kampert <kontakt@daniel-kampert.de>
 * Copyright (c) 2025 ZSWatch Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT hynitron_cst816s

#include <zephyr/sys/byteorder.h>
#include <zephyr/input/input.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/device.h>
#include <zephyr/dt-bindings/input/cst816s-gesture-codes.h>

LOG_MODULE_REGISTER(cst816s, CONFIG_INPUT_LOG_LEVEL);

#define CST816S_CHIP_ID1                0xB4
#define CST816S_CHIP_ID2                0xB5
#define CST816S_CHIP_ID3               	0xB6

#define CST816S_REG_DATA                0x00
#define CST816S_REG_GESTURE_ID          0x01
#define CST816S_REG_FINGER_NUM          0x02
#define CST816S_REG_XPOS_H              0x03
#define CST816S_REG_XPOS_L              0x04
#define CST816S_REG_YPOS_H              0x05
#define CST816S_REG_YPOS_L              0x06
#define CST816S_REG_BPC0H               0xB0
#define CST816S_REG_BPC0L               0xB1
#define CST816S_REG_BPC1H               0xB2
#define CST816S_REG_BPC1L               0xB3
#define CST816S_REG_POWER_MODE          0xA5
#define CST816S_REG_SLEEP_MODE          0xE5
#define CST816S_REG_CHIP_ID             0xA7
#define CST816S_REG_PROJ_ID             0xA8
#define CST816S_REG_FW_VERSION          0xA9
#define CST816S_REG_MOTION_MASK         0xEC
#define CST816S_REG_IRQ_PULSE_WIDTH     0xED
#define CST816S_REG_NOR_SCAN_PER        0xEE
#define CST816S_REG_MOTION_S1_ANGLE     0xEF
#define CST816S_REG_LP_SCAN_RAW1H       0xF0
#define CST816S_REG_LP_SCAN_RAW1L       0xF1
#define CST816S_REG_LP_SCAN_RAW2H       0xF2
#define CST816S_REG_LP_SCAN_RAW2L       0xF3
#define CST816S_REG_LP_AUTO_WAKEUP_TIME 0xF4
#define CST816S_REG_LP_SCAN_TH          0xF5
#define CST816S_REG_LP_SCAN_WIN         0xF6
#define CST816S_REG_LP_SCAN_FREQ        0xF7
#define CST816S_REG_LP_SCAN_I_DAC       0xF8
#define CST816S_REG_AUTOSLEEP_TIME      0xF9
#define CST816S_REG_IRQ_CTL             0xFA
#define CST816S_REG_DEBOUNCE_TIME       0xFB
#define CST816S_REG_LONG_PRESS_TIME     0xFC
#define CST816S_REG_IOCTL               0xFD
#define CST816S_REG_DIS_AUTO_SLEEP      0xFE

#define CST816S_MOTION_EN_CON_LR        BIT(2)
#define CST816S_MOTION_EN_CON_UR        BIT(1)
#define CST816S_MOTION_EN_DCLICK        BIT(0)

#define CST816S_IRQ_EN_TEST             BIT(7)
#define CST816S_IRQ_EN_TOUCH            BIT(6)
#define CST816S_IRQ_EN_CHANGE           BIT(5)
#define CST816S_IRQ_EN_MOTION           BIT(4)
#define CST816S_IRQ_ONCE_WLP            BIT(0)

#ifndef CONFIG_INPUT_CST816S_INTERRUPT
#warning "This mode is untested with power management"
#endif

#define CST816S_IOCTL_SOFT_RTS          BIT(2)
#define CST816S_IOCTL_IIC_OD            BIT(1)
#define CST816S_IOCTL_EN_1V8            BIT(0)

#define CST816S_POWER_MODE_SLEEP        0x03
#define CST816S_POWER_MODE_EXPERIMENTAL 0x05

#define CST816S_EVENT_BITS_POS          0x06

#define CST816S_RESET_DELAY_MS          5
#define CST816S_WAIT_DELAY_MS           50

#define CST816S_GESTURE_NONE            0x00
#define CST816S_GESTURE_UP_SLIDING      0x01
#define CST816S_GESTURE_DOWN_SLIDING    0x02
#define CST816S_GESTURE_LEFT_SLIDE      0x03
#define CST816S_GESTURE_RIGHT_SLIDE     0x04
#define CST816S_GESTURE_CLICK           0x05
#define CST816S_GESTURE_DOUBLE_CLICK    0x0B
#define CST816S_GESTURE_LONG_PRESS      0x0C

#define EVENT_PRESS_DOWN                0x00
#define EVENT_LIFT_UP                   0x01
#define EVENT_CONTACT                   0x02
#define EVENT_NONE                      0x03

struct cst816s_config {
	struct i2c_dt_spec i2c;
	const struct gpio_dt_spec rst_gpio;

#ifdef CONFIG_INPUT_CST816S_INTERRUPT
	const struct gpio_dt_spec int_gpio;
#endif
};

struct cst816s_data {
	const struct device *dev;
	struct k_work work;

#ifdef CONFIG_INPUT_CST816S_INTERRUPT
	struct gpio_callback int_gpio_cb;
#else
	struct k_timer timer;
#endif
};

struct cst816s_output {
	uint8_t gesture;
	uint8_t points;
	uint16_t x;
	uint16_t y;
};

#ifdef CONFIG_PM_DEVICE
struct cst816s_lp_profile {
	uint8_t autosleep_time_s;
	uint8_t lp_auto_wake_time_min;
	uint8_t lp_scan_th;
	uint8_t lp_scan_win;
	uint8_t lp_scan_freq;
	uint8_t lp_scan_i_dac;
};

/* NOTE: I can not make a correlation between datasheet descriptions and actual behavior of those.
 * Those values comes from experimentation. This results in reliable low-power operation with good wake sensitivity.
 * Consumes about 80uA in suspend mode. Can probably be tuned more to reduce power further while keeping good wake sensitivity.
 */
 static const struct cst816s_lp_profile cst816s_suspend_profile = {
	.autosleep_time_s = 2,
	.lp_auto_wake_time_min = 3,
	.lp_scan_th = 50,
	.lp_scan_win = 100,
	.lp_scan_freq = 50,
	.lp_scan_i_dac = 200,
};
#endif

static int cst816s_process(const struct device *dev)
{
	uint8_t event;
	uint16_t row, col;
	bool is_pressed;

	struct cst816s_output output;
	const struct cst816s_config *cfg = dev->config;

	if (i2c_burst_read_dt(&cfg->i2c, CST816S_REG_GESTURE_ID, (uint8_t *)&output, sizeof(output)) < 0) {
		LOG_ERR("Could not read data");
		return -ENODATA;
	}

	col = sys_be16_to_cpu(output.x) & 0x0FFF;
	row = sys_be16_to_cpu(output.y) & 0x0FFF;

	event = (output.x & 0xFF) >> CST816S_EVENT_BITS_POS;
	is_pressed = (event == EVENT_PRESS_DOWN) || (event == EVENT_CONTACT);

	LOG_DBG("Event: %u", event);
	LOG_DBG("Pressed: %u", is_pressed);
	LOG_DBG("Gesture: %u", output.gesture);

	if (is_pressed) {
		input_report_abs(dev, INPUT_ABS_X, col, false, K_FOREVER);
		input_report_abs(dev, INPUT_ABS_Y, row, false, K_FOREVER);
		input_report_key(dev, INPUT_BTN_TOUCH, 1, true, K_FOREVER);
	} else {
		input_report_key(dev, INPUT_BTN_TOUCH, 0, true, K_FOREVER);
	}

#ifdef CONFIG_INPUT_CST816S_EV_DEVICE
	if (output.gesture != CST816S_GESTURE_CODE_NONE) {
		/* Also put the custom touch gestures into the input queue,
		* some applications may want to process it
		*/
		input_report(dev, INPUT_EV_DEVICE, (uint16_t)output.gesture, 0, true, K_FOREVER);
	}
#endif

	return 0;
}

static int cst816s_write_reg_checked(const struct i2c_dt_spec *i2c, uint8_t reg, uint8_t val,
				     const char *label)
{
	int ret = i2c_reg_write_byte_dt(i2c, reg, val);

	if (ret) {
		LOG_WRN("Reg write %s (0x%02x) -> 0x%02x failed (%d)", label, reg, val, ret);
	}

	return ret;
}

static void cst816s_work_handler(struct k_work *work)
{
	struct cst816s_data *data = CONTAINER_OF(work, struct cst816s_data, work);

	cst816s_process(data->dev);
}

#ifdef CONFIG_INPUT_CST816S_INTERRUPT
static void cst816s_isr_handler(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	struct cst816s_data *data = CONTAINER_OF(cb, struct cst816s_data, int_gpio_cb);

	k_work_submit(&data->work);
}
#else
static void cst816s_timer_handler(struct k_timer *timer)
{
	struct cst816s_data *data = CONTAINER_OF(timer, struct cst816s_data, timer);

	k_work_submit(&data->work);
}
#endif

static void cst816s_chip_reset(const struct device *dev)
{
	const struct cst816s_config *config = dev->config;
	int ret;

	if (gpio_is_ready_dt(&config->rst_gpio)) {
		ret = gpio_pin_configure_dt(&config->rst_gpio, GPIO_OUTPUT_ACTIVE);
		if (ret < 0) {
			LOG_ERR("Could not configure reset GPIO pin");
			return;
		}

		k_msleep(CST816S_RESET_DELAY_MS);
		gpio_pin_set_dt(&config->rst_gpio, 0);
		k_msleep(CST816S_WAIT_DELAY_MS);
	}
}

static int cst816s_chip_init(const struct device *dev)
{
	const struct cst816s_config *cfg = dev->config;
	uint8_t chip_id;

	cst816s_chip_reset(dev);

	if (!device_is_ready(cfg->i2c.bus)) {
		LOG_ERR("I2C bus %s not ready!", cfg->i2c.bus->name);
		return -ENODEV;
	}

	if (i2c_reg_read_byte_dt(&cfg->i2c, CST816S_REG_CHIP_ID, &chip_id) < 0) {
		LOG_ERR("Failed reading chip id!");
		return -ENODATA;
	}

	if ((chip_id != CST816S_CHIP_ID1) && (chip_id != CST816S_CHIP_ID2) &&
	    (chip_id != CST816S_CHIP_ID3)) {
		LOG_ERR("Wrong chip id: returned 0x%x!", chip_id);
		return -ENODEV;
	}

	if (i2c_reg_update_byte_dt(&cfg->i2c, CST816S_REG_MOTION_MASK, CST816S_MOTION_EN_DCLICK,
				   CST816S_MOTION_EN_DCLICK) < 0) {
		LOG_ERR("Could not enable double-click motion mask!");
		return -ENODATA;
	}

	if (i2c_reg_update_byte_dt(&cfg->i2c, CST816S_REG_IRQ_CTL,
				   CST816S_IRQ_EN_TOUCH | CST816S_IRQ_EN_CHANGE | CST816S_IRQ_EN_MOTION,
				   CST816S_IRQ_EN_TOUCH | CST816S_IRQ_EN_CHANGE | CST816S_IRQ_EN_MOTION) < 0) {
		LOG_ERR("Could not enable irq!");
		return -ENODATA;
	}

	return 0;
}

static int cst816s_init(const struct device *dev)
{
	struct cst816s_data *data = dev->data;

	data->dev = dev;
	k_work_init(&data->work, cst816s_work_handler);

	LOG_DBG("Initialize CST816S");

#ifdef CONFIG_INPUT_CST816S_INTERRUPT
	const struct cst816s_config *config = dev->config;

	if (!gpio_is_ready_dt(&config->int_gpio)) {
		LOG_ERR("GPIO port %s not ready!", config->int_gpio.port->name);
		return -EIO;
	}

	if (gpio_pin_configure_dt(&config->int_gpio, GPIO_INPUT) < 0) {
		LOG_ERR("Could not configure interrupt GPIO pin!");
		return -EIO;
	}

	if (gpio_pin_interrupt_configure_dt(&config->int_gpio, GPIO_INT_EDGE_TO_ACTIVE) < 0) {
		LOG_ERR("Could not configure interrupt GPIO interrupt!");
		return -EIO;
	}

	gpio_init_callback(&data->int_gpio_cb, cst816s_isr_handler, BIT(config->int_gpio.pin));

	if (gpio_add_callback(config->int_gpio.port, &data->int_gpio_cb) < 0) {
		LOG_ERR("Could not set gpio callback!");
		return -EIO;
	}
#else
	k_timer_init(&data->timer, cst816s_timer_handler, NULL);
	k_timer_start(&data->timer, K_MSEC(CONFIG_INPUT_CST816S_PERIOD), K_MSEC(CONFIG_INPUT_CST816S_PERIOD));
#endif

	int status = cst816s_chip_init(dev);
	if (status < 0) {
		return status;
	}

	return 0;
};

#ifdef CONFIG_PM_DEVICE
static int cst816s_apply_profile(const struct cst816s_config *cfg, const struct cst816s_lp_profile *p)
{
	int ret = 0;

	ret |= cst816s_write_reg_checked(&cfg->i2c, CST816S_REG_AUTOSLEEP_TIME, p->autosleep_time_s, "autosleep_time");
	ret |= cst816s_write_reg_checked(&cfg->i2c, CST816S_REG_LP_AUTO_WAKEUP_TIME, p->lp_auto_wake_time_min, "lp_auto_wake_time");
	ret |= cst816s_write_reg_checked(&cfg->i2c, CST816S_REG_LP_SCAN_TH, p->lp_scan_th, "lp_scan_th");
	ret |= cst816s_write_reg_checked(&cfg->i2c, CST816S_REG_LP_SCAN_WIN, p->lp_scan_win, "lp_scan_win");
	ret |= cst816s_write_reg_checked(&cfg->i2c, CST816S_REG_LP_SCAN_FREQ, p->lp_scan_freq, "lp_scan_freq");
	ret |= cst816s_write_reg_checked(&cfg->i2c, CST816S_REG_LP_SCAN_I_DAC, p->lp_scan_i_dac, "lp_scan_i_dac");

	if (ret) {
		return -EIO;
	}

	return 0;
}

static int cst816s_pm_action(const struct device *dev, enum pm_device_action action)
{
	int status = 0;
	const struct cst816s_config *cfg = dev->config;

	/* For some reason the CST816S does not respond to I2C commands after we use the standby profile.
	 * Workaround for now is to just always reset it before we change power modes.
	 */
	cst816s_chip_reset(dev);
	status = cst816s_chip_init(dev);
	LOG_WRN("PM action: %u, reset status: %d", action, status);

	switch (action) {
		case PM_DEVICE_ACTION_SUSPEND:
		/* For PM TURN_ON means device is in suspend mode, hence apply suspend profile. */
		case PM_DEVICE_ACTION_TURN_ON: {
			status = cst816s_apply_profile(cfg, &cst816s_suspend_profile);

			if (i2c_reg_write_byte_dt(&cfg->i2c, CST816S_REG_DIS_AUTO_SLEEP, 0x00) < 0) {
				LOG_WRN("Could not enable auto sleep");
			}
			break;
		}
		case PM_DEVICE_ACTION_TURN_OFF: {
			/* Put into Deep Sleep mode */
			status = i2c_reg_write_byte_dt(&cfg->i2c, CST816S_REG_SLEEP_MODE, CST816S_POWER_MODE_SLEEP);
			if (status < 0) {
				LOG_WRN("Could not enter deep sleep mode");
			}
			break;
		}
		case PM_DEVICE_ACTION_RESUME: {
			/* Nothing to do, device is already reset above. */
			break;
		}
		default: {
			return -ENOTSUP;
		}
	}

	return status;
}
#endif

#define CST816S_DEFINE(index)                                                                       \
	static struct cst816s_data cst816s_data_##index;                                                \
	static const struct cst816s_config cst816s_config_##index = {                                   \
		.i2c = I2C_DT_SPEC_INST_GET(index),                                                         \
		COND_CODE_1(CONFIG_INPUT_CST816S_INTERRUPT,                                             \
				(.int_gpio = GPIO_DT_SPEC_INST_GET(index, irq_gpios),), ())                         \
			.rst_gpio = GPIO_DT_SPEC_INST_GET_OR(index, rst_gpios, {}),                             \
	};                                                                                              \
																									\
	PM_DEVICE_DT_INST_DEFINE(index, cst816s_pm_action);                                             \
																									\
	DEVICE_DT_INST_DEFINE(index, cst816s_init, PM_DEVICE_DT_INST_GET(index), &cst816s_data_##index, \
				  &cst816s_config_##index, POST_KERNEL, CONFIG_INPUT_INIT_PRIORITY,                 \
				  NULL);

DT_INST_FOREACH_STATUS_OKAY(CST816S_DEFINE)
