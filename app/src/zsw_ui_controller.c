#include "zsw_ui_controller.h"
#include <stdint.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/pm/device.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/reboot.h>
#include <lvgl.h>
#include <filesystem/zsw_filesystem.h>
#include <zsw_retained_ram_storage.h>
#include "applications/watchface/watchface_app.h"
#include "drivers/zsw_display_control.h"
#include "drivers/zsw_vibration_motor.h"
#include "fuel_gauge/zsw_pmic.h"
#include "managers/zsw_app_manager.h"
#include "managers/zsw_notification_manager.h"
#include "managers/zsw_power_manager.h"
#include "ui/zsw_ui.h"

LOG_MODULE_REGISTER(zsw_ui_controller, CONFIG_ZSW_APP_LOG_LEVEL);

typedef enum ui_state {
    INIT_STATE,
    WATCHFACE_STATE,
    APPLICATION_MANAGER_STATE,
} ui_state_t;

static struct input_worker_item_t {
    struct k_work work;
    struct input_event event;
} input_worker_item;

static struct input_event last_input_event;

static bool is_buttons_for_lvgl = false;

static lv_obj_t *root_screen;
static lv_group_t *input_group;
static lv_group_t *temp_group;
static lv_indev_t *enc_indev;
static uint8_t last_pressed;

static void run_input_work(struct k_work *item);
static void handle_screen_gesture(lv_dir_t event_code);
static void encoder_read(lv_indev_t *indev, lv_indev_data_t *data);
static void on_input_subsys_callback(struct input_event *evt, void *user_data);
static void on_lvgl_screen_gesture_event_callback(lv_event_t *e);
static void on_watchface_app_event_callback(watchface_app_evt_t evt);
static void async_turn_off_buttons_allocation(void *unused);
static void open_application_manager_page(void *app_name);
static void on_application_manager_close(void);

K_WORK_DEFINE(input_work, run_input_work);

static ui_state_t watch_state = INIT_STATE;

static void run_input_work(struct k_work *item)
{
    struct input_worker_item_t *container = CONTAINER_OF(item, struct input_worker_item_t, work);

    LOG_DBG("Input worker code: %u", container->event.code);

    // Don't process the press if it caused wakeup.
    if (zsw_power_manager_reset_idle_timout()) {
        return;
    }

    switch (container->event.code) {
        // Button event
        case (INPUT_KEY_Y): {
#if(CONFIG_MISC_ENABLE_SYSTEM_RESET && (CONFIG_ZSWATCH_PCB_REV < 5))
            LOG_INF("Force restart");

            retained.off_count += 1;
            zsw_retained_ram_update();
            sys_reboot(SYS_REBOOT_COLD);
#endif
            break;
        }
        // Button event
        case INPUT_KEY_3: {
            if (zsw_notification_popup_is_shown()) {
                zsw_notification_popup_remove();
            } else if (watch_state == APPLICATION_MANAGER_STATE) {
                zsw_app_manager_exit_app();

                return;
            }

            break;
        }
        case INPUT_KEY_1: {
            if ((watch_state == WATCHFACE_STATE) && !zsw_notification_popup_is_shown()) {
                watch_state = APPLICATION_MANAGER_STATE;
                zsw_vibration_run_pattern(ZSW_VIBRATION_PATTERN_CLICK);
                lv_async_call(open_application_manager_page, NULL);
            }

            break;
        }
    }

    // Handle the input events. We have to take care about the screen orientation for the touch events.
    lv_dir_t gesture_code = LV_DIR_NONE;
    switch (container->event.code) {
        // Watch: Slide from right to left.
        case INPUT_BTN_NORTH: {
            gesture_code = LV_DIR_LEFT;
            break;
        }
        // Watch: Slide from left to right.
        case INPUT_BTN_SOUTH: {
            gesture_code = LV_DIR_RIGHT;
            break;
        }
        // Watch: Slide from bottom to top.
        case INPUT_BTN_WEST: {
            gesture_code = LV_DIR_TOP;
            break;
        }
        // Watch: Slide from top to bottom.
        case INPUT_BTN_EAST: {
            gesture_code = LV_DIR_BOTTOM;
            break;
        }
    }

    if (is_buttons_for_lvgl && (gesture_code == LV_DIR_NONE)) {
        // Handled by LVGL
        last_input_event.code = container->event.code;
        return;
    }
}

