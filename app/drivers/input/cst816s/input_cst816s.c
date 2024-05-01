/*
 * Copyright (c) 2021 Qingsong Gou <gouqs@hotmail.com>
 * Copyright (c) 2022 Jakob Krantz <mail@jakobkrantz.se>
 * Copyright (c) 2023 Daniel Kampert <kontakt@daniel-kampert.de>
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

#define CST816S_CHIP_ID                 0xB4

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

#define CST816S_IOCTL_SOFT_RTS          BIT(2)
#define CST816S_IOCTL_IIC_OD            BIT(1)
#define CST816S_IOCTL_EN_1V8            BIT(0)

#define CST816S_POWER_MODE_SLEEP        0x03
#define CST816S_POWER_MODE_EXPERIMENTAL 0x05

#define CST816S_EVENT_BITS_POS          0x06

#define CST816S_RESET_DELAY             5  /* in ms */
#define CST816S_WAIT_DELAY              50 /* in ms */

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

LOG_MODULE_REGISTER(cst816s, CONFIG_INPUT_LOG_LEVEL);

static int cst816s_process(const struct device *dev)
{
	uint8_t event;
	uint16_t row, col;
	bool is_pressed;

	struct cst816s_output output;
	const struct cst816s_config *cfg = dev->config;

	if (i2c_burst_read_dt(&cfg->i2c, CST816S_REG_GESTURE_ID, (uint8_t * )&output, sizeof(output)) < 0) {
		LOG_ERR("Could not read data");
		return -ENODATA;
	}

	col = sys_be16_to_cpu(output.x) & 0x0fff;
	row = sys_be16_to_cpu(output.y) & 0x0fff;

	event = (output.x & 0xff) >> CST816S_EVENT_BITS_POS;
	is_pressed = (event == EVENT_CONTACT);

	LOG_DBG("Event: %u", event);
	LOG_DBG("Pressed: %u", is_pressed);
	LOG_DBG("Gesture: %u", output.gesture);

	if (is_pressed) {
		// These events are generated for the LVGL touch implementation.
		input_report_abs(dev, INPUT_ABS_X, col, false, K_FOREVER);
		input_report_abs(dev, INPUT_ABS_Y, row, false, K_FOREVER);
		input_report_key(dev, INPUT_BTN_TOUCH, 1, true, K_FOREVER);
	} else {
		// This event is generated for the LVGL touch implementation.
		input_report_key(dev, INPUT_BTN_TOUCH, 0, true, K_FOREVER);

		// These events are generated for common gesture events.
		switch (output.gesture) {
			case CST816S_GESTURE_LONG_PRESS: {
				break;
			}
			case CST816S_GESTURE_UP_SLIDING: {
				input_report_key(dev, INPUT_BTN_NORTH, 0, true, K_FOREVER);

				break;
			}
			case CST816S_GESTURE_DOWN_SLIDING: {
				input_report_key(dev, INPUT_BTN_SOUTH, 0, true, K_FOREVER);

				break;
			}
			case CST816S_GESTURE_LEFT_SLIDE: {
				input_report_key(dev, INPUT_BTN_WEST, 0, true, K_FOREVER);

				break;
			}
			case CST816S_GESTURE_RIGHT_SLIDE: {
				input_report_key(dev, INPUT_BTN_EAST, 0, true, K_FOREVER);

				break;
			}
			default: {
				break;
			}
		}
	}

	return 0;
}

static void cst816s_work_handler(struct k_work *work)
{
	struct cst816s_data *data = CONTAINER_OF(work, struct cst816s_data, work);

	cst816s_process(data->dev);
}

#ifdef CONFIG_INPUT_CST816S_INTERRUPT
static void cst816s_isr_handler(const struct device *dev, struct gpio_callback *cb, uint32_t mask)
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

	if (gpio_is_ready_dt(&config->rst_gpio)) {
		if (gpio_pin_configure_dt(&config->rst_gpio, GPIO_OUTPUT_INACTIVE) < 0) {
			LOG_ERR("Could not configure reset GPIO pin");
			return;
		}

		gpio_pin_set_dt(&config->rst_gpio, 1);
		k_msleep(CST816S_RESET_DELAY);
		gpio_pin_set_dt(&config->rst_gpio, 0);
		k_msleep(CST816S_WAIT_DELAY);
	}
}

