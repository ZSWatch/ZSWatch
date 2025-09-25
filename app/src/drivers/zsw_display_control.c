/*
 * This file is part of ZSWatch project <https://github.com/zswatch/>.
 * Copyright (c) 2025 ZSWatch Project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "drivers/zsw_display_control.h"
#include "managers/zsw_xip_manager.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>
#include <zephyr/pm/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/regulator.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>
#include "lvgl.h"

#include <zephyr/drivers/counter.h>

LOG_MODULE_REGISTER(display_control, LOG_LEVEL_WRN);

#define DISPLAY_BRIGHTNESS_LEVELS 32

static void lvgl_render(struct k_work *item);
static void set_brightness_level(uint8_t brightness);
static void brightness_alarm_start_cb(const struct device *counter_dev, uint8_t chan_id, uint32_t ticks,
                                      void *user_data);
static void brightness_alarm_run_cb(const struct device *counter_dev, uint8_t chan_id, uint32_t ticks, void *user_data);
static void brightness_alarm_stop_cb(const struct device *counter_dev, uint8_t chan_id, uint32_t ticks,
                                     void *user_data);

typedef enum display_state {
    DISPLAY_STATE_AWAKE,
    DISPLAY_STATE_SLEEPING,
    DISPLAY_STATE_POWERED_OFF,
} display_state_t;

static const struct pwm_dt_spec display_blk = PWM_DT_SPEC_GET_OR(DT_ALIAS(display_blk), {});
static const struct device *counter_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(timer1));
static const struct device *const reg_dev = DEVICE_DT_GET_OR_NULL(DT_NODELABEL(regulator_3v3));
static const struct device *display_dev = DEVICE_DT_GET_OR_NULL(DT_CHOSEN(zephyr_display));
static const struct device *touch_dev =  DEVICE_DT_GET_OR_NULL(DT_NODELABEL(cst816s));

K_WORK_DELAYABLE_DEFINE(lvgl_work, lvgl_render);

K_MUTEX_DEFINE(display_mutex);
K_SEM_DEFINE(brightness_sem, 1, 1);

static struct k_work_sync cancel_work_sync;
static display_state_t display_state;
static bool first_render_since_poweron;
static uint8_t last_brightness = 1;
static struct counter_alarm_cfg bri_alarm_start, bri_alarm_run, bri_alarm_stop;

uint8_t current_driver_brightness_level = DISPLAY_BRIGHTNESS_LEVELS;

void zsw_display_control_init(void)
{
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Device display not ready.");
    }
    if (!device_is_ready(display_blk.dev)) {
        LOG_WRN("Display brightness control not supported");
    }
    if (!device_is_ready(reg_dev)) {
        LOG_WRN("Display regulator control not supported");
    }
    if (!device_is_ready(touch_dev)) {
        LOG_WRN("Device touch not ready.");
    }

    if (counter_dev) {
        bri_alarm_start.flags = 0;
        bri_alarm_start.callback = &brightness_alarm_start_cb;

        bri_alarm_run.flags = 0;
        bri_alarm_run.callback = &brightness_alarm_run_cb;

        bri_alarm_stop.flags = 0;
        bri_alarm_stop.callback = &brightness_alarm_stop_cb;

        bri_alarm_start.ticks = counter_us_to_ticks(counter_dev, 0);
        bri_alarm_run.ticks = counter_us_to_ticks(counter_dev, 750);
    }

    pm_device_action_run(display_dev, PM_DEVICE_ACTION_SUSPEND);
    if (device_is_ready(touch_dev)) {
        pm_device_action_run(touch_dev, PM_DEVICE_ACTION_SUSPEND);
    }

    display_state = DISPLAY_STATE_SLEEPING;
}

int zsw_display_control_sleep_ctrl(bool on)
{
    int res = -EALREADY;

    k_mutex_lock(&display_mutex, K_FOREVER);

    switch (display_state) {
        case DISPLAY_STATE_AWAKE:
            if (on) {
                LOG_DBG("Display already awake");
                res = -EALREADY;
            } else {
                LOG_DBG("Put display to sleep");
                // Cancel pending call to lv_task_handler
                // Or let it finish if it's running.
                k_work_cancel_delayable_sync(&lvgl_work, &cancel_work_sync);
                // Since actual flushing the data over SPI to the screen is done in a
                // thread in the display driver, we need to give it some time to complete
                // before we power off the display. If not the display will glitch.
                // 100 ms wait seems to be enough.
                k_msleep(100);
                display_state = DISPLAY_STATE_SLEEPING;
                display_blanking_on(display_dev);
                // Suspend the display
                pm_device_action_run(display_dev, PM_DEVICE_ACTION_SUSPEND);
                // Turn off PWM peripheral as it consumes like 200-250uA
                zsw_display_control_set_brightness(0);
                // Prepare for next call to lv_task_handler when screen is enabled again,
                // Since the display will have been powered off, we need to tell LVGL
                // to rerender the complete display.
                lv_obj_invalidate(lv_scr_act());
                // Disable XIP when display sleeps (no display-related XIP code will run)
                zsw_xip_disable();
                res = 0;
            }
            break;
        case DISPLAY_STATE_SLEEPING:
            if (on) {
                LOG_DBG("Wake up display");
                // Enable XIP before waking display (display code might be in XIP)
                zsw_xip_enable();
                display_state = DISPLAY_STATE_AWAKE;
                // Resume the display and touch chip
                pm_device_action_run(display_dev, PM_DEVICE_ACTION_RESUME);
                // Turn backlight on, unless the display was off,
                // then wait to show content until rendering completes.
                // This avoids user seeing random pixel data for ~500ms
                if (!first_render_since_poweron) {
                    zsw_display_control_set_brightness(last_brightness);
                }
                display_blanking_off(display_dev);
                k_work_schedule(&lvgl_work, K_MSEC(250));
                res = 0;
            } else {
                LOG_DBG("Display already sleeping");
                res = -EALREADY;
            }
            break;
        case DISPLAY_STATE_POWERED_OFF:
            if (on) {
                LOG_DBG("Display is OFF, power on before exiting sleep");
            } else {
                LOG_DBG("Display is OFF, cannot enter sleep");
            }
            res = -EIO;
            break;
    }

    k_mutex_unlock(&display_mutex);

    return res;
}

int zsw_display_control_pwr_ctrl(bool on)
{
    int res = -EALREADY;

    k_mutex_lock(&display_mutex, K_FOREVER);

    switch (display_state) {
        case DISPLAY_STATE_AWAKE:
            if (on) {
                LOG_DBG("Display awake, power already on");
            } else {
                LOG_DBG("Display awake, sleep before power off");
            }
            break;
        case DISPLAY_STATE_SLEEPING:
            if (on) {
                LOG_DBG("Display sleeping, power already on");
            } else {
                LOG_DBG("Display sleeping, power off");
                if (device_is_ready(reg_dev)) {
                    display_state = DISPLAY_STATE_POWERED_OFF;
#ifndef CONFIG_ARCH_POSIX
                    regulator_disable(reg_dev);
#endif
                    pm_device_action_run(display_dev, PM_DEVICE_ACTION_TURN_OFF);
                    if (device_is_ready(touch_dev)) {
                        pm_device_action_run(touch_dev, PM_DEVICE_ACTION_TURN_OFF);
                    }
                    res = 0;
                }
            }
            break;
        case DISPLAY_STATE_POWERED_OFF:
            if (on) {
                LOG_DBG("Display is off, power already on");
                if (device_is_ready(reg_dev)) {
                    display_state = DISPLAY_STATE_SLEEPING;
#ifndef CONFIG_ARCH_POSIX
                    regulator_enable(reg_dev);
#endif
                    // As the device pm state after TURN_ON is SUSPENDED
                    pm_device_action_run(display_dev, PM_DEVICE_ACTION_TURN_ON);
                    pm_device_action_run(display_dev, PM_DEVICE_ACTION_RESUME);
                    pm_device_action_run(display_dev, PM_DEVICE_ACTION_SUSPEND);

                    if (device_is_ready(touch_dev)) {
                        pm_device_action_run(touch_dev, PM_DEVICE_ACTION_TURN_ON);
                    }
                    first_render_since_poweron = true;
                    current_driver_brightness_level = DISPLAY_BRIGHTNESS_LEVELS;
                    res = 0;
                }
            } else {
                LOG_DBG("Display is off, power already off");
            }
            break;
    }

    k_mutex_unlock(&display_mutex);

    return res;
}

uint8_t zsw_display_control_get_brightness(void)
{
    return last_brightness;
}

void zsw_display_control_set_brightness(uint8_t percent)
{
    uint8_t level = 0;
    if (!device_is_ready(display_blk.dev)) {
        return;
    }
    __ASSERT(percent >= 0 && percent <= 100, "Invalid range for brightness, valid range 0-100, was %d", percent);

    k_mutex_lock(&display_mutex, K_FOREVER);

    // Convert percent to a value between 1 and DISPLAY_BRIGHTNESS_LEVELS
    if (percent > 0) {
        level = MAX(((double)percent / (double)100.0) * DISPLAY_BRIGHTNESS_LEVELS, 1);
        last_brightness = percent;
    }
    set_brightness_level(level);

    k_mutex_unlock(&display_mutex);
}

static void lvgl_render(struct k_work *item)
{
    const int64_t next_update_in_ms = lv_task_handler();
    if (first_render_since_poweron) {
        zsw_display_control_set_brightness(last_brightness);
        first_render_since_poweron = false;
    }
    k_work_schedule(&lvgl_work, K_MSEC(next_update_in_ms));
}

static void set_brightness_level(uint8_t brightness)
{
    uint8_t npulses;
    current_driver_brightness_level = MIN(brightness, DISPLAY_BRIGHTNESS_LEVELS);
    if (brightness == 0 || counter_dev == NULL) {
        pwm_set_pulse_dt(&display_blk,  display_blk.period);
        current_driver_brightness_level = DISPLAY_BRIGHTNESS_LEVELS;
        return;
    }

    // Must let current brightness setting complete before changing it again.
    __ASSERT(k_sem_take(&brightness_sem, K_MSEC(50)) == 0, "Failed to take brightness semaphore");
    npulses = DISPLAY_BRIGHTNESS_LEVELS - current_driver_brightness_level;

    bri_alarm_stop.ticks =
        bri_alarm_run.ticks +
        counter_us_to_ticks(counter_dev, display_blk.period * (npulses + 1) / NSEC_PER_USEC);
    counter_set_channel_alarm(counter_dev, 0, &bri_alarm_start);
    counter_set_channel_alarm(counter_dev, 1, &bri_alarm_run);
    counter_set_channel_alarm(counter_dev, 2, &bri_alarm_stop);
    counter_start(counter_dev);
}

static void brightness_alarm_start_cb(const struct device *counter_dev,
                                      uint8_t chan_id, uint32_t ticks,
                                      void *user_data)
{
    pwm_set_pulse_dt(&display_blk, display_blk.period);
}

static void brightness_alarm_run_cb(const struct device *counter_dev,
                                    uint8_t chan_id, uint32_t ticks,
                                    void *user_data)
{
    pwm_set_pulse_dt(&display_blk, display_blk.period / 2);
}

static void brightness_alarm_stop_cb(const struct device *counter_dev,
                                     uint8_t chan_id, uint32_t ticks,
                                     void *user_data)
{
    pwm_set_pulse_dt(&display_blk, 0);
    counter_stop(counter_dev);
    k_sem_give(&brightness_sem);
}
