/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/flash/nrf_qspi_nor.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/device.h>
#include <zephyr/sys/poweroff.h>
#include <zephyr/sys/util.h>

#define POWER_DEBUG_LOG_LEVEL LOG_LEVEL_INF

/* Tunable windows and actions (all in milliseconds unless noted otherwise) */
#define ACTIVE_WINDOW_MS          3000   /* time with all peripherals active */
#define SUSPENDED_WINDOW_MS       30000   /* time after peripheral suspend */
#define CUT_TO_SYSTEM_OFF_MS      30000    /* delay after 3V0 cut before System OFF */

LOG_MODULE_REGISTER(power_debug, POWER_DEBUG_LOG_LEVEL);

struct pm_target {
	const char *name;
	const struct device *dev;
};

const struct device *touch_dev = DEVICE_DT_GET(DT_NODELABEL(cst816s));

static const struct pm_target pm_targets[] = {
#if DT_NODE_HAS_STATUS(DT_NODELABEL(gc9a01), okay)
	{ "Display (GC9A01)", DEVICE_DT_GET(DT_NODELABEL(gc9a01)) },
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(cst816s), okay)
	{ "Touch (CST816S)", DEVICE_DT_GET(DT_NODELABEL(cst816s)) },
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(bmi270), okay) && CONFIG_ZSW_BMI270
	{ "IMU (BMI270)", DEVICE_DT_GET(DT_NODELABEL(bmi270)) },
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(bmp581), okay)
	{ "Pressure (BMP581)", DEVICE_DT_GET(DT_NODELABEL(bmp581)) },
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(apds9306), okay)
	{ "Light (APDS9306)", DEVICE_DT_GET(DT_NODELABEL(apds9306)) },
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(lis2mdl), okay)
	{ "Magnetometer (LIS2MDL)", DEVICE_DT_GET(DT_NODELABEL(lis2mdl)) },
#endif
#if DT_NODE_HAS_STATUS(DT_NODELABEL(rv_8263_c8), okay)
	{ "RTC (RV-8263)", DEVICE_DT_GET(DT_NODELABEL(rv_8263_c8)) },
#endif
};

static bool peripherals_suspended;
static bool regulator_cut;
static int64_t system_off_deadline_ms = -1;
static int64_t stage_deadline_ms = -1;

enum power_stage {
	STAGE_ACTIVE,
	STAGE_SUSPEND_PENDING,
	STAGE_CUT_PENDING,
	STAGE_DONE,
};

static enum power_stage current_stage = STAGE_ACTIVE;

static void system_off_handler(struct k_work *work);
static void suspend_peripherals(void);
static void stage_handler(struct k_work *work);
static void schedule_system_off_ms(int64_t delay_ms);
static void schedule_stage_in_ms(int32_t delay_ms);

K_WORK_DELAYABLE_DEFINE(system_off_work, system_off_handler);
K_WORK_DELAYABLE_DEFINE(stage_work, stage_handler);

static void power_down_external_flash(void)
{
#if DT_NODE_HAS_STATUS(DT_NODELABEL(mx25u51245g), okay)
	const struct device *flash = DEVICE_DT_GET(DT_NODELABEL(mx25u51245g));

	if (!device_is_ready(flash)) {
		LOG_WRN("External flash not ready");
		return;
	}

	int err = pm_device_action_run(flash, PM_DEVICE_ACTION_SUSPEND);
	if (err == -EALREADY) {
		LOG_DBG("External flash already in low-power state");
	} else if (err) {
		LOG_WRN("Failed to suspend external flash (%d)", err);
	} else {
		LOG_INF("External flash suspended (DPD)");
	}
#endif
}

static void cut_main_regulator(void)
{
#if DT_NODE_HAS_STATUS(DT_NODELABEL(regulator_3v3), okay)
	const struct device *reg = DEVICE_DT_GET(DT_NODELABEL(regulator_3v3));

	if (regulator_cut) {
		return;
	}

	if (!device_is_ready(reg)) {
		LOG_WRN("3V0 regulator device not ready");
		return;
	}

	int err = regulator_disable(reg);
	if ((err == -EALREADY) || (err == 0)) {
		LOG_INF("3V0 regulator disabled");
		regulator_cut = true;
	} else {
		LOG_WRN("Failed to disable 3V0 regulator (%d)", err);
	}
#else
	LOG_WRN("3V0 regulator node not present");
#endif
}

