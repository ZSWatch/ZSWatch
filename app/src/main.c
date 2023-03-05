#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/sys/reboot.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <buttons.h>
#include <battery.h>
#include <gpio_debug.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/settings/settings.h>
#include <zephyr/drivers/led.h>
#include <zephyr/logging/log.h>
#include <clock.h>
#include <lvgl.h>
#include "watchface_app.h"
#include <general_ui.h>
#include <heart_rate_sensor.h>
#include <sys/time.h>
#include <ram_retention_storage.h>
#include <accelerometer.h>
#include <ble_comm.h>
#include <ble_aoa.h>
#include <lv_notifcation.h>
#include <notification_manager.h>
#include <notifications_page.h>
#include <vibration_motor.h>
#include <display_control.h>
#include <applications/application_manager.h>
#include <events/ble_data_event.h>
#include <events/accel_event.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_WRN);

#define RENDER_INTERVAL_LVGL    K_MSEC(100)

typedef enum ui_state {
    INIT_STATE,
    SETTINGS_STATE,
    WATCHFACE_STATE,
    APPLICATION_MANAGER_STATE,
    NOTIFCATION_STATE,
    NOTIFCATION_LIST_STATE,
} ui_state_t;

void run_init_work(struct k_work *item);
static void lvgl_render(struct k_work *item);

