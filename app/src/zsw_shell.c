#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/shell/shell.h>
#include <zephyr/settings/settings.h>
#include <zephyr/input/input.h>
#include <zephyr/zbus/zbus.h>
#ifdef CONFIG_RETENTION_BOOT_MODE
#include <zephyr/retention/bootmode.h>
#endif

#include "zsw_settings.h"
#include <zsw_coredump.h>
#include <zsw_cpu_freq.h>
#include "fuel_gauge/zsw_pmic.h"
#include "managers/zsw_power_manager.h"
#include "managers/zsw_app_manager.h"
#include "drivers/zsw_vibration_motor.h"
#include "drivers/zsw_display_control.h"
#include "ui/zsw_ui_controller.h"
#include "events/battery_event.h"
#include "events/pressure_event.h"
#include "events/accel_event.h"
#if defined(CONFIG_DT_HAS_MAXIM_MAX32664C_ENABLED)
#include "drivers/zsw_hr.h"
#include "managers/zsw_hr_manager.h"
#endif

ZBUS_CHAN_DECLARE(battery_sample_data_chan);
ZBUS_CHAN_DECLARE(pressure_data_chan);
ZBUS_CHAN_DECLARE(accel_data_chan);

#if defined(CONFIG_THREAD_MONITOR) && defined(CONFIG_THREAD_STACK_INFO) && defined(CONFIG_INIT_STACKS)
struct stack_query {
    const struct shell *sh;
    const char *filter;
};

static void print_stack(const struct k_thread *thread, void *user_data)
{
    const struct stack_query *query = user_data;
    const char *name = k_thread_name_get((k_tid_t)thread);
    size_t unused;

    if (!name) {
        name = "unnamed";
    }
    if (query->filter && !strstr(name, query->filter)) {
        return;
    }
    if (k_thread_stack_space_get(thread, &unused)) {
        return;
    }
    shell_print(query->sh, "%s: size=%u used=%u free=%u", name,
                (unsigned)thread->stack_info.size,
                (unsigned)(thread->stack_info.size - unused), (unsigned)unused);
}

static int cmd_stack(const struct shell *sh, size_t argc, char **argv)
{
    struct stack_query query = {.sh = sh, .filter = argc > 1 ? argv[1] : NULL};
    k_thread_foreach_unlocked(print_stack, &query);
    return 0;
}

SHELL_CMD_ARG_REGISTER(stack, NULL, "Show stack high-water use; optional thread-name filter", cmd_stack, 1, 1);
#endif

#ifdef CONFIG_DT_HAS_NORDIC_NPM1300_ENABLED

static int cmd_battery(const struct shell *sh, size_t argc, char **argv)
{
    struct battery_sample_event sample;
    int ret = zsw_pmic_get_full_state(&sample);

    if (ret != 0) {
        shell_error(sh, "Failed to read battery status");
        return ret;
    }

    shell_print(sh, "Battery Status:");
    shell_print(sh, "  Voltage:     %d mV", sample.mV);
    shell_print(sh, "  Level:       %d%%", (int)sample.percent);
    shell_print(sh, "  Temperature: %.1f C", sample.temperature);
    shell_print(sh, "  Current:     %.2f mA", sample.avg_current * 1000);
    shell_print(sh, "  Charging:    %s", sample.is_charging ? "Yes" : "No");
    shell_print(sh, "  Time to empty: %.0f min", sample.tte);
    shell_print(sh, "  Time to full:  %.0f min", sample.ttf);
    shell_print(sh, "  VBUS:        %s", zsw_pmic_get_vbus_connected() ? "Connected" : "Disconnected");

    return 0;
}

SHELL_CMD_ARG_REGISTER(battery, NULL, "Show battery voltage, level, and charging status", cmd_battery, 1, 0);

#endif

static int cmd_factory_reset(const struct shell *sh, size_t argc, char **argv)
{
    int ret;

    shell_print(sh, "WARNING: This will erase all settings and reboot the device!");
    shell_print(sh, "Erasing settings...");

    ret = zsw_settings_erase_all();
    if (ret != 0) {
        shell_error(sh, "Failed to erase settings: %d", ret);
        return ret;
    }

    shell_print(sh, "Settings erased successfully.");
    shell_print(sh, "Rebooting in 2 seconds...");
    k_msleep(2000);

    sys_reboot(SYS_REBOOT_COLD);

    return 0;
}