static int cst816s_chip_init(const struct device *dev)
{
	const struct cst816s_config *cfg = dev->config;
	uint8_t chip_id;

	cst816s_chip_reset(dev);

	if (!device_is_ready(cfg->i2c.bus)) {
		LOG_ERR("I2C bus %s not ready", cfg->i2c.bus->name);
		return -ENODEV;
	}

	if (i2c_reg_read_byte_dt(&cfg->i2c, CST816S_REG_CHIP_ID, &chip_id) < 0) {
		LOG_ERR("failed reading chip id");
		return -ENODATA;
	}

	if (chip_id != CST816S_CHIP_ID) {
		LOG_ERR("CST816S wrong chip id: returned 0x%x", chip_id);
		return -ENODEV;
	}

	if (i2c_reg_update_byte_dt(&cfg->i2c, CST816S_REG_MOTION_MASK, CST816S_MOTION_EN_DCLICK, 0) < 0) {
		LOG_ERR("Could not set motion mask");
		return -ENODATA;
	}

	if (i2c_reg_update_byte_dt(&cfg->i2c, CST816S_REG_IRQ_CTL, CST816S_IRQ_EN_TOUCH | CST816S_IRQ_EN_CHANGE,
							   CST816S_IRQ_EN_TOUCH | CST816S_IRQ_EN_CHANGE) < 0) {
		LOG_ERR("Could not enable irq");
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
		LOG_ERR("GPIO port %s not ready", config->int_gpio.port->name);
		return -EIO;
	}

	if (gpio_pin_configure_dt(&config->int_gpio, GPIO_INPUT) < 0) {
		LOG_ERR("Could not configure interrupt GPIO pin");
		return -EIO;
	}

	if (gpio_pin_interrupt_configure_dt(&config->int_gpio, GPIO_INT_EDGE_TO_ACTIVE) < 0) {
		LOG_ERR("Could not configure interrupt GPIO interrupt.");
		return -EIO;
	}

	gpio_init_callback(&data->int_gpio_cb, cst816s_isr_handler, BIT(config->int_gpio.pin));

	if (gpio_add_callback(config->int_gpio.port, &data->int_gpio_cb) < 0) {
		LOG_ERR("Could not set gpio callback");
		return -EIO;
	}
#else
	k_timer_init(&data->timer, cst816s_timer_handler, NULL);
	k_timer_start(&data->timer, K_MSEC(CONFIG_INPUT_CST816S_PERIOD), K_MSEC(CONFIG_INPUT_CST816S_PERIOD));
#endif

	return cst816s_chip_init(dev);
};

#ifdef CONFIG_PM_DEVICE
static int cst816s_pm_action(const struct device *dev, enum pm_device_action action)
{
	int status = 0;

	LOG_DBG("Status: %u", action);

	switch (action) {
		case PM_DEVICE_ACTION_SUSPEND: {
			// Suspend/Resume only used to handle re-init after powered off.
			break;
		}
		case PM_DEVICE_ACTION_RESUME: {
			LOG_DBG("State changed to active");
			status = cst816s_chip_init(dev);

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
		COND_CODE_1(CONFIG_INPUT_CST816S_INTERRUPT,                                                 \
				(.int_gpio = GPIO_DT_SPEC_INST_GET(index, irq_gpios),), ())                         \
			.rst_gpio = GPIO_DT_SPEC_INST_GET_OR(index, rst_gpios, {}),                             \
	};                                                                                              \
																									\
	PM_DEVICE_DT_INST_DEFINE(index, cst816s_pm_action);                                             \
																									\
	DEVICE_DT_INST_DEFINE(index, cst816s_init, PM_DEVICE_DT_INST_GET(index), &cst816s_data_##index,                         \
				  &cst816s_config_##index, POST_KERNEL, CONFIG_INPUT_INIT_PRIORITY,                 \
				  NULL);

DT_INST_FOREACH_STATUS_OKAY(CST816S_DEFINE)