static void open_application_manager_page(void *app_name)
{
    watchface_app_stop();
    is_buttons_for_lvgl = true;
    watch_state = APPLICATION_MANAGER_STATE;
    zsw_app_manager_show(on_application_manager_close, root_screen, input_group, (char *)app_name);
}

void zsw_ui_controller_set_notification_mode(void)
{
    lv_group_set_default(temp_group);
    lv_indev_set_group(enc_indev, temp_group);
    is_buttons_for_lvgl = true;
}

void zsw_ui_controller_clear_notification_mode(void)
{
    lv_group_set_default(input_group);
    lv_indev_set_group(enc_indev, input_group);
    if (watch_state == WATCHFACE_STATE) {
        is_buttons_for_lvgl = 0;
    } else {
        is_buttons_for_lvgl = 1;
    }
}

static void on_application_manager_close(void)
{
    zsw_app_manager_delete();
    watch_state = WATCHFACE_STATE;
    watchface_app_start(root_screen, input_group, on_watchface_app_event_callback);
    lv_async_call(async_turn_off_buttons_allocation, NULL);
}

static void async_turn_off_buttons_allocation(void *unused)
{
    is_buttons_for_lvgl = false;
}

static void on_input_subsys_callback(struct input_event *evt, void *user_data)
{
    // Currently you have to define a keycode as binding between buttons and longpress. We skip this binding codes for now.
    // Also touch events will be skipped, because they are handled by LVGL.
    // TODO: Charger is also ignored for now.
    // TODO: Replace this filtering with a propper device filtering setup for the input handler
    if ((evt->code == INPUT_ABS_X) || (evt->code == INPUT_ABS_Y) || (evt->code == INPUT_BTN_TOUCH) ||
        (evt->code == INPUT_KEY_KP0) || (evt->code == INPUT_KEY_POWER) || (evt->value == 1)) {
        return;
    }

    input_worker_item.event = *evt;
    input_worker_item.work = input_work;
    k_work_submit(&input_worker_item.work);
}

static void handle_screen_gesture(lv_dir_t event_code)
{
    if (watch_state == WATCHFACE_STATE && !zsw_notification_popup_is_shown()) {
        switch (event_code) {
            case LV_DIR_LEFT: {
                open_application_manager_page("Notification");
                break;
            }
            case LV_DIR_RIGHT: {
                open_application_manager_page("Watchface Picker");
                break;
            }
            case LV_DIR_TOP: {
                open_application_manager_page(NULL);
                break;
            }
            case LV_DIR_BOTTOM: {
                break;
            }
            default:
                __ASSERT(false, "Not a valid gesture code: %d", event_code);
        }
        lv_indev_wait_release(lv_indev_get_act());
    } else if (zsw_notification_popup_is_shown()) {
        zsw_notification_popup_remove();
    }
}

static void on_lvgl_screen_gesture_event_callback(lv_event_t *e)
{
    lv_dir_t  dir;
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_GESTURE) {
        dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        handle_screen_gesture(dir);
    }
}

static void encoder_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (!is_buttons_for_lvgl) {
        return;
    }

    if (last_input_event.code == INPUT_KEY_4) {
        data->key = LV_KEY_LEFT;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = 2;
    } else if (last_input_event.code == INPUT_KEY_1) {
        data->key = LV_KEY_ENTER;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = 1;
    } else if (last_input_event.code == INPUT_KEY_2) {
        data->key = LV_KEY_RIGHT;
        data->state = LV_INDEV_STATE_PR;
        last_pressed = 3;
    } else if (last_input_event.code == INPUT_KEY_3) {
        // Not used for now. TODO exit/back button.
    } else {
        if (last_pressed == 0xFF) {
            return;
        }
        data->state = LV_INDEV_STATE_REL;
        switch (last_pressed) {
            case 2:
                data->key = LV_KEY_RIGHT;
                break;
            case 1:
                data->key = LV_KEY_ENTER;
                break;
            case 3:
                data->key = LV_KEY_LEFT;
                break;
            default:
                break;
        }
        last_pressed = 0xFF;
    }

    last_input_event.code = 0xFF;
}