SHELL_CMD_ARG_REGISTER(factory_reset, NULL, "Erase all settings and reboot device", cmd_factory_reset, 1, 0);

static int cmd_power_status(const struct shell *sh, size_t argc, char **argv)
{
    const char *state_str[] = {"Active", "Inactive", "Not worn/Stationary"};
    zsw_power_manager_state_t current_state = zsw_power_manager_get_state();
    uint32_t ms_to_inactive = zsw_power_manager_get_ms_to_inactive();

    shell_print(sh, "Power Status:");
    shell_print(sh, "  State: %s", state_str[current_state]);
    shell_print(sh, "  Time to sleep: %u seconds", ms_to_inactive / 1000);
    shell_print(sh, "  Total uptime: %llu seconds", k_uptime_get() / 1000);
    return 0;
}

static int cmd_power_wake(const struct shell *sh, size_t argc, char **argv)
{
    bool woke_up = zsw_power_manager_reset_idle_timout();
    shell_print(sh, "Idle timeout reset. %s", woke_up ? "Device woken up" : "Device already awake");
    return 0;
}

static bool power_sleep_stationary;
static int power_sleep_rc;

// Runs on the system workqueue like all other power state changes.
static void power_sleep_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    power_sleep_rc = zsw_power_manager_request_sleep();
    if (!power_sleep_rc && power_sleep_stationary) {
        struct accel_event event = {.data.type = ZSW_IMU_EVT_TYPE_NO_MOTION};
        power_sleep_rc = zbus_chan_pub(&accel_data_chan, &event, K_MSEC(250));
    }
}

static K_WORK_DEFINE(power_sleep_work, power_sleep_work_handler);