static void enable_bluetoth(void);
static void open_notifications_page(void *unused);
static bool load_retention_ram(void);
static void enocoder_read(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static void encoder_vibration(struct _lv_indev_drv_t *drv, uint8_t e);
static void play_not_vibration(void);
static void ble_data_cb(ble_comm_cb_data_t *cb);
static void open_notification_popup(void *data);
static void async_turn_off_buttons_allocation(void *unused);
static void on_popup_notifcation_closed(uint32_t id);
static void on_notification_page_close(void);
static void on_notification_page_notification_close(uint32_t not_id);

static void onButtonPressCb(buttonPressType_t type, buttonId_t id);

static void on_close_application_manager(void);

static bool buttons_allocated = false;
static lv_group_t *input_group;
static lv_indev_drv_t enc_drv;
static lv_indev_t *enc_indev;
static buttonId_t last_pressed;

static bool display_on = true;
static bool vibrator_on = false;

static ui_state_t watch_state = INIT_STATE;

K_WORK_DEFINE(init_work, run_init_work);
K_WORK_DELAYABLE_DEFINE(lvgl_work, lvgl_render);

void run_init_work(struct k_work *item)
{
    load_retention_ram();
    heart_rate_sensor_init();
    notifications_page_init(on_notification_page_close, on_notification_page_notification_close);
    notification_manager_init();
    enable_bluetoth();
    accelerometer_init(NULL);
    clock_init(retained.current_time_seconds);
    buttonsInit(&onButtonPressCb);
    vibration_motor_init();
    vibration_motor_set_on(false);

    display_control_set_brightness(50);

    lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = enocoder_read;
    enc_drv.feedback_cb = encoder_vibration;
    enc_indev = lv_indev_drv_register(&enc_drv);

    input_group = lv_group_create();
    lv_group_set_default(input_group);
    lv_indev_set_group(enc_indev, input_group);

    watch_state = WATCHFACE_STATE;
    watchface_app_start(input_group);
    k_work_schedule(&lvgl_work, K_MSEC(1));
}

static void lvgl_render(struct k_work *item)
{
    const int64_t next_update_in_ms = lv_task_handler();
    k_work_schedule(&lvgl_work, K_MSEC(next_update_in_ms));
}

void main(void)
{
    // The init code requires a bit of stack.
    // So in order to not increase CONFIG_MAIN_STACK_SIZE and loose
    // this RAM forever, instead re-use the system workqueue for init
    // it has the required amount of stack.
    k_work_submit(&init_work);
}

static void enable_bluetoth(void)
{
    int err;

    err = bt_enable(NULL);
    if (err != 0) {
        LOG_ERR("Failed to enable Bluetooth, err: %d", err);
        return;
    }
#ifdef CONFIG_SETTINGS
    settings_load();
#endif

    //__ASSERT_NO_MSG(bleAoaInit());
    __ASSERT_NO_MSG(ble_comm_init(ble_data_cb) == 0);
}

static bool load_retention_ram(void)
{
    bool retained_ok = retained_validate();
    //memset(&retained, 0, sizeof(retained));
    /* Increment for this boot attempt and update. */
    retained.boots += 1;
    retained_update();

    printk("Retained data: %s\n", retained_ok ? "valid" : "INVALID");
    printk("Boot count: %u\n", retained.boots);
    printk("uptime_latest: %" PRIu64 "\n", retained.uptime_latest);
    printk("Active Ticks: %" PRIu64 "\n", retained.uptime_sum);

    return retained_ok;
}

static void open_notification_popup(void *data)
{
    not_mngr_notification_t *not = notification_manager_get_newest();
    if (not != NULL) {
        play_not_vibration();
        lv_notification_show(not->title, not->body, not->src, not->id, on_popup_notifcation_closed, 10);
        buttons_allocated = true;
    } else {
        buttons_allocated = false;
    }
}

static void open_notifications_page(void *unused)
{
    not_mngr_notification_t notifications[NOTIFICATION_MANAGER_MAX_STORED];
    int num_unread = 0;
    watchface_app_stop();
    buttons_allocated = true;
    watch_state = NOTIFCATION_LIST_STATE;
    notification_manager_get_all(notifications, &num_unread);
    notifications_page_create(notifications, num_unread, input_group);
}

static void open_application_manager_page(void *unused)
{
    watchface_app_stop();
    buttons_allocated = true;
    watch_state = APPLICATION_MANAGER_STATE;
    application_manager_show(on_close_application_manager, lv_scr_act(), input_group);
}

static void play_press_vibration(void)
{
    vibrator_on = true;
    vibration_motor_set_on(true);
    vibration_motor_set_power(80);
    k_msleep(150);
    vibration_motor_set_power(80);
    vibration_motor_set_on(false);
    vibrator_on = false;
}

static void play_not_vibration(void)
{
    vibrator_on = true;
    vibration_motor_set_power(100);
    vibration_motor_set_on(true);
    k_msleep(50);
    vibration_motor_set_on(false);
    k_msleep(50);
    vibration_motor_set_on(true);
    k_msleep(50);
    vibration_motor_set_on(false);
    k_msleep(50);
    vibration_motor_set_power(0);
    vibration_motor_set_on(true);
    k_msleep(50);
    vibration_motor_set_on(false);
    vibrator_on = false;
}

static void on_popup_notifcation_closed(uint32_t id)
{
    // Notification was dismissed, hence consider it read.
    // TODO send to phone that the notification was read.
    notification_manager_remove(id);
    lv_async_call(async_turn_off_buttons_allocation, NULL);
}

static void on_notification_page_close(void)
{
    lv_async_call(async_turn_off_buttons_allocation, NULL);
    watch_state = WATCHFACE_STATE;
    watchface_app_start(input_group);
}

static void on_notification_page_notification_close(uint32_t not_id)
{
    // TODO send to phone that the notification was read.
    notification_manager_remove(not_id);
}

static void on_close_application_manager(void)
{
    application_manager_delete();
    watch_state = WATCHFACE_STATE;
    watchface_app_start(input_group);
    lv_async_call(async_turn_off_buttons_allocation, NULL);
}

static void async_turn_off_buttons_allocation(void *unused)
{
    buttons_allocated = false;
}

static void onButtonPressCb(buttonPressType_t type, buttonId_t id)
{
    LOG_WRN("Pressed %d, type: %d", id, type);

    // Always allow force restart
    if (type == BUTTONS_LONG_PRESS && id == BUTTON_3) {
        retained.off_count += 1;
        retained_update();
        sys_reboot(SYS_REBOOT_COLD);
    }

    if (type == BUTTONS_LONG_PRESS && id == BUTTON_2 && watch_state == APPLICATION_MANAGER_STATE) {
        // TODO doesn't work, as this press is read later with lvgl and causes extra press in settings.
        // To fix each application must have exit button, maybe we can register long press on the whole view to exit
        // apps without input device
        //application_manager_exit_app();
        //return;
    }

    if (buttons_allocated) {
        // Handled by LVGL
        return;
    }

    if (type == BUTTONS_SHORT_PRESS && watch_state == WATCHFACE_STATE) {
        play_press_vibration();
        if (id == BUTTON_3) {
            LOG_DBG("Close Watchface, open App Manager");
            lv_async_call(open_application_manager_page, NULL);
        } else if (id == BUTTON_2) {
            LOG_DBG("CloseWatchface, open Notifications page");
            lv_async_call(open_notifications_page, NULL);
        } else {
            LOG_WRN("Unhandled button %d, type: %d, watch_state: %d", id, type, watch_state);
        }
    } else {
        if (id == BUTTON_3) {
            retained.off_count += 1;
            retained_update();
            sys_reboot(SYS_REBOOT_COLD);
        } else {
            LOG_WRN("Unhandled button %d, type: %d, watch_state: %d", id, type, watch_state);
        }
    }
}

static void enocoder_read(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    if (!buttons_allocated) {
        return;
    }
    if (button_read(BUTTON_1)) {
        data->key = LV_KEY_RIGHT;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = BUTTON_1;
    } else if (button_read(BUTTON_2)) {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = BUTTON_2;
    } else if (button_read(BUTTON_3)) {
        data->key = LV_KEY_RIGHT;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = BUTTON_3;
    } else {
        if (last_pressed == 0xFF) {
            return;
        }
        data->state = LV_INDEV_STATE_REL;
        switch (last_pressed) {
            case BUTTON_1:
                data->key = LV_KEY_RIGHT;
                break;
            case BUTTON_2:
                data->key = LV_KEY_ENTER;
                break;
            case BUTTON_3:
                data->key = LV_KEY_LEFT;
                break;
        }
        last_pressed = 0xFF;
    }
}

void encoder_vibration(struct _lv_indev_drv_t *drv, uint8_t e)
{
    if (e == LV_EVENT_PRESSED) {
        if (!vibrator_on) {
            play_press_vibration();
        }
    }
}

static void ble_data_cb(ble_comm_cb_data_t *cb)
{
    not_mngr_notification_t *parsed_not;

    switch (cb->type) {
        case BLE_COMM_DATA_TYPE_NOTIFY:
            parsed_not = notification_manager_add(&cb->data.notify);
            if (!parsed_not) {
                return;
            }
            if (buttons_allocated) {
                return;
            }

            buttons_allocated = true;
            lv_async_call(open_notification_popup, NULL);
            break;

        default:
            break;
    }
}

static bool app_event_handler(const struct app_event_header *aeh)
{
    if (is_ble_data_event(aeh)) {
        struct ble_data_event *event = cast_ble_data_event(aeh);

        switch (event->data.type) {
            case BLE_COMM_DATA_TYPE_NOTIFY:
                // TODO, this one not supported yet through events
                // Handled in ble_comm callback for now
                break;
            case BLE_COMM_DATA_TYPE_NOTIFY_REMOVE:
                if (notification_manager_remove(event->data.data.notify_remove.id) != 0) {
                    LOG_WRN("Notification %d not found", event->data.data.notify_remove.id);
                }
                break;
            case BLE_COMM_DATA_TYPE_SET_TIME: {
                struct timespec tspec;
                tspec.tv_sec = event->data.data.time.seconds;
                tspec.tv_nsec = 0;
                clock_settime(CLOCK_REALTIME, &tspec);
                break;
            }
            case BLE_COMM_DATA_TYPE_WEATHER:
                break;
            default:
                break;
        }
        return false;
    } else if (is_accel_event(aeh)) {
        struct accel_event *event = cast_accel_event(aeh);
        switch (event->data.type) {
        case ACCELEROMETER_EVT_TYPE_DOOUBLE_TAP: {
            if (vibrator_on || (watch_state != WATCHFACE_STATE)) {
                // Vibrator causes false double tap detections.
                // Need more work to not detect when vibration is running, hence for now only allow on watchface page
                break;
            }
            display_on = !display_on;
            if (display_on) {
                display_control_set_brightness(100);
            } else {
                display_control_set_brightness(1);
            }
            break;
        }
        case ACCELEROMETER_EVT_TYPE_XYZ: {
            LOG_ERR("x: %d y: %d z: %d", event->data.data.xyz.x, event->data.data.xyz.y, event->data.data.xyz.z);
            break;
        }
        case ACCELEROMETER_EVT_TYPE_TILT: {
            // Tilt detect not working as we want yet, so don't do for now.
            //display_control_set_brightness(100);
            break;
        }
        default:
            break;
        }
    }
    return false;
}

APP_EVENT_LISTENER(main, app_event_handler);
APP_EVENT_SUBSCRIBE(main, ble_data_event);
APP_EVENT_SUBSCRIBE(main, accel_event);