static void handle_watchface_open_app_event(watchface_app_evt_open_app_t click)
{
    switch (click) {
        case WATCHFACE_APP_EVT_CLICK_BATT:
            open_application_manager_page("Battery");
            break;
        case WATCHFACE_APP_EVT_CLICK_STEP:
            open_application_manager_page("Fitness");
            break;
        case WATCHFACE_APP_EVT_CLICK_WEATHER:
            break;
        case WATCHFACE_APP_EVT_CLICK_MUSIC:
            open_application_manager_page("Music");
            break;
        case WATCHFACE_APP_EVT_CLICK_SETTINGS:
            open_application_manager_page("Settings");
            break;
        case WATCHFACE_APP_EVT_CLICK_FLASHLIGHT:
            open_application_manager_page("Flashlight");
            break;
        default:
            break;
    }
}

static void on_watchface_app_event_callback(watchface_app_evt_t evt)
{
    if (watch_state == WATCHFACE_STATE && !zsw_notification_popup_is_shown()) {
        switch (evt.type) {
            case WATCHFACE_APP_EVENT_OPEN_APP:
                handle_watchface_open_app_event(evt.data.app);
                break;
            case WATCHFACE_APP_EVENT_SET_BRIGHTNESS:
                zsw_display_control_set_brightness(evt.data.brightness);
                zsw_settings_brightness_t brightness = evt.data.brightness;
                if (evt.data.store_brightness) {
                    settings_save_one(ZSW_SETTINGS_BRIGHTNESS, &brightness, sizeof(brightness));
                }
                break;
            case WATCHFACE_APP_EVENT_RESTART:
                sys_reboot(SYS_REBOOT_COLD);
                break;
            case WATCHFACE_APP_EVENT_SHUTDOWN:
#if CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED
                int ret = zsw_pmic_power_down();
                if (ret == -ENOTSUP) {
                    // If the nPM1300 is charging thenpowering down/entering ship mode is not possible
                    // Instead we just do a reset
                    ret = zsw_pmic_reset();
                    if (ret) {
                        LOG_ERR("Failed to power down or reset the PMIC");
                    }
                }
#endif
                break;
        }
    }
}

int zsw_ui_controller_init(void)
{
    lv_indev_t *touch_indev;

    root_screen = lv_scr_act();

    lv_obj_set_style_bg_color(root_screen, zsw_color_dark_gray(), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Need to enable the gpio-keys as they are suspended by default
    pm_device_action_run(DEVICE_DT_GET(DT_NODELABEL(buttons)), PM_DEVICE_ACTION_RESUME);
    INPUT_CALLBACK_DEFINE(NULL, on_input_subsys_callback, NULL);

    enc_indev = lv_indev_create();
    lv_indev_set_type(enc_indev, LV_INDEV_TYPE_ENCODER);
    lv_indev_set_read_cb(enc_indev, encoder_read);
    lv_indev_set_group(enc_indev, input_group);

    input_group = lv_group_create();
    lv_group_set_default(input_group);
    lv_indev_set_group(enc_indev, input_group);
    temp_group = lv_group_create();

    touch_indev = lv_indev_get_next(NULL);
    while (touch_indev) {
        if (lv_indev_get_type(touch_indev) == LV_INDEV_TYPE_POINTER) {
            // TODO First fix so not all presses everyw
            //here are registered as clicks and cause vibration
            // Clicking anywhere with this below added right now will cause a vibration, which
            // is not what we want
            // touch_indev->driver->feedback_cb = click_feedback;
            break;
        }
        touch_indev = lv_indev_get_next(touch_indev);
    }

    watch_state = WATCHFACE_STATE;

    lv_obj_add_event_cb(root_screen, on_lvgl_screen_gesture_event_callback, LV_EVENT_GESTURE, NULL);

    watchface_app_start(root_screen, input_group, on_watchface_app_event_callback);

    LOG_INF("UI Controller initialized");

    return 0;
}