static int cmd_power_sleep(const struct shell *sh, size_t argc, char **argv)
{
    struct k_work_sync sync;

    if (argc == 2 && strcmp(argv[1], "stationary") != 0) {
        shell_error(sh, "Usage: power sleep [stationary]");
        return -EINVAL;
    }
    power_sleep_stationary = argc == 2;
    k_work_submit(&power_sleep_work);
    k_work_flush(&power_sleep_work, &sync);
    if (power_sleep_rc) {
        shell_error(sh, "Sleep failed, retry shortly (%d)", power_sleep_rc);
        return power_sleep_rc;
    }
    shell_print(sh, power_sleep_stationary ? "Screen asleep, injected NO_MOTION" : "Screen asleep");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_power,
                               SHELL_CMD_ARG(status, NULL, "Show power status and sleep timer", cmd_power_status, 1, 0),
                               SHELL_CMD_ARG(wake, NULL, "Reset idle timeout to keep device awake", cmd_power_wake, 1, 0),
                               SHELL_CMD_ARG(sleep, NULL, "Sleep now; optional stationary injects NO_MOTION", cmd_power_sleep, 1, 1),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(power, &sub_power, "Power management commands", NULL);

#if defined(CONFIG_DT_HAS_MAXIM_MAX32664C_ENABLED)
static int cmd_hr_measure(const struct shell *sh, size_t argc, char **argv)
{
    int rc = zsw_hr_manager_measure_now();
    shell_print(sh, "HR manual request: %d", rc);
    return rc;
}

static int cmd_hr_stop(const struct shell *sh, size_t argc, char **argv)
{
    int rc = zsw_hr_manager_set_mode(ZSW_HR_SETTING_OFF);
    shell_print(sh, "HR disabled: %d", rc);
    return rc;
}

static int cmd_hr_cancel(const struct shell *sh, size_t argc, char **argv)
{
    int rc = zsw_hr_manager_cancel_measurement();
    shell_print(sh, "HR manual cancel: %d", rc);
    return rc;
}

static int cmd_hr_mode(const struct shell *sh, size_t argc, char **argv)
{
    if (argc == 1) {
        struct zsw_hr_manager_snapshot s;
        zsw_hr_manager_get_snapshot(&s);
        shell_print(sh, "HR mode: %u (0=off, 1=background, 2=continuous)", s.mode);
        return 0;
    }
    zsw_hr_setting_t mode;
    if (!strcmp(argv[1], "off")) {
        mode = ZSW_HR_SETTING_OFF;
    } else if (!strcmp(argv[1], "background")) {
        mode = ZSW_HR_SETTING_BACKGROUND;
    } else if (!strcmp(argv[1], "continuous")) {
        mode = ZSW_HR_SETTING_CONTINUOUS;
    } else {
        return -EINVAL;
    }
    int rc = zsw_hr_manager_set_mode(mode);
    shell_print(sh, "HR mode set: %d", rc);
    return rc;
}

static int cmd_hr_motion(const struct shell *sh, size_t argc, char **argv)
{
    struct accel_event event = {0};
    if (!strcmp(argv[1], "moving")) {
        event.data.type = ZSW_IMU_EVT_TYPE_ANY_MOTION;
    } else if (!strcmp(argv[1], "stationary")) {
        event.data.type = ZSW_IMU_EVT_TYPE_NO_MOTION;
    } else {
        return -EINVAL;
    }
    int rc = zbus_chan_pub(&accel_data_chan, &event, K_MSEC(250));
    shell_print(sh, "Injected IMU event: %d", rc);
    return rc;
}

static int cmd_hr_status(const struct shell *sh, size_t argc, char **argv)
{
    struct zsw_hr_manager_snapshot s;
    zsw_hr_manager_get_snapshot(&s);
    shell_print(sh, "HR mode=%u status=%u manual=%u hardware=%u error=%d sequence=%u next=%lld ms",
                s.mode, s.status, s.manual, zsw_hr_is_running(), s.error, s.sequence,
                (long long)MAX(0, s.next_measurement_ms - k_uptime_get()));
    if (s.has_sample) {
        shell_print(sh, "Accepted HR: %d bpm confidence=%u age=%lld ms",
                    s.sample.heart_rate_bpm, s.sample.heart_rate_confidence,
                    (long long)(k_uptime_get() - s.sample.timestamp_ms));
    } else {
        shell_print(sh, "No accepted measurement");
    }
    if (s.has_report) {
        shell_print(sh, "Latest report: %d bpm confidence=%u skin=%u age=%lld ms",
                    s.report.heart_rate_bpm, s.report.heart_rate_confidence,
                    s.report.skin_contact_state, (long long)(k_uptime_get() - s.report.timestamp_ms));
    }
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_hr,
                               SHELL_CMD_ARG(measure, NULL, "Request one manual measurement", cmd_hr_measure, 1, 0),
                               SHELL_CMD_ARG(cancel, NULL, "Cancel the manual request", cmd_hr_cancel, 1, 0),
                               SHELL_CMD_ARG(stop, NULL, "Save mode off and stop all HR requests", cmd_hr_stop, 1, 0),
                               SHELL_CMD_ARG(mode, NULL, "Get/set off, background, continuous", cmd_hr_mode, 1, 1),
                               SHELL_CMD_ARG(motion, NULL, "Inject moving/stationary for policy tests", cmd_hr_motion, 2, 0),
                               SHELL_CMD_ARG(status, NULL, "Show manager state and the latest report", cmd_hr_status, 1, 0),
                               SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(hr, &sub_hr, "Heart rate manager", NULL);
#endif

static int cmd_vibration_run_pattern(const struct shell *sh, size_t argc, char **argv)
{
    zsw_vibration_pattern_t pattern;

    if (argc < 2) {
        shell_error(sh, "Missing pattern argument");
        return -EINVAL;
    }

    if (strcmp(argv[1], "click") == 0) {
        pattern = ZSW_VIBRATION_PATTERN_CLICK;
    } else if (strcmp(argv[1], "notification") == 0) {
        pattern = ZSW_VIBRATION_PATTERN_NOTIFICATION;
    } else if (strcmp(argv[1], "alarm") == 0) {
        pattern = ZSW_VIBRATION_PATTERN_ALARM;
    } else {
        shell_error(sh, "Unknown pattern '%s'", argv[1]);
        shell_print(sh, "Valid patterns: click, notification, alarm");
        return -EINVAL;
    }

    int ret = zsw_vibration_run_pattern(pattern);
    if (ret != 0) {
        shell_error(sh, "Failed to start vibration pattern (%d)", ret);
        return ret;
    }

    shell_print(sh, "Pattern started: %s", argv[1]);
    return 0;
}

static int cmd_vibration_help(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_help(sh);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_vibration,
                               SHELL_CMD_ARG(run_pattern, NULL, "Run vibration pattern <click|notification|alarm>", cmd_vibration_run_pattern, 2, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(vibration, &sub_vibration, "Vibration motor commands", cmd_vibration_help);

static int cmd_display_set_brightness(const struct shell *sh, size_t argc, char **argv)
{
    long brightness;
    char *endptr;

    if (argc < 2) {
        shell_error(sh, "Missing brightness value (0-100)");
        return -EINVAL;
    }

    brightness = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || brightness < 0 || brightness > 100) {
        shell_error(sh, "Invalid brightness '%s' (expected 0-100)", argv[1]);
        return -EINVAL;
    }

    zsw_display_control_set_brightness((uint8_t)brightness);
    shell_print(sh, "Brightness set to %ld%%", brightness);
    return 0;
}

static int cmd_display_get_brightness(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    shell_print(sh, "Current brightness: %u%%", zsw_display_control_get_brightness());
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_display,
                               SHELL_CMD_ARG(set_brightness, NULL, "Set display brightness percent", cmd_display_set_brightness, 2, 0),
                               SHELL_CMD_ARG(get_brightness, NULL, "Get current display brightness", cmd_display_get_brightness, 1, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(display, &sub_display, "Display control commands", NULL);

static int cmd_coredump_init(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int ret = zsw_coredump_init();
    if (ret != 0) {
        shell_error(sh, "Coredump init failed (%d)", ret);
        return ret;
    }

    shell_print(sh, "Coredump initialized");
    return 0;
}

static int cmd_coredump_to_log(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int ret = zsw_coredump_to_log();
    if (ret != 0) {
        shell_error(sh, "Failed to print coredump (%d)", ret);
    }
    return ret;
}

static int cmd_coredump_erase(const struct shell *sh, size_t argc, char **argv)
{
    long index;
    char *endptr;

    if (argc < 2) {
        shell_error(sh, "Missing coredump index");
        return -EINVAL;
    }

    index = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || index < 0) {
        shell_error(sh, "Invalid index '%s'", argv[1]);
        return -EINVAL;
    }

    zsw_coredump_erase((int)index);
    shell_print(sh, "Erased coredump index %ld", index);
    return 0;
}

static int cmd_coredump_summary(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    zsw_coredump_sumary_t summary = {0};
    int dumps = 0;
    int ret = zsw_coredump_get_summary(&summary, 1, &dumps);

    if (ret == -ENODATA) {
        shell_print(sh, "No stored coredumps");
        return 0;
    }

    if (ret != 0) {
        shell_error(sh, "Failed to read summary (%d)", ret);
        return ret;
    }

    if (dumps == 0) {
        shell_print(sh, "No stored coredumps");
        return 0;
    }

    shell_print(sh, "Stored coredumps: %d", dumps);
    shell_print(sh, "  Timestamp: %s", summary.datetime);
    shell_print(sh, "  File: %s", summary.file);
    shell_print(sh, "  Line: %d", summary.line);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_coredump,
                               SHELL_CMD_ARG(init, NULL, "Initialize coredump handling", cmd_coredump_init, 1, 0),
                               SHELL_CMD_ARG(log, NULL, "Print stored coredump to log", cmd_coredump_to_log, 1, 0),
                               SHELL_CMD_ARG(erase, NULL, "Erase stored coredump by index", cmd_coredump_erase, 2, 0),
                               SHELL_CMD_ARG(summary, NULL, "Show summary of stored coredump", cmd_coredump_summary, 1, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(coredump, &sub_coredump, "Coredump management commands", NULL);

static int cmd_cpu_get_freq(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    zsw_cpu_freq_t freq = zsw_cpu_get_freq();
    const char *freq_str = (freq == ZSW_CPU_FREQ_FAST) ? "fast" : "slow";

    shell_print(sh, "CPU frequency profile: %s", freq_str);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_cpu,
                               SHELL_CMD_ARG(freq, NULL, "Show current CPU frequency profile", cmd_cpu_get_freq, 1, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(cpu, &sub_cpu, "CPU frequency commands", cmd_cpu_get_freq);

#ifdef CONFIG_RETENTION_BOOT_MODE

static void boot_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    sys_reboot(SYS_REBOOT_COLD);
}

K_WORK_DELAYABLE_DEFINE(boot_work, boot_work_handler);

static int cmd_boot(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2 || strcmp(argv[1], "start") != 0) {
        shell_error(sh, "Usage: boot start");
        return -EINVAL;
    }

    shell_print(sh, "Preparing to enter serial recovery");
    int rc = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);
    if (rc != 0) {
        shell_error(sh, "Failed to set boot mode (%d)", rc);
        return rc;
    }

    shell_print(sh, "Rebooting into bootloader...");
    k_work_schedule(&boot_work, K_MSEC(500));

    return 0;
}

SHELL_CMD_REGISTER(boot, NULL, "Enter bootloader mode (start)", cmd_boot);

#endif /* CONFIG_RETENTION_BOOT_MODE */

static int cmd_app_list(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    int num_apps = zsw_app_manager_get_num_apps();

    shell_print(sh, "Registered apps (%d):", num_apps);
    for (int i = 0; i < num_apps; i++) {
        application_t *app = zsw_app_manager_get_app(i);
        if (app) {
            const char *state_str;
            switch (app->current_state) {
                case ZSW_APP_STATE_STOPPED:
                    state_str = "stopped";
                    break;
                case ZSW_APP_STATE_UI_VISIBLE:
                    state_str = "visible";
                    break;
                case ZSW_APP_STATE_UI_HIDDEN:
                    state_str = "hidden";
                    break;
                default:
                    state_str = "unknown";
                    break;
            }
            shell_print(sh, "  [%d] %s (%s)%s", i, app->name, state_str,
                        app->hidden ? " [hidden]" : "");
        }
    }
    return 0;
}

static int cmd_app_launch(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: app launch <name>");
        return -EINVAL;
    }

    // App names can have spaces, concatenate
    static char app_name_buf[64];
    app_name_buf[0] = '\0';
    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            strncat(app_name_buf, " ", sizeof(app_name_buf) - strlen(app_name_buf) - 1);
        }
        strncat(app_name_buf, argv[i], sizeof(app_name_buf) - strlen(app_name_buf) - 1);
    }

    // Wake the display so the launched app is visible
    zsw_power_manager_reset_idle_timout();

    if (zsw_ui_controller_get_state() != ZSW_UI_STATE_WATCHFACE) {
        shell_error(sh, "Cannot launch app: not on watchface");
        return -EBUSY;
    }

    shell_print(sh, "Launching app: %s", app_name_buf);
    zsw_ui_controller_launch_app(app_name_buf);
    return 0;
}

static int cmd_app_close(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    if (zsw_ui_controller_get_state() != ZSW_UI_STATE_APP_MANAGER) {
        shell_error(sh, "No app running");
        return -EINVAL;
    }

    shell_print(sh, "Closing current app");
    zsw_app_manager_exit_app();
    return 0;
}

static int cmd_app_state(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    const char *state_str;
    switch (zsw_ui_controller_get_state()) {
        case ZSW_UI_STATE_INIT:
            state_str = "init";
            break;
        case ZSW_UI_STATE_WATCHFACE:
            state_str = "watchface";
            break;
        case ZSW_UI_STATE_APP_MANAGER:
            state_str = "app_manager";
            break;
        default:
            state_str = "unknown";
            break;
    }

    const char *running_app = zsw_ui_controller_get_running_app_name();
    shell_print(sh, "UI state: %s", state_str);
    if (running_app) {
        shell_print(sh, "Running app: %s", running_app);
    }
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_app,
                               SHELL_CMD_ARG(list,   NULL, "List all registered apps", cmd_app_list, 1, 0),
                               SHELL_CMD_ARG(launch, NULL, "Launch app by name: app launch <name>", cmd_app_launch, 2, 10),
                               SHELL_CMD_ARG(close,  NULL, "Close the currently running app", cmd_app_close, 1, 0),
                               SHELL_CMD_ARG(state,  NULL, "Show current UI state and running app", cmd_app_state, 1, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(app, &sub_app, "App management commands", NULL);

/* --- touch simulation commands --- */

/* Convert display-space (x, y) to sensor-space using the inverse of the LVGL
 * pointer input transforms defined in the board DT overlay. */
#define LVGL_PTR DT_NODELABEL(lvgl_pointer_input)
static void touch_display_to_sensor(long x, long y, int32_t *sx, int32_t *sy)
{
    *sx = (int32_t)x;
    *sy = (int32_t)y;
#if DT_PROP_OR(LVGL_PTR, invert_y, 0)
    *sy = 239 - *sy;
#endif
#if DT_PROP_OR(LVGL_PTR, invert_x, 0)
    *sx = 239 - *sx;
#endif
#if DT_PROP_OR(LVGL_PTR, swap_xy, 0)
    {
        int32_t tmp = *sx;
        *sx = *sy;
        *sy = tmp;
    }
#endif
}
#undef LVGL_PTR

static int parse_touch_xy(const struct shell *sh, char **argv, long *x, long *y)
{
    char *endptr;
    *x = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || endptr == argv[1] || *x < 0 || *x > 239) {
        shell_error(sh, "Invalid x (0-239)");
        return -EINVAL;
    }
    *y = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || endptr == argv[2] || *y < 0 || *y > 239) {
        shell_error(sh, "Invalid y (0-239)");
        return -EINVAL;
    }
    return 0;
}

#if DT_HAS_COMPAT_STATUS_OKAY(zephyr_input_sdl_touch)
#define TOUCH_DEV DEVICE_DT_GET(DT_NODELABEL(input_sdl_touch))
#else
#define TOUCH_DEV DEVICE_DT_GET(DT_NODELABEL(cst816s))
#endif

static int cmd_touch(const struct shell *sh, size_t argc, char **argv)
{
    long x, y;
    if (parse_touch_xy(sh, argv, &x, &y) != 0) {
        return -EINVAL;
    }
    int32_t sx, sy;
    touch_display_to_sensor(x, y, &sx, &sy);

    const struct device *touch_dev = TOUCH_DEV;
    zsw_power_manager_reset_idle_timout();
    input_report_abs(touch_dev, INPUT_ABS_X, sx, false, K_FOREVER);
    input_report_abs(touch_dev, INPUT_ABS_Y, sy, false, K_FOREVER);
    input_report_key(touch_dev, INPUT_BTN_TOUCH, 1, true, K_FOREVER);
    k_msleep(50);
    input_report_key(touch_dev, INPUT_BTN_TOUCH, 0, true, K_FOREVER);
    return 0;
}

static int cmd_touch_down(const struct shell *sh, size_t argc, char **argv)
{
    long x, y;
    if (parse_touch_xy(sh, argv, &x, &y) != 0) {
        return -EINVAL;
    }
    int32_t sx, sy;
    touch_display_to_sensor(x, y, &sx, &sy);

    const struct device *touch_dev = TOUCH_DEV;
    zsw_power_manager_reset_idle_timout();
    input_report_abs(touch_dev, INPUT_ABS_X, sx, false, K_FOREVER);
    input_report_abs(touch_dev, INPUT_ABS_Y, sy, false, K_FOREVER);
    input_report_key(touch_dev, INPUT_BTN_TOUCH, 1, true, K_FOREVER);
    return 0;
}

static int cmd_touch_move(const struct shell *sh, size_t argc, char **argv)
{
    long x, y;
    if (parse_touch_xy(sh, argv, &x, &y) != 0) {
        return -EINVAL;
    }
    int32_t sx, sy;
    touch_display_to_sensor(x, y, &sx, &sy);

    const struct device *touch_dev = TOUCH_DEV;
    zsw_power_manager_reset_idle_timout();
    input_report_abs(touch_dev, INPUT_ABS_X, sx, false, K_FOREVER);
    input_report_abs(touch_dev, INPUT_ABS_Y, sy, true, K_FOREVER);
    return 0;
}

static int cmd_touch_up(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    const struct device *touch_dev = TOUCH_DEV;
    input_report_key(touch_dev, INPUT_BTN_TOUCH, 0, true, K_FOREVER);
    return 0;
}

SHELL_CMD_ARG_REGISTER(touch,     NULL, "Tap at display coords: touch <x> <y>",      cmd_touch,      3, 0);
SHELL_CMD_ARG_REGISTER(touchdown, NULL, "Press at display coords: touchdown <x> <y>", cmd_touch_down, 3, 0);
SHELL_CMD_ARG_REGISTER(touchmove, NULL, "Move to display coords: touchmove <x> <y>",  cmd_touch_move, 3, 0);
SHELL_CMD_ARG_REGISTER(touchup,   NULL, "Release touch: touchup",                     cmd_touch_up,   1, 0);

static int cmd_event_battery(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: event battery <percent> [charging:0|1]");
        return -EINVAL;
    }

    char *endptr;
    long val = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || endptr == argv[1] || val < 0 || val > 100) {
        shell_error(sh, "Invalid percent (0-100)");
        return -EINVAL;
    }
    int percent = (int)val;
    bool charging = (argc > 2 && strcmp(argv[2], "1") == 0);

    struct battery_sample_event evt = {
        .mV = 3700 + (percent * 5),
        .percent = percent,
        .temperature = 25.0f,
        .avg_current = charging ? -0.1f : 0.01f,
        .tte = charging ? 0 : (float)(percent * 6),
        .ttf = charging ? (float)((100 - percent) * 3) : 0,
        .is_charging = charging,
        .pmic_data_valid = true,
    };

    int ret = zbus_chan_pub(&battery_sample_data_chan, &evt, K_MSEC(100));
    if (ret != 0) {
        shell_error(sh, "Failed to publish battery event: %d", ret);
        return ret;
    }

    shell_print(sh, "Injected battery: %d%%, charging=%s", percent, charging ? "yes" : "no");
    return 0;
}