static void schedule_stage_in_ms(int32_t delay_ms)
{
	if (delay_ms < 0) {
		delay_ms = 0;
	}

	stage_deadline_ms = k_uptime_get() + delay_ms;
	k_work_reschedule(&stage_work, K_MSEC(delay_ms));
}

static void log_reset_cause(void)
{
	uint32_t reset_cause = 0;
	int rc = hwinfo_get_reset_cause(&reset_cause);

	if (rc != 0) {
		LOG_WRN("Could not read reset cause (%d)", rc);
		return;
	}

	if (reset_cause == 0) {
		LOG_INF("Reset cause not available");
		return;
	}

	LOG_INF("Reset cause mask: 0x%08x", reset_cause);

	if (reset_cause & RESET_DEBUG) {
		LOG_INF("Reset by debugger");
	} else if (reset_cause & RESET_PIN) {
		LOG_INF("Reset from pin reset");
	} else if (reset_cause & RESET_SOFTWARE) {
		LOG_INF("Reset from software");
	}

	if (reset_cause & RESET_LOW_POWER_WAKE) {
		LOG_INF("Reset from low power wake (System OFF exit)");
	}

	if (reset_cause & RESET_CLOCK) {
		LOG_INF("Reset caused by clock/power event (possible timer wake)");
	}
}

static void suspend_peripherals(void)
{
	if (peripherals_suspended) {
		return;
	}

	power_down_external_flash();

	for (size_t i = 0; i < ARRAY_SIZE(pm_targets); i++) {
		const struct pm_target *target = &pm_targets[i];

		if (!device_is_ready(target->dev)) {
			LOG_DBG("%s not ready; skipping", target->name);
			continue;
		}

		int err = pm_device_action_run(target->dev, PM_DEVICE_ACTION_SUSPEND);
		if (err == -ENOTSUP) {
			LOG_DBG("%s does not support PM_DEVICE_ACTION_SUSPEND", target->name);
		} else if (err == -EALREADY) {
			LOG_DBG("%s already suspended", target->name);
		} else if (err) {
			LOG_WRN("Suspending %s failed (%d)", target->name, err);
		} else {
			LOG_INF("%s suspended", target->name);
		}
	}

	peripherals_suspended = true;
}

static void stage_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	switch (current_stage) {
	case STAGE_ACTIVE:
		LOG_INF("Active window done -> suspending peripherals");
		suspend_peripherals();
		current_stage = STAGE_SUSPEND_PENDING;
		schedule_stage_in_ms(SUSPENDED_WINDOW_MS);
		break;
	case STAGE_SUSPEND_PENDING:
		LOG_INF("Suspended window done -> cutting 3V0 and scheduling System OFF");
		cut_main_regulator();
		current_stage = STAGE_CUT_PENDING;
		stage_deadline_ms = -1;
		schedule_system_off_ms(CUT_TO_SYSTEM_OFF_MS);
		break;
	default:
		break;
	}
}

static void system_off_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	LOG_INF("Entering System OFF");

	/* Make one last attempt to deep-power-down external flash */
	peripherals_suspended = false;
	suspend_peripherals();
	cut_main_regulator();

	const struct device *cons = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(zephyr_console));
	if (cons && device_is_ready(cons)) {
		int err = pm_device_action_run(cons, PM_DEVICE_ACTION_SUSPEND);
		if (err && err != -EALREADY) {
			LOG_WRN("Console suspend failed (%d)", err);
		}
	}

	hwinfo_clear_reset_cause();
	sys_poweroff();
}

static void schedule_system_off_ms(int64_t delay_ms)
{
	if (delay_ms < 0) {
		delay_ms = 0;
	}

	system_off_deadline_ms = k_uptime_get() + delay_ms;

	k_work_reschedule(&system_off_work, K_MSEC(delay_ms));

	LOG_INF("System OFF scheduled in %lld ms", (long long)delay_ms);
}

int main(void)
{
	LOG_INF("Power debug application booting on %s", CONFIG_BOARD);

	log_reset_cause();

	LOG_INF("Sequence:");
	LOG_INF("  Active for %d ms -> suspend peripherals for %d ms -> cut 3V0 -> System OFF",
		ACTIVE_WINDOW_MS, SUSPENDED_WINDOW_MS);

	/* Start the staged shutdown pipeline */
	peripherals_suspended = false;
	regulator_cut = false;
	current_stage = STAGE_ACTIVE;
	schedule_stage_in_ms(ACTIVE_WINDOW_MS);

	while (true) {
		k_sleep(K_SECONDS(1));
	}

	return 0;
}