static int cmd_event_pressure(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: event pressure <hPa> [temperature_C]");
        return -EINVAL;
    }

    struct pressure_event evt = {
        .pressure = strtof(argv[1], NULL),
        .temperature = (argc > 2) ? strtof(argv[2], NULL) : 25.0f,
    };

    int ret = zbus_chan_pub(&pressure_data_chan, &evt, K_MSEC(100));
    if (ret != 0) {
        shell_error(sh, "Failed to publish pressure event: %d", ret);
        return ret;
    }

    shell_print(sh, "Injected pressure: %.2f hPa, temp: %.1f C",
                (double)evt.pressure, (double)evt.temperature);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_event,
                               SHELL_CMD_ARG(battery,  NULL, "Inject battery event: event battery <percent> [charging:0|1]",
                                             cmd_event_battery, 2, 1),
                               SHELL_CMD_ARG(pressure, NULL, "Inject pressure event: event pressure <hPa> [temp_C]",
                                             cmd_event_pressure, 2, 1),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(event, &sub_event, "Event injection commands", NULL);

/* --- microphone gain commands --- */
#if defined(CONFIG_ZSW_MIC)
#include "drivers/zsw_microphone.h"

static int cmd_mic_gain_get(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);

    uint8_t gain = zsw_microphone_get_gain();
    int db = ((int)gain - 0x28) * 20 / 0x28;
    shell_print(sh, "Mic gain: 0x%02x (%d dB approx)", (unsigned)gain, db);
    return 0;
}

static int cmd_mic_gain_set(const struct shell *sh, size_t argc, char **argv)
{
    if (argc < 2) {
        shell_error(sh, "Usage: mic gain_set <0-80>");
        return -EINVAL;
    }

    long val = strtol(argv[1], NULL, 10);
    if (val < 0 || val > 80) {
        shell_error(sh, "Gain must be 0-80 (0=mute, 40=0dB, 80=+20dB)");
        return -EINVAL;
    }

    zsw_microphone_set_gain((uint8_t)val);
    shell_print(sh, "Mic gain set to 0x%02x (%ld decimal)", (unsigned)val, val);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_mic,
                               SHELL_CMD_ARG(gain_get, NULL, "Show current PDM mic gain", cmd_mic_gain_get, 1, 0),
                               SHELL_CMD_ARG(gain_set, NULL, "Set PDM mic gain: mic gain_set <0-80>", cmd_mic_gain_set, 2, 0),
                               SHELL_SUBCMD_SET_END
                              );

SHELL_CMD_REGISTER(mic, &sub_mic, "Microphone commands", NULL);

#endif /* CONFIG_ZSW_MIC */
